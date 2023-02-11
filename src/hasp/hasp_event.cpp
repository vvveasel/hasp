/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

/* ********************************************************************************************
 *
 *  HASP Event Handlers
 *     - Value Senders       : Convert values and events into topic/payload before forwarding
 *     - Event Handlers      : Callbacks for object event processing
 *
 * To avoid race conditions:
 *  - Objects need to send consistent events encapsulated between `up`and `down` events
 *  - Where appropriate include the current value of the object
 *
 *  TODO:
 *  - Swiping an open dropdown list activates a gesture (lvgl bug)
 *  - Rolling a roller object sends the events out-of-order (down > up > changed)
 *  - Long pressing a colorpicker (to change the mode) does not send an up event:
 * static void indev_proc_release(lv_indev_proc_t* proc)
 * {
 *    if(proc->wait_until_release != 0) {
 *        lv_event_send(proc->types.pointer.act_obj, LV_EVENT_RELEASED, NULL);  // Add this line for HASP
 *
 ******************************************************************************************** */

#include <time.h>
#include <sys/time.h>

#include "hasplib.h"

#include "lv_core/lv_obj.h" // for tabview ext

static lv_style_int_t last_value_sent;
static lv_obj_t* last_obj_sent = NULL;
static lv_color_t last_color_sent;

void swipe_event_handler(lv_obj_t* obj, lv_event_t event);

// resets the last_value_sent
void event_reset_last_value_sent()
{
    last_obj_sent   = NULL;
    last_value_sent = INT16_MIN;
}

/**
 * Clean-up allocated memory before an object is deleted
 * @param obj pointer to an object to clean-up
 */
void delete_event_handler(lv_obj_t* obj, lv_event_t event)
{
    if(event != LV_EVENT_DELETE) return;

    uint8_t part_cnt = LV_OBJ_PART_MAIN;

    switch(obj_get_type(obj)) {
        case LV_HASP_LINE:
            my_line_clear_points(obj);
            break;

        case LV_HASP_BTNMATRIX:
            my_btnmatrix_map_clear(obj);
            break;

        case LV_HASP_MSGBOX:
            my_msgbox_map_clear(obj);
            break;

        case LV_HASP_IMAGE:
            my_image_release_resources(obj);
            break;

        case LV_HASP_GAUGE:
            break;

        default:
            break;
    }

    // TODO: delete value_str data for ALL parts
    for(uint8_t part = 0; part <= part_cnt; part++) {
        my_obj_set_value_str_text(obj, part, LV_STATE_DEFAULT, NULL);
        my_obj_set_value_str_text(obj, part, LV_STATE_CHECKED, NULL);
        my_obj_set_value_str_text(obj, part, LV_STATE_PRESSED + LV_STATE_DEFAULT, NULL);
        my_obj_set_value_str_text(obj, part, LV_STATE_PRESSED + LV_STATE_CHECKED, NULL);
        my_obj_set_value_str_text(obj, part, LV_STATE_DISABLED + LV_STATE_DEFAULT, NULL);
        my_obj_set_value_str_text(obj, part, LV_STATE_DISABLED + LV_STATE_CHECKED, NULL);
    }
    my_obj_set_tag(obj, (char*)NULL);
    my_obj_set_action(obj, (char*)NULL);
}

