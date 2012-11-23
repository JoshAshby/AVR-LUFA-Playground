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
static char gyroStr[10];

static uint8_t InternalReadAddress = 0x1B;
static uint8_t GyroPacket;
static uint8_t InternalReadAddressHigh = 0x1C;
static uint8_t GyroPacketHigh;
static uint16_t gyroValue = 5;

static uint8_t WHO_AM_I = 0x00;
static uint8_t SMPLRT_DIV= 0x15;
static uint8_t DLPF_FS = 0x16;

static uint8_t gyroSetup = 0x18;
static uint8_t gyroSampleRate = 0x00;

static uint16_t dacValue;
static uint8_t dacOutput = 0x68;

void LCDstart(void);
void ButtonManage(void);
void LCDupdate(void);
static void LCDwrite(char* data);
static void LCDnextLine(char* data);
static void LCDbrightnessUpdate(void);

#endif
