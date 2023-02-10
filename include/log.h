#ifndef __LOG_H__
#define __LOG_H__

#include <Arduino.h>
#include <time.h>

#define  SYSTEM_LOG_ENABLED  1

#ifdef SYSTEM_LOG_ENABLED
    #if SYSTEM_LOG_ENABLED
        #define SYSTEM_LOGF(...) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.printf(__VA_ARGS__);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.printf(__VA_ARGS__);\
            }\
        }while(0)

        #define SYSTEM_LOGF_LN(...) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.printf(__VA_ARGS__);\
                Serial.println();\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.printf(__VA_ARGS__);\
                Serial.println();\
            }\
        }while(0)
    
        #define SYSTEM_LOG(log) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.print(log);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.print(log);\
            }\
        }while(0)

        #define SYSTEM_LOG_LN(log) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.println(log);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.println(log);\
            }\
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