/**
  ******************************************************************************
  * @file    rgb.cpp
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

#include "rgb.h"
#include "log.h"

RGBClass RGB;
TaskHandle_t hRGB  = NULL;

void RGBClass::setup(void){
    pinMode(RGB_GPIO_RED, OUTPUT);
    analogWrite(RGB_GPIO_RED, RGB_ANALOG_LED_ON);
    pinMode(RGB_GPIO_GREEN, OUTPUT);
    analogWrite(RGB_GPIO_GREEN, RGB_ANALOG_LED_OFF);
    pinMode(RGB_GPIO_BLUE, OUTPUT);
    analogWrite(RGB_GPIO_BLUE, RGB_ANALOG_LED_OFF);
    this->selectColor(true, false, false);
    delay(100);
}

void RGBClass::loop(void){
    while(true)
    {
        if(this->isColorChanged)
        {
            this->turnOffUnselectedLED();
        }
        // Decrease brightness (1300 milliseconds)
        for(uint8_t i=0; i<=255; i=i+5)
        {
            this->changeBrightness(i);
            delay(25);
        }
        delay(200);
        // Increase brightness (1300 milliseconds)
        for(uint8_t i=255; i>=0; i=i-5)
        {
            this->changeBrightness(i);
            delay(25);
        }
        delay(200);
    }
}

void RGBClass::turnOffUnselectedLED(void){
    if(!this->red)
    {
        analogWrite(RGB_GPIO_RED, RGB_ANALOG_LED_OFF);
    }
    if(!this->green)
    {
        analogWrite(RGB_GPIO_GREEN, RGB_ANALOG_LED_OFF);
    }
    if(!this->blue)
    {
        analogWrite(RGB_GPIO_BLUE, RGB_ANALOG_LED_OFF);
    }
    this->isColorChanged = false;
}

void RGBClass::changeBrightness(uint8_t value){
    if(this->red)
    {
        analogWrite(RGB_GPIO_RED, value);
    }
    if(this->green)
    {
        analogWrite(RGB_GPIO_GREEN, value);
    }
    if(this->blue)
    {
        analogWrite(RGB_GPIO_BLUE, value);
    }
}

void RGBClass::main(void){
    SYSTEM_PRINT_LN("RGB setting...");
    this->setup();
    SYSTEM_PRINT_LN("RGB running...");
    this->loop();
}

void RGBClass::selectColor(bool red, bool green, bool blue){
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->isColorChanged = true;
}

/*********************** (C) COPYRIGHT OpenSnz Technology *****END OF FILE****/