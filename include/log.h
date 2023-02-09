#ifndef __LOG_H__
#define __LOG_H__

#include <Arduino.h>
#include <time.h>

#define  APP_LOG_ENABLED  1

#ifdef APP_LOG_ENABLED
    #if APP_LOG_ENABLED
        #define APP_LOGF(...) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.printf(__VA_ARGS__);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.printf(__VA_ARGS__);\
            }\
        }while(0)

        #define APP_LOGF_LN(...) do{ \
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
    
        #define APP_LOG(log) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.print(log);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.print(log);\
            }\
        }while(0)

        #define APP_LOG_LN(log) do{ \
            struct tm timeinfo;\
            if(getLocalTime(&timeinfo)){\
                Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S --> ");\
                Serial.println(log);\
            }else{\
                Serial.print("No time available (yet) --> ");\
                Serial.println(log);\
            }\
        }while(0)

        #define APP_PRINTF(...)  Serial.printf(__VA_ARGS__)

        #define APP_PRINTF_LN(...)  Serial.printf(__VA_ARGS__);Serial.println()

        #define APP_PRINT(log)  Serial.print(log)

        #define APP_PRINT_LN(log)  Serial.println(log)
    #else
        #define APP_LOGF(...)
        #define APP_LOGF_LN(...)
        #define APP_LOG(log)
        #define APP_LOG_LN(log)
        #define APP_PRINTF(...)
        #define APP_PRINTF_LN(...)
        #define APP_PRINT(log)
        #define APP_PRINT_LN(log)
    #endif
#else
    #define APP_LOGF(...)
    #define APP_LOGF_LN(...)
    #define APP_LOG(log)
    #define APP_LOG_LN(log)
    #define APP_PRINT(...)
    #define APP_PRINT_LN(...) 
    #define APP_PRINT(log)
    #define APP_PRINT_LN(log)
#endif


#endif /* __LOG_H__ */