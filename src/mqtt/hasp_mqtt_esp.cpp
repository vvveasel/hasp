/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#include "hasp_conf.h"

#if HASP_USE_MQTT > 0
#ifdef HASP_USE_ESP_MQTT

#include "mqtt_client.h"
#include "esp_crt_bundle.h"
#include "Preferences.h"

#include "hasp/hasp.h"
#include "hasp_mqtt.h"
#include "hasp_mqtt_ha.h"

#include "hal/hasp_hal.h"
#include "hasp_debug.h"
#include "hasp_config.h"
#include "hasp_gui.h"

#include "../hasp/hasp_dispatch.h"
#include "freertos/queue.h"

QueueHandle_t queue;
typedef struct
{
    char* topic;   //[64];
    char* payload; //[512];
} mqtt_message_t;

char mqttLwtTopic[28];
char mqttNodeTopic[24];
char mqttClientId[64];
char mqttGroupTopic[24];
bool mqttEnabled        = false;
bool mqttHAautodiscover = true;
uint32_t mqttPublishCount;
uint32_t mqttReceiveCount;
uint32_t mqttFailedCount;

char mqttServer[MAX_HOSTNAME_LENGTH]   = MQTT_HOSTNAME;
char mqttUsername[MAX_USERNAME_LENGTH] = MQTT_USERNAME;
char mqttPassword[MAX_PASSWORD_LENGTH] = MQTT_PASSWORD;
// char mqttNodeName[16]  = MQTT_NODENAME;
char mqttGroupName[16] = MQTT_GROUPNAME;
uint16_t mqttPort      = MQTT_PORT;
esp_mqtt_client_handle_t mqttClient;
static esp_mqtt_client_config_t mqtt_cfg;

extern const uint8_t rootca_crt_bundle_start[] asm("_binary_data_cert_x509_crt_bundle_bin_start");
extern const uint8_t rootca_crt_bundle_end[] asm("_binary_data_cert_x509_crt_bundle_bin_end");

bool last_mqtt_state            = false;
bool current_mqtt_state         = false;
uint16_t mqtt_reconnect_counter = 0;

void mqtt_run_scripts()
{
    if(last_mqtt_state != current_mqtt_state) {
        // mqtt_message_t data;
        // snprintf(data.topic, sizeof(data.topic), "run");

        // if(current_mqtt_state) {
        //     snprintf(data.payload, sizeof(data.payload), "L:/mqtt_on.cmd");
        //     // networkStart();
        // } else {
        //     snprintf(data.payload, sizeof(data.payload), "L:/mqtt_off.cmd");
        //     // networkStop();
        // }

        // size_t attempt = 0;
        // while(xQueueSend(queue, &data, (TickType_t)0) == errQUEUE_FULL && attempt < 100) {
        //     vTaskDelay(5 / portTICK_PERIOD_MS);
        //     attempt++;
        // };

        if(current_mqtt_state) {
            dispatch_run_script(NULL, "L:/mqtt_on.cmd", TAG_HASP);
        } else {
            dispatch_run_script(NULL, "L:/mqtt_off.cmd", TAG_HASP);
        }

        last_mqtt_state = current_mqtt_state;
    }
}

void mqtt_disconnected()
{
    current_mqtt_state = false; // now we are disconnected
    mqtt_run_scripts();
    mqtt_reconnect_counter++;
}

void mqtt_connected()
{
    if(!current_mqtt_state) {
        mqtt_reconnect_counter = 0;
        current_mqtt_state     = true; // now we are connected
        printf("%s", current_mqtt_state ? PSTR(D_SERVICE_CONNECTED) : PSTR(D_SERVICE_DISCONNECTED));
    }
    mqtt_run_scripts();
}

int mqttPublish(const char* topic, const char* payload, size_t len, bool retain)
{
    if(!mqttEnabled) return MQTT_ERR_DISABLED;

    // Write directly to the client, don't use the buffer
    if(current_mqtt_state && esp_mqtt_client_publish(mqttClient, topic, payload, len, 0, retain) != ESP_FAIL) {

        // Enqueue a message to the outbox, to be sent later
        // if(current_mqtt_state && esp_mqtt_client_enqueue(mqttClient, topic, payload, len, 0, retain, true) !=
        // ESP_FAIL) {
        mqttPublishCount++;
        return MQTT_ERR_OK;
    }

    mqttFailedCount++;
    return current_mqtt_state ? MQTT_ERR_PUB_FAIL : MQTT_ERR_NO_CONN;
}

