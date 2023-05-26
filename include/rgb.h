/**
  ******************************************************************************
  * @file    rgb.h
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

#ifndef __RGB_H__
#define __RGB_H__

#include <Arduino.h>


#define RGB_GPIO_RED             GPIO_NUM_13
#define RGB_GPIO_GREEN           GPIO_NUM_12
#define RGB_GPIO_BLUE            GPIO_NUM_2

#define RGB_PWM_FREQUENCY        5000

#define RGB_ANALOG_LED_OFF       255
#define RGB_ANALOG_LED_ON        0


class RGBClass
{
private:
    bool red;
    bool green;
    bool blue;
    bool hasColorChanged;
    void setup(void);
    void loop(void);
    void turnOffUnselectedLED(void);
    void changeBrightness(uint8_t value);

public:
    void main(void);
    void selectColor(bool red, bool green, bool blue);
    bool isRedColor(void);
    bool isGreenColor(void);
    bool isBlueColor(void);

};

/******************* Exported Global Variables ************************/

extern RGBClass RGB;
extern TaskHandle_t hRGB;


#endif /* __RGB_H__ */

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/