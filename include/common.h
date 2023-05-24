/**
  ******************************************************************************
  * @file    common.h
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

#ifndef __COMMON_H__
#define __COMMON_H__


#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP32Time.h>

#define  S_TO_MS_FACTOR     1000
#define  TIMESTAMP_DEFAULT  1672531200   // January 1, 2023, 00:00:00 (seconds)
#define  TASK_PRIORITY      3

uint32_t hexStringToArray(const String & hexString, uint8_t *pArray);

uint32_t arrayToHexString(const uint8_t *pArray, uint32_t size, String & hexString);

void timeAdjustmentNotification(struct timeval *t);

void genericPacket(JSONVar & packet);

extern ESP32Time RTC;

#endif /* __COMMON_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/