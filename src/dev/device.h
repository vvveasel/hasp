/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_DEVICE_H
#define HASP_DEVICE_H

#ifdef ARDUINO
#include "Arduino.h"
#endif

#if defined(WINDOWS) || defined(POSIX)
#include <cstdint>
#endif
#if defined(POSIX)
#include <stddef.h>
#endif
#ifdef WINDOWS
#include "Windows.h"
#endif

#include "ArduinoJson.h"

#define STR_LEN_HOSTNAME 64
#define Q(x) #x
#define QUOTE(x) Q(x)

namespace dev {

class BaseDevice {
  public:
    bool has_battery           = false;
    bool has_backligth_control = true;

    virtual void reboot()
    {}
    const char* get_hostname()
    {
        return _hostname; //.c_str();
    }
    void set_hostname(const char* hostname)
    {
        strncpy(_hostname, hostname, STR_LEN_HOSTNAME);
    }
    const char* get_core_version()
    {
        return "";
    }
    virtual const char* get_chip_model()
    {
        return "";
    }
    const char* get_model()
    {
#ifdef HASP_MODEL
        return QUOTE(HASP_MODEL);
#else
        return PIOENV;
#endif
    }
    const char* get_version()
    {
        return (QUOTE(HASP_VER_MAJ) "." QUOTE(HASP_VER_MIN) "." QUOTE(HASP_VER_REV));
    }
    virtual const char* get_hardware_id()
    {
        return "";
    }
    virtual void init()
    {}
    virtual void show_info()
    {}
    virtual void post_setup()
    {}
    virtual void loop()
    {}
    virtual void loop_5s()
    {}
    virtual void set_backlight_pin(uint8_t pin)
    {}
    virtual void set_backlight_invert(bool override)
    {}
    virtual void set_backlight_level(uint8_t level)
    {}
    virtual uint8_t get_backlight_level()
    {
        return -1;
    }
    virtual void set_backlight_power(bool power)
    {}
    virtual bool get_backlight_invert()
    {
        return false;
    }
    virtual bool get_backlight_power()
    {
        return true;
    }
    virtual size_t get_free_max_block()
    {
        return 0;
    }
    virtual size_t get_free_heap()
    {
        return 0;
    }
    virtual uint8_t get_heap_fragmentation()
    {
        return 0;
    }
    virtual uint16_t get_cpu_frequency()
    {
        return 0;
    }
    virtual void get_info(JsonDocument& doc)
    {}
    virtual void get_sensors(JsonDocument& doc)
    {}
    virtual long get_uptime()
    {
        return 0;
    }
    virtual bool is_system_pin(uint8_t pin)
    {
        return false;
    }
    virtual std::string gpio_name(uint8_t pin)
    {
        char buffer[8];
        snprintf(buffer, sizeof(buffer), "%d", pin);
        return buffer;
    }

  private:
    // std::string _hostname;
    char _hostname[STR_LEN_HOSTNAME];
};

} // namespace dev

// #warning Building for ESP32 Devices
#include "pico/pico.h"

#endif