/* ============================== Timer Event  ============================ */
#if LV_USE_CALENDAR > 0
void event_timer_calendar(lv_task_t* task)
{
    hasp_task_user_data_t* data = (hasp_task_user_data_t*)task->user_data;
    lv_obj_t* obj               = NULL;

    if(data) obj = hasp_find_obj_from_page_id(data->pageid, data->objid);
    if(!obj || !data || !obj_check_type(obj, LV_HASP_CALENDER)) {
        if(data) lv_mem_free(data); // the object that the user_data points to is gone
        lv_task_del(task);          // the calendar object for this task was deleted
        LOG_WARNING(TAG_EVENT, "event_timer_calendar could not find the linked object");
        return;
    }

    lv_calendar_date_t date;

    timeval curTime;
    int rslt     = gettimeofday(&curTime, NULL);
    time_t t     = curTime.tv_sec;
    tm* timeinfo = localtime(&t);
    (void)rslt; // unused

    if(timeinfo->tm_year < 120) {
        lv_task_set_period(task, 60000); // try again in a minute
        LOG_WARNING(TAG_EVENT, "event_timer_calendar could not sync the clock");
        return;
    } else {
        uint32_t next_hour = (3600 - (t % 3600)) * 1000; // ms to next top of hour
        // lv_task_set_period(task, next_hour + 128);       // small offset so all tasks don't run at once
        lv_task_set_period(task, data->interval);
    }

    date.day   = timeinfo->tm_mday;
    date.month = timeinfo->tm_mon + 1;     // months since January 0-11
    date.year  = timeinfo->tm_year + 1900; // years since 1900

    LOG_VERBOSE(TAG_EVENT, "event_timer_calendar called with user %d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min,
                timeinfo->tm_sec);

    lv_calendar_set_today_date(obj, &date);
}
#endif

void event_timer_clock(lv_task_t* task)
{
    hasp_task_user_data_t* data = (hasp_task_user_data_t*)task->user_data;

    if(!data || !data->obj || !lv_debug_check_obj_valid(data->obj)) {
        if(data) {
            if(data->templ != D_TIMESTAMP) hasp_free(data->templ);
            lv_mem_free(data); // the object that the user_data points to is gone}
        }
        lv_task_del(task); // the calendar object for this task was deleted
        printf("event_timer_clock could not find the linked object");
        return;

    } else if(!data->templ) {
        return; // nothing to do
    }

    timeval curTime;
    int rslt = gettimeofday(&curTime, NULL);
    (void)rslt; // unused
    time_t seconds     = curTime.tv_sec;
    useconds_t tv_msec = curTime.tv_usec / 1000;
    tm* timeinfo       = localtime(&seconds);
    lv_task_set_period(task, data->interval - tv_msec);

    char buffer[128] = {0};
    /* if(timeinfo->tm_year < 120) {
         snprintf_P(buffer, sizeof(buffer), PSTR("%ld"), seconds);
     } else */
    strftime(buffer, sizeof(buffer), data->templ, timeinfo);

    // LOG_VERBOSE(TAG_EVENT, "event_timer_clock called with user %d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min,
    //             timeinfo->tm_sec);

    if(!strcmp(buffer, lv_label_get_text(data->obj))) return; // No change
    lv_label_set_text(data->obj, buffer);
}

/* ============================== Timer Event  ============================ */
void event_timer_refresh(lv_task_t* task)
{
    lv_obj_t* obj = (lv_obj_t*)task->user_data;
    printf("event_timer_refresh called with user data\n");
    if(!obj) return;

    lv_obj_invalidate(obj);
}

/* ============================== Event Senders ============================ */

/* Takes and lv_obj and finds the pageid and objid
   Then sends the data out on the state/pxby topic
*/

/**
 * Get the hasp eventid for LV_EVENT_PRESSED, LV_EVENT_VALUE_CHANGED, LV_EVENT_LONG_PRESSED_REPEAT and
 * LV_EVENT_RELEASED Also updates the sleep state and handles LV_EVENT_DELETE events
 * @param obj pointer to a color picker
 * @param event type of event that occured
 * @param eventid returns the hasp eventid
 */