int mqttPublish(const char* topic, const char* payload, bool retain)
{
    return mqttPublish(topic, payload, strlen(payload), retain);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Send changed values OUT

bool mqttIsConnected()
{
    return mqttEnabled && current_mqtt_state;
}

bool mqtt_send_lwt(bool online)
{
    char tmp_payload[8];
    // char tmp_topic[strlen(mqttNodeTopic) + 4];

    // strncpy(tmp_topic, mqttNodeTopic, sizeof(tmp_topic));
    // strncat_P(tmp_topic, PSTR(MQTT_TOPIC_LWT), sizeof(tmp_topic));

    size_t len = snprintf_P(tmp_payload, sizeof(tmp_payload), online ? PSTR("online") : PSTR("offline"));
    bool res   = mqttPublish(mqttLwtTopic, tmp_payload, len, true);
    return res;
}

int mqtt_send_object_state(uint8_t pageid, uint8_t btnid, const char* payload)
{
    char tmp_topic[strlen(mqttNodeTopic) + 16];
    snprintf_P(tmp_topic, sizeof(tmp_topic), PSTR("%s" MQTT_TOPIC_STATE "/" HASP_OBJECT_NOTATION), mqttNodeTopic,
               pageid, btnid);
    return mqttPublish(tmp_topic, payload, false);
}

int mqtt_send_state(const char* subtopic, const char* payload)
{
    char tmp_topic[strlen(mqttNodeTopic) + strlen(subtopic) + 16];
    snprintf_P(tmp_topic, sizeof(tmp_topic), PSTR("%s" MQTT_TOPIC_STATE "/%s"), mqttNodeTopic, subtopic);
    return mqttPublish(tmp_topic, payload, false);
}

int mqtt_send_discovery(const char* payload, size_t len)
{
    char tmp_topic[128];
    snprintf_P(tmp_topic, sizeof(tmp_topic), PSTR(MQTT_PREFIX "/" MQTT_TOPIC_DISCOVERY "/%s"),
               haspDevice.get_hardware_id());
    return mqttPublish(tmp_topic, payload, len, false);
}

static inline size_t mqtt_msg_length(size_t len)
{
    return (len / 64) * 64 + 64;
}

void mqtt_process_topic_payload(const char* topic, const char* payload, unsigned int length)
{
    if(gui_acquire()) {
        mqttLoop(); // First empty the MQTT queue
        printf("%s = %s", topic, payload);
        dispatch_topic_payload(topic, payload, length > 0, TAG_MQTT);
        gui_release();
    } else {
        // Add new message to the queue
        mqtt_message_t data;

        size_t topic_len   = strlen(topic);
        size_t payload_len = length;
        data.topic         = (char*)hasp_calloc(sizeof(char), mqtt_msg_length(topic_len + 1));
        data.payload       = (char*)hasp_calloc(sizeof(char), mqtt_msg_length(payload_len + 1));

        if(!data.topic || !data.payload) {
            printf(D_ERROR_OUT_OF_MEMORY);
            hasp_free(data.topic);
            hasp_free(data.payload);
            return;
        }
        memcpy(data.topic, topic, topic_len);
        memcpy(data.payload, payload, payload_len);

        {
            size_t attempt = 0;
            while(xQueueSend(queue, &data, (TickType_t)0) == errQUEUE_FULL && attempt < 100) {
                delay(5);
                attempt++;
            };
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Receive incoming messages
static void mqtt_message_cb(const char* topic, byte* payload, unsigned int length)
{ // Handle incoming commands from MQTT
    mqttReceiveCount++;
    // LOG_TRACE(TAG_MQTT_RCV, F("%s = %s"), topic, (char*)payload);

    if(topic == strstr(topic, mqttNodeTopic)) { // startsWith mqttNodeTopic

        // Node topic
        topic += strlen(mqttNodeTopic); // shorten topic

    } else if(topic == strstr(topic, mqttGroupTopic)) { // startsWith mqttGroupTopic

        // Group topic
        topic += strlen(mqttGroupTopic); // shorten topic

#ifdef HASP_USE_BROADCAST
    } else if(topic == strstr_P(topic, PSTR(MQTT_PREFIX "/" MQTT_TOPIC_BROADCAST "/"))) { // broadcast  topic

        // Broadcast topic
        topic += strlen_P(PSTR(MQTT_PREFIX "/" MQTT_TOPIC_BROADCAST "/")); // shorten topic
#endif

#ifdef HASP_USE_HA
    } else if(topic == strstr_P(topic, PSTR("homeassistant/status"))) { // HA discovery topic
        if(mqttHAautodiscover && !strcasecmp_P((char*)payload, PSTR("online"))) {
            mqtt_ha_register_auto_discovery(); // auto-discovery first
            dispatch_current_state(TAG_MQTT);  // send the data
        }
        return;
#endif

    } else {
        // Other topic
        printf(D_MQTT_INVALID_TOPIC);
        return;
    }

    // catch a dangling LWT from a previous connection if it appears
    /*    if(!strcmp_P(topic, PSTR(MQTT_TOPIC_LWT))) { // endsWith LWT
            if(!strcasecmp_P((char*)payload, PSTR("offline"))) {
                {
                    char msg[8];
                    char tmp_topic[strlen(mqttNodeTopic) + 8];
                    snprintf_P(tmp_topic, sizeof(tmp_topic), PSTR("%s" MQTT_TOPIC_LWT), mqttNodeTopic);
                    snprintf_P(msg, sizeof(msg), PSTR("online"));

                    // bool res =
                    mqttClient.publish(tmp_topic, msg, true);
                    mqttPublish(tmp_topic, msg, true);
                }
                }
                else
                {
                    // LOG_TRACE(TAG_MQTT, F("ignoring LWT = online"));
                }
                }
                else */

    {
        mqtt_process_topic_payload(topic, (const char*)payload, length);
    }

    /*    {
            mqtt_message_t data;
            snprintf(data.topic, sizeof(data.topic), topic);
            snprintf(data.payload, sizeof(data.payload), (const char*)payload);
            size_t attempt = 0;
            while(xQueueSend(queue, &data, (TickType_t)0) == errQUEUE_FULL && attempt < 100) {
                vTaskDelay(5 / portTICK_PERIOD_MS);
                attempt++;
            };
            // dispatch_topic_payload(topic, (const char*)payload, length > 0, TAG_MQTT);
        }  */
}

static void mqttSubscribeTo(const char* topic)
{
    if(esp_mqtt_client_subscribe(mqttClient, topic, 0) == ESP_FAIL) {
        printf(D_MQTT_NOT_SUBSCRIBED, topic);
        mqttFailedCount++;
    } else {
        printf(D_BULLET D_MQTT_SUBSCRIBED, topic);
    }
}

void onMqttConnect(esp_mqtt_client_handle_t client)
{
    printf(D_MQTT_CONNECTED, mqttServer, mqttClientId);

    // Subscribe to our incoming topics
    char topic[64];
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_COMMAND "/#"), mqttGroupTopic);
    mqttSubscribeTo(topic);
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_COMMAND "/#"), mqttNodeTopic);
    mqttSubscribeTo(topic);
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_CONFIG "/#"), mqttGroupTopic);
    mqttSubscribeTo(topic);
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_CONFIG "/#"), mqttNodeTopic);
    mqttSubscribeTo(topic);

