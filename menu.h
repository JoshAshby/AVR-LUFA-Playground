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
    menuState_buttons
};

static bool  click  = false;
static char* button = NULL;
static uint8_t brightness = 15;
static char brightnessStr[2];
static uint16_t adc_value = 5;
static char adcStr[10];

void LCDstart(void);
void LCDmanage(void);
static void LCDupdate(void);
static void LCDwrite(char* data);
static void LCDnextLine(char* data);
static void LCDbrightnessUpdate(void);

#endif