static bool translate_event(lv_obj_t* obj, lv_event_t event, uint8_t& eventid)
{
    switch(event) {
        case LV_EVENT_GESTURE:
            swipe_event_handler(obj, event);
            break;

        case LV_EVENT_DELETE:
            printf(D_OBJECT_DELETED);
            delete_event_handler(obj, event);
            break;

        case LV_EVENT_PRESSED:
            hasp_update_sleep_state(); // wakeup on press down?
            eventid = HASP_EVENT_DOWN;
            return true;

        case LV_EVENT_LONG_PRESSED_REPEAT:
        case LV_EVENT_LONG_PRESSED:
            eventid = HASP_EVENT_CHANGED;
            return true;

        case LV_EVENT_RELEASED:
            eventid = HASP_EVENT_UP;
            return true;

        case LV_EVENT_VALUE_CHANGED:
            eventid = HASP_EVENT_CHANGED;
            return true;
    }

    return false; // event not translated
}

// ##################### Value Senders ########################################################

static void event_send_object_data(lv_obj_t* obj, const char* data)
{
    uint8_t pageid;
    uint8_t objid;

    if(hasp_find_id_from_obj(obj, &pageid, &objid)) {
        if(!data) return;
        object_dispatch_state(pageid, objid, data);
    } else {
        printf(D_OBJECT_UNKNOWN);
    }
}

// Send out events with a val attribute
static void event_object_val_event(lv_obj_t* obj, uint8_t eventid, int16_t val)
{
    char data[512];
    {
        char eventname[8];
        Parser::get_event_name(eventid, eventname, sizeof(eventname));
        if(const char* tag = my_obj_get_tag(obj))
            snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"val\":%d,\"tag\":%s}"), eventname, val, tag);
        else
            snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"val\":%d}"), eventname, val);
    }
    event_send_object_data(obj, data);
}

// Send out events with a val and text attribute
static void event_object_selection_changed(lv_obj_t* obj, uint8_t eventid, int16_t val, const char* text)
{
    char data[512];
    {
        char eventname[8];
        Parser::get_event_name(eventid, eventname, sizeof(eventname));

        if(const char* tag = my_obj_get_tag(obj))
            snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"val\":%d,\"text\":\"%s\",\"tag\":%s}"), eventname,
                       val, text, tag);
        else
            snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"val\":%d,\"text\":\"%s\"}"), eventname, val, text);
    }
    event_send_object_data(obj, data);
}

// ##################### Event Handlers ########################################################

static inline void event_update_group(uint8_t group, lv_obj_t* obj, bool power, int32_t val, int32_t min, int32_t max)
{
    hasp_update_value_t value = {.obj = obj, .group = group, .min = min, .max = max, .val = val, .power = power};
    dispatch_normalized_group_values(value);
}

static void log_event(const char* name, lv_event_t event)
{
    return;

    switch(event) {
        case LV_EVENT_PRESSED:
            printf("%s Changed", name);
            break;

        case LV_EVENT_PRESS_LOST:
            printf("%s Press lost", name);
            break;

        case LV_EVENT_SHORT_CLICKED:
            printf("%s Short clicked", name);
            break;

        case LV_EVENT_CLICKED:
            printf("%s Clicked", name);
            break;

        case LV_EVENT_LONG_PRESSED:
            printf("%S Long press", name);
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            printf("%s Long press repeat", name);
            break;

        case LV_EVENT_RELEASED:
            printf("%s Released", name);
            break;

        case LV_EVENT_VALUE_CHANGED:
            printf("%s Changed", name);
            break;

        case LV_EVENT_GESTURE:
            printf("%s Gesture", name);
            break;

        case LV_EVENT_FOCUSED:
            printf("%s Focussed", name);
            break;

        case LV_EVENT_DEFOCUSED:
            printf("%s Defocussed", name);
            break;

        case LV_EVENT_PRESSING:
            break;

        default:
            printf("%s Other %d", name, event);
    }
}

/**
 * Called when a press on the system layer is detected
 * @param obj pointer to a button matrix
 * @param event type of event that occured
 */
