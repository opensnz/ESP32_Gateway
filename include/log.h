/**
  ******************************************************************************
  * @file    log.h
  * @author  OpenSnz IoT Team
  * @version 1.0
  ******************************************************************************
  * @attention
  * 
    Copyright (C) 2023 OpenSnz Technology - All Rights Reserved.

    THE CONTENTS OF THIS PROJECT ARE PROPRIETARY AND CONFIDENTIAL.
    UNAUTHORIZED COPYING, TRANSFERRING OR REPRODUCTION OF THE CONTENTS OF THIS PROJECT, VIA ANY MEDIUM IS STRICTLY PROHIBITED.

    The receipt or possession of the source code and/or any parts thereof does not convey or imply any right to use them
    for any purpose other than the purpose for which they were provided to you.

    The software is provided "AS IS", without warranty of any kind, express or implied, including but not limited to
    the warranties of merchantability, fitness for a particular purpose and non infringement.
    In no event shall the authors or copyright holders be liable for any claim, damages or other liability,
    whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software
    or the use or other dealings in the software.

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  *
  ******************************************************************************
  */ 

#ifndef __LOG_H__
#define __LOG_H__

#include <Arduino.h>
#include <ESP32Time.h>

extern ESP32Time RTC;

#define  SYSTEM_LOG_ENABLED  1

#ifdef SYSTEM_LOG_ENABLED
    #if SYSTEM_LOG_ENABLED
        #define SYSTEM_LOGF(...) do{ \
                Serial.print(RTC.getDateTime(true));\
                Serial.print(" --> ");\
                Serial.printf(__VA_ARGS__);\
            }while(0)

        #define SYSTEM_LOGF_LN(...) do{ \
                SYSTEM_LOGF(__VA_ARGS__);\
                Serial.println();\
            }while(0)
    
        #define SYSTEM_LOG(log) do{ \
                Serial.print(RTC.getDateTime(true));\
                Serial.print(" --> ");\
                Serial.print(log);\
            }while(0)

        #define SYSTEM_LOG_LN(log) do{ \
                SYSTEM_LOG(log);\
                Serial.println();\
            }while(0)

        #define SYSTEM_PRINTF(...)  Serial.printf(__VA_ARGS__)

        #define SYSTEM_PRINTF_LN(...)  Serial.printf(__VA_ARGS__);Serial.println()

        #define SYSTEM_PRINT(log)  Serial.print(log)

        #define SYSTEM_PRINT_LN(log)  Serial.println(log)
    #else
        #define SYSTEM_LOGF(...)
        #define SYSTEM_LOGF_LN(...)
        #define SYSTEM_LOG(log)
        #define SYSTEM_LOG_LN(log)
        #define SYSTEM_PRINTF(...)
        #define SYSTEM_PRINTF_LN(...)
        #define SYSTEM_PRINT(log)
        #define SYSTEM_PRINT_LN(log)
    #endif
#else
    #define SYSTEM_LOGF(...)
    #define SYSTEM_LOGF_LN(...)
    #define SYSTEM_LOG(log)
    #define SYSTEM_LOG_LN(log)
    #define SYSTEM_PRINT(...)
    #define SYSTEM_PRINT_LN(...) 
    #define SYSTEM_PRINT(log)
    #define SYSTEM_PRINT_LN(log)
#endif


#endif /* __LOG_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/