#if defined(HASP_USE_CUSTOM)
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_CUSTOM "/#"), mqttGroupTopic);
    mqttSubscribeTo(topic);
    snprintf_P(topic, sizeof(topic), PSTR("%s" MQTT_TOPIC_CUSTOM "/#"), mqttNodeTopic);
    mqttSubscribeTo(topic);
#endif

#ifdef HASP_USE_BROADCAST
    snprintf_P(topic, sizeof(topic), PSTR(MQTT_PREFIX "/" MQTT_TOPIC_BROADCAST "/" MQTT_TOPIC_COMMAND "/#"));
    mqttSubscribeTo(topic);
#endif

    /* Home Assistant auto-configuration */
#ifdef HASP_USE_HA
    if(mqttHAautodiscover) {
        char topic[64];
        snprintf_P(topic, sizeof(topic), PSTR("hass/status"));
        mqttSubscribeTo(topic);
        snprintf_P(topic, sizeof(topic), PSTR("homeassistant/status"));
        mqttSubscribeTo(topic);
    }
#endif

    // Force any subscribed clients to toggle offline/online when we first connect to
    // make sure we get a full panel refresh at power on.  Sending offline,
    // "online" will be sent by the mqttStatusTopic subscription action.
    mqtt_send_lwt(true);

    // mqttFirstConnect   = false;
    // mqttReconnectCount = 0;

    // haspReconnect();
    // haspProgressVal(255);

    dispatch_current_state(TAG_MQTT);
}