void first_touch_event_handler(lv_obj_t* obj, lv_event_t event)
{
    //  log_event("wakeup", event);
    if(obj != lv_disp_get_layer_sys(NULL)) return;

    if(event == LV_EVENT_RELEASED) {
        bool changed = hasp_stop_antiburn(); // Disable antiburn task

        if(!haspDevice.get_backlight_power()) {
            dispatch_backlight(NULL, "on", TAG_EVENT); // backlight on and also disable wakeup touch
            hasp_set_wakeup_touch(false);              // only disable wakeup touch
        } else {
            hasp_set_wakeup_touch(false); // only disable wakeup touch
        }

        hasp_update_sleep_state();                                // wakeup, send Idle off
        if(changed) dispatch_state_antiburn(hasp_get_antiburn()); // publish the new state

    } else if(event == LV_EVENT_PRESSED) {
        haspDevice.set_backlight_power(true);
    }
}

void swipe_event_handler(lv_obj_t* obj, lv_event_t event)
{
    if(!obj || obj->user_data.swipeid == 0) return;

    if(event == LV_EVENT_GESTURE) {
        lv_gesture_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
            case LV_GESTURE_DIR_LEFT:
                haspPages.next(LV_SCR_LOAD_ANIM_NONE, 500, 0);
                break;
            case LV_GESTURE_DIR_RIGHT:
                haspPages.prev(LV_SCR_LOAD_ANIM_NONE, 500, 0);
                break;
            case LV_GESTURE_DIR_BOTTOM:
                haspPages.back(LV_SCR_LOAD_ANIM_NONE, 500, 0);
                break;
            default:
                dispatch_current_page();
        }
    }
}

/**
 * Called when a textarea is clicked
 * @param obj pointer to a textarea object
 * @param event type of event that occured
 */
void textarea_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("textarea", event);

    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Changed to: %s", lv_textarea_get_text(obj));

        uint8_t hasp_event_id;
        if(!translate_event(obj, event, hasp_event_id)) return;

        char data[1024];
        {
            char eventname[8];
            Parser::get_event_name(hasp_event_id, eventname, sizeof(eventname));

            if(const char* tag = my_obj_get_tag(obj))
                snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"text\":\"%s\",\"tag\":%s}"), eventname,
                           lv_textarea_get_text(obj), tag);
            else
                snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"text\":\"%s\"}"), eventname,
                           lv_textarea_get_text(obj));
        }

        event_send_object_data(obj, data);
    } else if(event == LV_EVENT_FOCUSED) {
        lv_textarea_set_cursor_hidden(obj, false);
    } else if(event == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(obj, true);
    }
}

/**
 * Called when a button-style object is clicked
 * @param obj pointer to a button object
 * @param event type of event that occured
 */
