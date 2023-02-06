/* MIT License - Copyright (c) 2019-2023 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_CONSOLE_H
#define HASP_CONSOLE_H

#if HASP_USE_CONSOLE > 0

#include "hasplib.h"

/* ===== Default Event Processors ===== */
void consoleSetup();
void consoleLoop(void);
void consoleEvery5Seconds(void);
void consoleEverySecond(void);
void consoleStart(void);
void consoleStop(void);

/* ===== Special Event Processors ===== */
void console_update_prompt();

/* ===== Getter and Setter Functions ===== */

/* ===== Read/Write Configuration ===== */
#if HASP_USE_CONFIG > 0
bool consoleSetConfig(const JsonObject& settings);
bool consoleGetConfig(const JsonObject& settings);
#endif

#define CONSOLE_UNAUTHENTICATED 0
#define CONSOLE_USERNAME_OK 10
#define CONSOLE_USERNAME_NOK 99
#define CONSOLE_AUTHENTICATED 255

#endif
#endif
