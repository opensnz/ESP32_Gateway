/**
  ******************************************************************************
  * @file    system.h
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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <Arduino.h>
#include <SPIFFS.h>
//#include <LittleFS.h>
#include "log.h"

#define SYSTEM_LOG_BAUDRATE  115200

#ifdef SYSTEM_LOG_ENABLED
    #if SYSTEM_LOG_ENABLED
        #define SYSTEM_LOG_BEGIN(baudrate)   Serial.begin(baudrate)
    #else
        #define SYSTEM_LOG_BEGIN(baudrate)
    #endif
#else
    #define SYSTEM_LOG_BEGIN(baudrate)
#endif

// File System : SPIFFS (will be deprecated)
#define FILE_SYSTEM          SPIFFS

class SystemClass
{
private:
    bool taskBeforePowerOFF(void);

public:
    bool begin(void);
    bool exists(String path);
    bool readFile(String path, String & content);
    bool writeFile(String path, String & content);
    bool removeFile(String path);
    bool readFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos = 0);
    bool writeFile(String path, uint8_t *pContent, uint32_t size, uint32_t pos = 0);
    bool getSystemInfo(String & info);
    void restart(void);
};


/******************* Exported Global Variables ************************/

extern SystemClass System;


#endif /* __SYSTEM_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/