void generic_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("generic", event);
    last_obj_sent = obj; // updated but not used in this function

    switch(event) {
        case LV_EVENT_GESTURE:
            swipe_event_handler(obj, event);
            return;

        case LV_EVENT_PRESSED:
            hasp_update_sleep_state(); // wakeup?
            last_value_sent = HASP_EVENT_DOWN;
            break;

        case LV_EVENT_CLICKED:
            // UP = the same object was release then was pressed and press was not lost!
            // eventid = HASP_EVENT_UP;
            if(last_value_sent != HASP_EVENT_LOST && last_value_sent != HASP_EVENT_UP)
                last_value_sent = HASP_EVENT_LONG;
            return;

        case LV_EVENT_SHORT_CLICKED:
            if(last_value_sent != HASP_EVENT_LOST) last_value_sent = HASP_EVENT_UP; // Avoid SHORT + UP double events
            break;

        case LV_EVENT_LONG_PRESSED:
            if(last_value_sent != HASP_EVENT_LOST) last_value_sent = HASP_EVENT_LONG;
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            if(last_value_sent != HASP_EVENT_LOST) last_value_sent = HASP_EVENT_HOLD;
            break; // we do care about hold

        case LV_EVENT_PRESS_LOST:
            last_value_sent = HASP_EVENT_LOST;
            break;

        case LV_EVENT_RELEASED:
            if(last_value_sent == HASP_EVENT_UP) return;
            last_value_sent = HASP_EVENT_RELEASE;
            break;

        case LV_EVENT_DELETE:
            printf(D_OBJECT_DELETED);
            delete_event_handler(obj, event); // free and destroy persistent memory allocated for certain objects
            return;

        case LV_EVENT_PRESSING:
        case LV_EVENT_FOCUSED:
        case LV_EVENT_DEFOCUSED:
            return; // Don't care about these

        case LV_EVENT_VALUE_CHANGED: // Should not occur in this event handler
        default:
            printf(D_OBJECT_EVENT_UNKNOWN, event);
            return;
    }

    if(last_value_sent == HASP_EVENT_LOST) return;

    if(obj->user_data.action) {
     //   if(last_value_sent == HASP_EVENT_UP || last_value_sent == HASP_EVENT_RELEASE) {
            // dispatch_text_line(obj->user_data.action, TAG_EVENT);

            StaticJsonDocument<256> doc;
            StaticJsonDocument<64> filter;
            char eventname[8];

            Parser::get_event_name(last_value_sent, eventname, sizeof(eventname));
            filter[eventname] = true;
            DeserializationError jsonError =
                deserializeJson(doc, (const char*)obj->user_data.action, DeserializationOption::Filter(filter));

            if(!jsonError) {
                JsonVariant json  = doc[eventname].as<JsonVariant>();
                uint8_t savedPage = haspPages.get();
                if(!dispatch_json_variant(json, savedPage, TAG_EVENT)) {
                    printf(D_DISPATCH_COMMAND_NOT_FOUND, eventname);
                    // dispatch_simple_text_command(payload, source);
                }
            }
      //  }

    } else if(obj->user_data.actionid) {
        /* If an actionid is attached, perform that action on UP event only */
        if(last_value_sent == HASP_EVENT_UP || last_value_sent == HASP_EVENT_RELEASE) {
            lv_scr_load_anim_t transitionid = (lv_scr_load_anim_t)obj->user_data.transitionid;
            switch(obj->user_data.actionid) {
                case HASP_NUM_PAGE_PREV:
                    haspPages.prev(transitionid, 500, 0);
                    break;
                case HASP_NUM_PAGE_BACK:
                    haspPages.back(transitionid, 500, 0);
                    break;
                case HASP_NUM_PAGE_NEXT:
                    haspPages.next(transitionid, 500, 0);
                    break;
                default:
                    haspPages.set(obj->user_data.actionid, transitionid, 500, 0);
            }
            dispatch_current_page();
        }

    } else {
        char data[512];
        {
            char eventname[8];
            Parser::get_event_name(last_value_sent, eventname, sizeof(eventname));

            if(const char* tag = my_obj_get_tag(obj))
                snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\",\"tag\":%s}"), eventname, tag);
            else
                snprintf_P(data, sizeof(data), PSTR("{\"event\":\"%s\"}"), eventname);
        }
        event_send_object_data(obj, data);
    }

    // Update group objects and gpios on release
    if(last_value_sent != LV_EVENT_LONG_PRESSED && last_value_sent != LV_EVENT_LONG_PRESSED_REPEAT) {
        bool state = Parser::get_event_state(last_value_sent);
        event_update_group(obj->user_data.groupid, obj, state, state, HASP_EVENT_OFF, HASP_EVENT_ON);
    }
}

/**
 * Called when a object state is toggled on/off
 * @param obj pointer to a switch object
 * @param event type of event that occured
 */