static void onMqttData(esp_mqtt_event_handle_t event)
{
    String topic = String(event->topic).substring(0, event->topic_len);
    String msg   = String(event->data).substring(0, event->data_len);
    // Serial.printf("onMqttData topic '%s' msg '%s'\n", topic.c_str(), msg.c_str());
    mqtt_message_cb(topic.c_str(), (byte*)msg.c_str(), msg.length());
}

static void onMqttSubscribed(esp_mqtt_event_handle_t event)
{
    String topic = String(event->topic).substring(0, event->topic_len);
    String msg   = String(event->data).substring(0, event->data_len);
    printf(D_BULLET D_MQTT_SUBSCRIBED " %d %s %d", topic.c_str(), event->topic_len, msg.c_str(),
                event->data_len);
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    // LOG_WARNING(TAG_MQTT, "mqtt_event_handler %d", event->event_id);

    int msg_id;
    switch(event->event_id) {
        case MQTT_EVENT_DISCONNECTED:
            printf(D_MQTT_DISCONNECTED);
            mqtt_disconnected();
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            // LOG_INFO(TAG_MQTT, F(D_MQTT_CONNECTING));
            break;
        case MQTT_EVENT_CONNECTED:
            printf(D_SERVICE_STARTED);
            mqtt_connected();
            onMqttConnect(event->client);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            onMqttSubscribed(event);
            break;
        case MQTT_EVENT_DATA:
            onMqttData(event);
            break;
        case MQTT_EVENT_ERROR: {
            mqtt_disconnected();
            esp_mqtt_error_codes_t* error_handle = event->error_handle;
            switch(error_handle->error_type) {
                case MQTT_ERROR_TYPE_TCP_TRANSPORT:
                    printf("Transport error");
                    break;
                case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
                    switch(error_handle->connect_return_code) {
                        case MQTT_CONNECTION_REFUSE_PROTOCOL: /*!< MQTT connection refused reason: Wrong protocol */
                            printf("Connection refused: Wrong protocol");
                            break;
                        case MQTT_CONNECTION_REFUSE_ID_REJECTED: /*!< MQTT connection refused reason: ID rejected */
                            printf("Connection refused: ID rejected");
                            break;
                        case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE: /*!< MQTT connection refused reason: Server
                                                                           unavailable */
                            printf("Connection refused: Server unavailable");
                            break;
                        case MQTT_CONNECTION_REFUSE_BAD_USERNAME: /*!< MQTT connection refused reason: Wrong user */
                            printf("Connection refused: Wrong user");
                            break;
                        case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED: /*!< MQTT connection refused reason: Wrong username
                                                                       or password */
                            printf("Connection refused: Authentication error");
                            break;
                        default:;
                    }
                    break;
                case MQTT_ERROR_TYPE_NONE:
                default:;
            }
            break;
        }
        default:
            printf("mqtt_event_handler %d", event->event_id);
    }
    return ESP_OK;
}

void mqttSetup()
{
    Preferences preferences;
    preferences.begin("mqtt", true);
    String password = preferences.getString(FP_CONFIG_PASS, MQTT_PASSWORD);
    strncpy(mqttPassword, password.c_str(), sizeof(mqttPassword));
    printf(D_BULLET "Read %s => %s (%d bytes)", FP_CONFIG_PASS, password.c_str(), password.length());

    queue = xQueueCreate(64, sizeof(mqtt_message_t));
    esp_crt_bundle_set(rootca_crt_bundle_start);
    mqttStart();
}

