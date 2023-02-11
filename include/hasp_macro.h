/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_MACRO_H
#define HASP_MACRO_H


#ifdef USE_CONFIG_OVERRIDE
#include "user_config_override.h"
#endif

#define HASP_RANDOM(x) rand() % x

#endif // HASP_MACRO_H