void toggle_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("toggle", event);
    last_obj_sent = obj; // updated but not used in this function

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id)) return;
    if(hasp_event_id != HASP_EVENT_DOWN && hasp_event_id != HASP_EVENT_UP) return; // only up or down wanted

    /* Get the new value */
    switch(obj->user_data.objid) {
        case LV_HASP_SWITCH:
            last_value_sent = lv_switch_get_state(obj);
            break;

            /* case LV_HASP_CHECKBOX:
                // This lvgl value is incorrect, it returns pressed instead of checked
                // last_value_sent = lv_checkbox_is_checked(obj);
                last_value_sent = lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED;
                break; */

        case LV_HASP_CHECKBOX:
        case LV_HASP_BUTTON: {
            last_value_sent = lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED;
            break;
        }

        default:
            return; // invalid object
    }

    event_object_val_event(obj, hasp_event_id, last_value_sent);

    // Update group objects and gpios on release
    if(obj->user_data.groupid && hasp_event_id == HASP_EVENT_UP) {
        event_update_group(obj->user_data.groupid, obj, last_value_sent, last_value_sent, HASP_EVENT_OFF,
                           HASP_EVENT_ON);
    }
}

/**
 * Called when a range value has changed
 * @param obj pointer to a dropdown list or roller
 * @param event type of event that occured
 */
void selector_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("selector", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id)) return; // Use LV_EVENT_VALUE_CHANGED

    /* Get the new value */
    char buffer[128];
#if LV_USE_TABLE > 0
    char property[36];
#endif
    uint16_t val = 0;
    uint16_t max = 0;

    /* Get the text, val and max properties */
    switch(obj->user_data.objid) {
        case LV_HASP_DROPDOWN:
            val = lv_dropdown_get_selected(obj);
            max = lv_dropdown_get_option_cnt(obj) - 1;
            lv_dropdown_get_selected_str(obj, buffer, sizeof(buffer));
            break;

        case LV_HASP_ROLLER:
            val = lv_roller_get_selected(obj);
            max = lv_roller_get_option_cnt(obj) - 1;
            lv_roller_get_selected_str(obj, buffer, sizeof(buffer));
            break;

        case LV_HASP_TABVIEW: {
            val = lv_tabview_get_tab_act(obj);
            max = lv_tabview_get_tab_count(obj) - 1;

            lv_tabview_ext_t* ext = (lv_tabview_ext_t*)lv_obj_get_ext_attr(obj);
            strcpy(buffer, ext->tab_name_ptr[val]);
            break;
        }

#if LV_USE_TABLE > 0
        case LV_HASP_TABLE: {
            uint16_t row;
            uint16_t col;
            if(lv_table_get_pressed_cell(obj, &row, &col) != LV_RES_OK) return; // outside any cell

            const char* txt = lv_table_get_cell_value(obj, row, col);
            strncpy(buffer, txt, sizeof(buffer));

            snprintf_P(property, sizeof(property), PSTR("row\":%d,\"col\":%d,\"text"), row, col);
            attr_out_str(obj, property, buffer);
            return; // done sending
        }
#endif

        default:
            return; // Invalid selector type
    }

    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    last_value_sent = val;
    last_obj_sent   = obj;
    event_object_selection_changed(obj, hasp_event_id, val, buffer);

    if(obj->user_data.groupid && max > 0) // max a cannot be 0, its the divider
        if(hasp_event_id == HASP_EVENT_UP || hasp_event_id == HASP_EVENT_CHANGED) {
            event_update_group(obj->user_data.groupid, obj, !!last_value_sent, last_value_sent, 0, max);
        }

    // set the property
    // snprintf_P(property, sizeof(property), PSTR("val\":%d,\"text"), val);
    // attr_out_str(obj, property, buffer);
}

/**
 * Called when a btnmatrix value has changed
 * @param obj pointer to a dropdown list or roller
 * @param event type of event that occured
 */