void mqttLoop(void)
{
    // mqttClient.loop();

    if(!uxQueueMessagesWaiting(queue)) return;

    mqtt_message_t data;
    while(xQueueReceive(queue, &data, (TickType_t)0)) {
        printf("[%d] QUE %s => %s", uxQueueMessagesWaiting(queue), data.topic, data.payload);
        size_t length = strlen(data.payload);
        dispatch_topic_payload(data.topic, data.payload, length > 0, TAG_MQTT);
        hasp_free(data.topic);
        hasp_free(data.payload);
        // delay(1);
    }
}

void mqttEvery5Seconds(bool networkIsConnected)
{
    // if(mqttEnabled && networkIsConnected && !mqttClientConnected) {
    //     LOG_TRACE(TAG_MQTT, F(D_MQTT_RECONNECTING));
    //     mqttStart();
    // }
}

void mqttStart()
{
    char buffer[64];
    char lastWillPayload[8];

    mqttEnabled = strlen(mqttServer) > 0 && mqttPort > 0;
    if(!mqttEnabled) {
        printf(D_MQTT_NOT_CONFIGURED);
        return;
    }

    /* Construct unique Client ID*/
    {
        String mac = halGetMacAddress(3, "");
        mac.toLowerCase();
        memset(mqttClientId, 0, sizeof(mqttClientId));
        snprintf_P(mqttClientId, sizeof(mqttClientId), haspDevice.get_hostname());
        size_t len = strlen(mqttClientId);
        snprintf_P(mqttClientId + len, sizeof(mqttClientId) - len, PSTR("_%s"), mac.c_str());
        printf(mqttClientId);
    }

    strncpy(mqttLwtTopic, mqttNodeTopic, sizeof(mqttNodeTopic));
    strncat_P(mqttLwtTopic, PSTR(MQTT_TOPIC_LWT), sizeof(mqttLwtTopic)-sizeof(mqttNodeTopic));
    printf(mqttLwtTopic);

    mqtt_cfg.event_handle           = mqtt_event_handler;
    mqtt_cfg.buffer_size            = MQTT_MAX_PACKET_SIZE;
    mqtt_cfg.out_buffer_size        = 512;
    mqtt_cfg.reconnect_timeout_ms   = 5000;
    mqtt_cfg.disable_auto_reconnect = false;
    mqtt_cfg.keepalive              = 15; /* seconds */
    mqtt_cfg.disable_clean_session  = true;

    mqtt_cfg.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
    mqtt_cfg.transport    = MQTT_TRANSPORT_OVER_TCP;
    mqtt_cfg.host         = mqttServer;
    mqtt_cfg.port         = mqttPort;
    mqtt_cfg.username     = mqttUsername;
    mqtt_cfg.password     = mqttPassword;
    mqtt_cfg.client_id    = mqttClientId;

    mqtt_cfg.lwt_msg    = "offline";
    mqtt_cfg.lwt_retain = true;
    mqtt_cfg.lwt_topic  = mqttLwtTopic;
    mqtt_cfg.lwt_qos    = 1;

    mqtt_cfg.task_prio = 1;

    // mqtt_cfg.crt_bundle_attach = esp_crt_bundle_attach;

    // test Mosquitto doesn't need a user/pwd
    //    // mqtt_cfg.username=(const char *)mqtt_user;
    //    // mqtt_cfg.password=(const char *)mqtt_pwd;

    if(mqttClient) {
        esp_mqtt_set_config(mqttClient, &mqtt_cfg);
    } else {
        mqttClient = esp_mqtt_client_init(&mqtt_cfg);
        if(esp_mqtt_client_start(mqttClient) != ESP_OK) {
            printf(D_SERVICE_START_FAILED);
            return;
        }
    }

    printf(D_SERVICE_STARTING);
}

