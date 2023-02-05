/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#if defined(ESP32)

#include "hasp_conf.h"
#include "../device.h"
#include "pico.h"
#include "hasp_debug.h"

//#include "../../drv/tft/tft_driver.h" // for haspTft

#define BACKLIGHT_CHANNEL 0

#if defined(CONFIG_IDF_TARGET_ESP32)
//uint8_t temprature_sens_read();
#endif

namespace dev {

static String picoResetReason(uint8_t cpuid)
{
    return "";
}

static void halGetResetInfo(String& resetReason)
{
    resetReason = String(picoResetReason(0));
    resetReason += F(" / ");
    resetReason += String(picoResetReason(1));
}

pico_Device::pico_Device()
{
    BaseDevice::set_hostname(MQTT_NODENAME);
    _backlight_invert = (TFT_BACKLIGHT_ON == LOW);
    _backlight_power  = 1;
    _backlight_level  = 255;
    _backlight_pin    = 255; // not TFT_BCKL because it is unkown at this stage

}

void pico_Device::reboot()
{}

void pico_Device::show_info()
{
    LOG_VERBOSE(TAG_DEV, F("Processor  : pico"));
    LOG_VERBOSE(TAG_DEV, F("CPU freq.  : %i MHz"), 125);
}

const char* pico_Device::get_core_version()
{
    return "";
}
const char* pico_Device::get_chip_model()
{
    return "";
}

const char* pico_Device::get_hardware_id()
{
    return "";
}

void pico_Device::set_backlight_pin(uint8_t pin)
{}

void pico_Device::set_backlight_invert(bool invert)
{}

bool pico_Device::get_backlight_invert()
{
    return _backlight_invert;
}

void pico_Device::set_backlight_level(uint8_t level)
{
    _backlight_level = level;
    update_backlight();
}

uint8_t pico_Device::get_backlight_level()
{
    return _backlight_level;
}

void pico_Device::set_backlight_power(bool power)
{
    _backlight_power = power;
    update_backlight();
}

bool pico_Device::get_backlight_power()
{
    return _backlight_power != 0;
}

void pico_Device::update_backlight()
{}

size_t pico_Device::get_free_max_block()
{
    return 8000;    //TODO: Implement
}

size_t pico_Device::get_free_heap()
{
    return 90000;   //TODO: Implement
}

uint8_t pico_Device::get_heap_fragmentation()
{
    uint32_t free = get_free_heap();
    if(free) {
        return (int8_t)(100.00f - (float)get_free_max_block() * 100.00f / (float)free);
    } else {
        return 100; // no free memory
    }
}

uint16_t pico_Device::get_cpu_frequency()
{
    return 125;     //TODO: Implement
}

bool pico_Device::is_system_pin(uint8_t pin)
{
    return false;   //TODO: Implement
}

void pico_Device::get_info(JsonDocument& doc)
{
    char size_buf[64];
    String buffer((char*)0);
    buffer.reserve(64);

    JsonObject info = doc.createNestedObject(F(D_INFO_MODULE));

    /* ESP Stats */
    buffer = String(get_cpu_frequency());
    buffer += F("MHz");
    info[F(D_INFO_MODEL)]     = get_chip_model(); // 10ms
    info[F(D_INFO_FREQUENCY)] = buffer;

    info[F(D_INFO_CORE_VERSION)] = get_core_version();
    halGetResetInfo(buffer);
    info[F(D_INFO_RESET_REASON)] = buffer;

    Parser::format_bytes(999, size_buf, sizeof(size_buf)); // TODO: Implement
    info[F(D_INFO_FLASH_SIZE)] = size_buf;

    if(_sketch_size == 0) _sketch_size = 999; // TODO: Implement
    Parser::format_bytes(_sketch_size, size_buf, sizeof(size_buf));
    info[F(D_INFO_SKETCH_USED)] = size_buf;

    Parser::format_bytes(999, size_buf, sizeof(size_buf));   // TODO: Implement
    info[F(D_INFO_SKETCH_FREE)] = size_buf;

    Parser::format_bytes(999, size_buf, sizeof(size_buf)); // TODO: Implement
    info[F(D_INFO_FS_SIZE)] = size_buf;

    Parser::format_bytes(999, size_buf, sizeof(size_buf));  // TODO: Implement
    info[F(D_INFO_FS_USED)] = size_buf;

    Parser::format_bytes(999, size_buf, sizeof(size_buf));   // TODO: Implement
    info[F(D_INFO_FS_FREE)] = size_buf;
}

void pico_Device::get_sensors(JsonDocument& doc)
{
#if defined(CONFIG_IDF_TARGET_ESP32)
    JsonObject sensor        = doc.createNestedObject(F("ESP32"));
   // uint32_t temp            = (temprature_sens_read() - 32) * 100 / 1.8;
    sensor[F("Temperature")] = serialized(String(1.0f * 25 / 100, 2));  // TODO: Implement
#endif
}

long pico_Device::get_uptime()
{
    return esp_timer_get_time() / 1000000U;
}

} // namespace dev

#if defined(LANBONL8)
// #warning Building for Lanbon L8
#include "dev/esp32/lanbonl8.h"
#elif defined(M5STACK)
                                                // #warning Building for M5Stack core2
#include "dev/esp32/m5stackcore2.h"
#else
dev::pico_Device haspDevice;
#endif

#endif // ESP32