void alarm_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("alarm", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id)) return; // Use LV_EVENT_VALUE_CHANGED

    /* Get the new value */
    // char buffer[128] = "";
    uint16_t val = 0;

    val = lv_btnmatrix_get_active_btn(obj);
    if(val != LV_BTNMATRIX_BTN_NONE && hasp_event_id == HASP_EVENT_UP) {
        lv_obj_t* ta    = hasp_find_obj_from_parent_id(lv_obj_get_parent(obj), 5);
        const char* txt = lv_btnmatrix_get_btn_text(obj, val);
        if(!strcmp(txt, LV_SYMBOL_BACKSPACE))
            lv_textarea_del_char(ta);
        else if(!strcmp(txt, LV_SYMBOL_CLOSE))
            lv_textarea_set_text(ta, "");
        else if(strlen(txt) == 1)
            lv_textarea_add_text(ta, txt);
        else
            ;
        // strncpy(buffer, txt, sizeof(buffer));
    }

    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    last_value_sent = val;
    last_obj_sent   = obj;
    // event_object_selection_changed(obj, hasp_event_id, val, buffer);

    // if(max > 0) // max a cannot be 0, its the divider
    //     if(hasp_event_id == HASP_EVENT_UP || hasp_event_id == LV_EVENT_VALUE_CHANGED) {
    //         event_update_group(obj->user_data.groupid, obj, last_value_sent, 0, max);
    //     }
}

/**
 * Called when a btnmatrix value has changed
 * @param obj pointer to a dropdown list or roller
 * @param event type of event that occured
 */
void btnmatrix_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("btnmatrix", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id)) return; // Use LV_EVENT_VALUE_CHANGED

    /* Get the new value */
    char buffer[128] = "";
    uint16_t val     = 0;

    val = lv_btnmatrix_get_active_btn(obj);
    if(val != LV_BTNMATRIX_BTN_NONE) {
        const char* txt = lv_btnmatrix_get_btn_text(obj, val);
        strncpy(buffer, txt, sizeof(buffer));
    }

    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    last_value_sent = val;
    last_obj_sent   = obj;
    event_object_selection_changed(obj, hasp_event_id, val, buffer);

    // if(max > 0) // max a cannot be 0, its the divider
    //     if(hasp_event_id == HASP_EVENT_UP || hasp_event_id == LV_EVENT_VALUE_CHANGED) {
    //         event_update_group(obj->user_data.groupid, obj, last_value_sent, 0, max);
    //     }
}

/**
 * Called when a msgbox value has changed
 * @param obj pointer to a dropdown list or roller
 * @param event type of event that occured
 */
void msgbox_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("msgbox", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id)) return; // Use LV_EVENT_VALUE_CHANGED

    /* Get the new value */
    char buffer[128];
    uint16_t val = 0;

    val = lv_msgbox_get_active_btn(obj);
    if(val != LV_BTNMATRIX_BTN_NONE) {
        const char* txt = lv_msgbox_get_active_btn_text(obj);
        strncpy(buffer, txt, sizeof(buffer));
        if(hasp_event_id == HASP_EVENT_UP || hasp_event_id == HASP_EVENT_RELEASE) lv_msgbox_start_auto_close(obj, 0);
    } else {
        buffer[0] = 0; // empty string
    }

    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    last_value_sent = val;
    last_obj_sent   = obj;
    event_object_selection_changed(obj, hasp_event_id, val, buffer);
    // if(max > 0) event_update_group(obj->user_data.groupid, obj, val, 0, max);
}

/**
 * Called when a slider or adjustable arc is clicked
 * @param obj pointer to a slider
 * @param event type of event that occured
 */
void slider_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("slider", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id) || event == LV_EVENT_VALUE_CHANGED) return;

    /* Get the new value */
    int16_t val;
    int16_t min;
    int16_t max;

    if(obj->user_data.objid == LV_HASP_SLIDER) {
        val = lv_slider_get_value(obj);
        min = lv_slider_get_min_value(obj);
        max = lv_slider_get_max_value(obj);

    } else if(obj->user_data.objid == LV_HASP_ARC) {
        val = lv_arc_get_value(obj);
        min = lv_arc_get_min_value(obj);
        max = lv_arc_get_max_value(obj);

    } else {
        return; // not a slider
    }

    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    last_value_sent = val;
    last_obj_sent   = obj;
    event_object_val_event(obj, hasp_event_id, val);

    if(obj->user_data.groupid && (hasp_event_id == HASP_EVENT_CHANGED || hasp_event_id == HASP_EVENT_UP) && min != max)
        event_update_group(obj->user_data.groupid, obj, !!val, val, min, max);
}