void mqttStop()
{
    if(!mqttEnabled) {
        printf(D_SERVICE_DISABLED);
        return;
    }

    if(mqttClient != NULL) {
        if(current_mqtt_state) {
            printf(D_MQTT_DISCONNECTING);
        }
        mqtt_send_lwt(false);
        // esp_err_t err = esp_mqtt_client_stop(mqttClient); // Cannot be called from the *MQTT* event handler
        mqtt_cfg.disable_auto_reconnect = true;
        esp_mqtt_set_config(mqttClient, &mqtt_cfg);
        esp_err_t err = esp_mqtt_client_disconnect(mqttClient);
        if(err == ESP_OK) {
            mqtt_disconnected();
            printf(D_MQTT_DISCONNECTED);
        } else {
            printf(D_MQTT_FAILED " %d", err);
        }
    } else {
        printf(D_SERVICE_STOPPED);
    }
}

void mqtt_get_info(JsonDocument& doc)
{
    char buffer[64];
    String mac((char*)0);
    mac.reserve(64);

    JsonObject info          = doc.createNestedObject(F("MQTT"));
    info[F(D_INFO_SERVER)]   = mqttServer;
    info[F(D_INFO_USERNAME)] = mqttUsername;
    info[F(D_INFO_CLIENTID)] = mqttClientId;

    // switch(mqttClient.state()) {
    //     case MQTT_CONNECT_UNAUTHORIZED:
    //         snprintf_P(buffer, sizeof(buffer), PSTR(D_NETWORK_CONNECTION_UNAUTHORIZED));
    //         break;
    //     case MQTT_CONNECT_FAILED:
    //         snprintf_P(buffer, sizeof(buffer), PSTR(D_NETWORK_CONNECTION_FAILED));
    //         break;
    //     case MQTT_DISCONNECTED:
    //         snprintf_P(buffer, sizeof(buffer), PSTR(D_SERVICE_DISCONNECTED));
    //         break;
    //     case MQTT_CONNECTED:
    //         snprintf_P(buffer, sizeof(buffer), PSTR(D_SERVICE_CONNECTED));
    //         break;
    //     case MQTT_CONNECTION_TIMEOUT:
    //     case MQTT_CONNECTION_LOST:
    //     case MQTT_CONNECT_BAD_PROTOCOL:
    //     case MQTT_CONNECT_BAD_CLIENT_ID:
    //     case MQTT_CONNECT_UNAVAILABLE:
    //     case MQTT_CONNECT_BAD_CREDENTIALS:
    //     default:
    //         snprintf_P(buffer, sizeof(buffer), PSTR(D_SERVICE_DISCONNECTED " (%d)"), mqttClient.state());
    //         break;
    // }
    // info[F(D_INFO_STATUS)] = buffer;
    info[F(D_INFO_STATUS)] = !mqttEnabled         ? D_SERVICE_DISABLED
                             : !mqttClient        ? D_SERVICE_STOPPED
                             : current_mqtt_state ? D_SERVICE_CONNECTED
                                                  : D_SERVICE_DISCONNECTED;

    info[F(D_INFO_RECEIVED)]  = mqttReceiveCount;
    info[F(D_INFO_PUBLISHED)] = mqttPublishCount;
    info[F(D_INFO_FAILED)]    = mqttFailedCount;
}

#if HASP_USE_CONFIG > 0
bool mqttGetConfig(const JsonObject& settings)
{
    bool changed = false;

    if(strcmp(haspDevice.get_hostname(), settings[FPSTR(FP_CONFIG_NAME)].as<String>().c_str()) != 0) changed = true;
    settings[FPSTR(FP_CONFIG_NAME)] = haspDevice.get_hostname();

    if(strcmp(mqttGroupName, settings[FPSTR(FP_CONFIG_GROUP)].as<String>().c_str()) != 0) changed = true;
    settings[FPSTR(FP_CONFIG_GROUP)] = mqttGroupName;

    if(strcmp(mqttServer, settings[FPSTR(FP_CONFIG_HOST)].as<String>().c_str()) != 0) changed = true;
    settings[FPSTR(FP_CONFIG_HOST)] = mqttServer;

    if(mqttPort != settings[FPSTR(FP_CONFIG_PORT)].as<uint16_t>()) changed = true;
    settings[FPSTR(FP_CONFIG_PORT)] = mqttPort;

    if(strcmp(mqttUsername, settings[FPSTR(FP_CONFIG_USER)].as<String>().c_str()) != 0) changed = true;
    settings[FPSTR(FP_CONFIG_USER)] = mqttUsername;

    // if(strcmp(mqttPassword, settings[FPSTR(FP_CONFIG_PASS)].as<String>().c_str()) != 0) changed = true;
    // settings[FPSTR(FP_CONFIG_PASS)] = mqttPassword;
    if(strcmp(D_PASSWORD_MASK, settings[FPSTR(FP_CONFIG_PASS)].as<String>().c_str()) != 0) changed = true;
    settings[FPSTR(FP_CONFIG_PASS)] = D_PASSWORD_MASK;

    if(changed) configOutput(settings, TAG_MQTT);
    return changed;
}

