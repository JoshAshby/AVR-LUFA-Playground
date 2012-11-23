//-------------------------------------------
/*
MENU.h
2012 - Josh Ashby
joshuaashby@joshashby.com
http://joshashby.com
http://github.com/JoshAshby
freenode/#linuxandsci - JoshAshby
*/
//-------------------------------------------
#ifndef MENU_H
#define MENU_H
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

static enum menuStates_t {
    menuState_init=0,
    menuState_settings,
    menuState_settings_brightness,
    menuState_adc,
    menuState_x,
    menuState_y,
    menuState_z,
    menuState_pot,
    menuState_gyro,
    menuState_gyroX,
    menuState_gyroY,
    menuState_gyroZ,
    menuState_buttons
};

static bool  click  = false;
static bool update = true;
static char* button = NULL;
static uint8_t brightness = 15;
static char brightnessStr[2];
static uint16_t adc_value = 0;
static char adcStr[10];
static uint16_t gyro_value = 0;
static char gyroStr[10];

void LCDstart(void);
void ButtonManage(void);
void LCDupdate(void);
static void LCDwrite(char* data);
static void LCDnextLine(char* data);
static void LCDbrightnessUpdate(void);

#endif