/**
 * Called when a color picker is clicked
 * @param obj pointer to a color picker
 * @param event type of event that occured
 */
void cpicker_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("cpicker", event);

    uint8_t hasp_event_id;
    if(!translate_event(obj, event, hasp_event_id) || event == LV_EVENT_VALUE_CHANGED) return;

    /* Get the new value */
    lv_color_t color = lv_cpicker_get_color(obj);

    if(hasp_event_id == HASP_EVENT_CHANGED && last_color_sent.full == color.full) return; // same value as before

    char data[512];
    {
        char eventname[8];
        Parser::get_event_name(hasp_event_id, eventname, sizeof(eventname));

        lv_color32_t c32;
        c32.full        = lv_color_to32(color);
        last_color_sent = color;

        if(const char* tag = my_obj_get_tag(obj))
            snprintf_P(data, sizeof(data),
                       PSTR("{\"event\":\"%s\",\"color\":\"#%02x%02x%02x\",\"r\":%d,\"g\":%d,\"b\":%d,\"tag\":%s}"),
                       eventname, c32.ch.red, c32.ch.green, c32.ch.blue, c32.ch.red, c32.ch.green, c32.ch.blue, tag);
        else
            snprintf_P(data, sizeof(data),
                       PSTR("{\"event\":\"%s\",\"color\":\"#%02x%02x%02x\",\"r\":%d,\"g\":%d,\"b\":%d}"), eventname,
                       c32.ch.red, c32.ch.green, c32.ch.blue, c32.ch.red, c32.ch.green, c32.ch.blue);
    }
    event_send_object_data(obj, data);

    // event_update_group(obj->user_data.groupid, obj, val, min, max);
}

#if LV_USE_CALENDAR > 0
void calendar_event_handler(lv_obj_t* obj, lv_event_t event)
{
    log_event("calendar", event);

    uint8_t hasp_event_id;
    if(event != LV_EVENT_PRESSED && event != LV_EVENT_RELEASED && event != LV_EVENT_VALUE_CHANGED) return;
    if(!translate_event(obj, event, hasp_event_id)) return; // Use LV_EVENT_VALUE_CHANGED

    /* Get the new value */
    lv_calendar_date_t* date;
    if(hasp_event_id == HASP_EVENT_CHANGED)
        date = lv_calendar_get_pressed_date(obj); // pressed date
    else
        date = lv_calendar_get_showed_date(obj); // current month
    if(!date) return;

    lv_style_int_t val = date->day + date->month * 31;
    if(hasp_event_id == HASP_EVENT_CHANGED && last_value_sent == val && last_obj_sent == obj)
        return; // same object and value as before

    char data[512];
    {
        char eventname[8];
        Parser::get_event_name(hasp_event_id, eventname, sizeof(eventname));

        last_value_sent = val;
        last_obj_sent   = obj;

        if(const char* tag = my_obj_get_tag(obj))
            snprintf_P(data, sizeof(data),
                       PSTR("{\"event\":\"%s\",\"val\":\"%d\",\"text\":\"%04d-%02d-%02dT00:00:00Z\",\"tag\":%s}"),
                       eventname, date->day, date->year, date->month, date->day, tag);
        else
            snprintf_P(data, sizeof(data),
                       PSTR("{\"event\":\"%s\",\"val\":\"%d\",\"text\":\"%04d-%02d-%02dT00:00:00Z\"}"), eventname,
                       date->day, date->year, date->month, date->day);
    }
    event_send_object_data(obj, data);

    // event_update_group(obj->user_data.groupid, obj, val, min, max);
}
#endif