/** Set MQTT Configuration.
 *
 * Read the settings from json and sets the application variables.
 *
 * @note: data pixel should be formated to uint32_t RGBA. Imagemagick requirements.
 *
 * @param[in] settings    JsonObject with the config settings.
 **/
bool mqttSetConfig(const JsonObject& settings)
{
    Preferences preferences;
    preferences.begin("mqtt", false);

    configOutput(settings, TAG_MQTT);
    bool changed = false;

    changed |= configSet(mqttPort, settings[FPSTR(FP_CONFIG_PORT)], F("mqttPort"));

    if(!settings[FPSTR(FP_CONFIG_NAME)].isNull()) {
        changed |= strcmp(haspDevice.get_hostname(), settings[FPSTR(FP_CONFIG_NAME)]) != 0;
        // strncpy(mqttNodeName, settings[FPSTR(FP_CONFIG_NAME)], sizeof(mqttNodeName));
        haspDevice.set_hostname(settings[FPSTR(FP_CONFIG_NAME)].as<const char*>());
    }
    // Prefill node name
    if(strlen(haspDevice.get_hostname()) == 0) {
        char mqttNodeName[64];
        String mac = halGetMacAddress(3, "");
        mac.toLowerCase();
        snprintf_P(mqttNodeName, sizeof(mqttNodeName), PSTR(D_MQTT_DEFAULT_NAME), mac.c_str());
        haspDevice.set_hostname(mqttNodeName);
        changed = true;
    }

    if(!settings[FPSTR(FP_CONFIG_GROUP)].isNull()) {
        changed |= strcmp(mqttGroupName, settings[FPSTR(FP_CONFIG_GROUP)]) != 0;
        strncpy(mqttGroupName, settings[FPSTR(FP_CONFIG_GROUP)], sizeof(mqttGroupName));
    }

    if(strlen(mqttGroupName) == 0) {
        strcpy_P(mqttGroupName, PSTR("plates"));
        changed = true;
    }

    if(!settings[FPSTR(FP_CONFIG_HOST)].isNull()) {
        changed |= strcmp(mqttServer, settings[FPSTR(FP_CONFIG_HOST)]) != 0;
        strncpy(mqttServer, settings[FPSTR(FP_CONFIG_HOST)], sizeof(mqttServer));
    }

    if(!settings[FPSTR(FP_CONFIG_USER)].isNull()) {
        changed |= strcmp(mqttUsername, settings[FPSTR(FP_CONFIG_USER)]) != 0;
        strncpy(mqttUsername, settings[FPSTR(FP_CONFIG_USER)], sizeof(mqttUsername));
    }

    if(!settings[FPSTR(FP_CONFIG_PASS)].isNull() &&
       settings[FPSTR(FP_CONFIG_PASS)].as<String>() != String(FPSTR(D_PASSWORD_MASK))) {
        changed |= strcmp(mqttPassword, settings[FPSTR(FP_CONFIG_PASS)]) != 0;
        strncpy(mqttPassword, settings[FPSTR(FP_CONFIG_PASS)], sizeof(mqttPassword));
        nvsUpdateString(preferences, FP_CONFIG_PASS, settings[FPSTR(FP_CONFIG_PASS)]);
    }

    snprintf_P(mqttNodeTopic, sizeof(mqttNodeTopic), PSTR(MQTT_PREFIX "/%s/"), haspDevice.get_hostname());
    snprintf_P(mqttGroupTopic, sizeof(mqttGroupTopic), PSTR(MQTT_PREFIX "/%s/"), mqttGroupName);

    return changed;
}
#endif // HASP_USE_CONFIG

#endif // PUBSUBCLIENT

#endif // HASP_USE_MQTT
