#include "menu.h"
#include "global.h"

static enum menuStates_t menuState=menuState_init;

void LCDstart(void) {
    _delay_ms(1000);
    Serial_Init(9600, false);
    LCDupdate();
    brightness = 30;
    LCDbrightnessUpdate();
}

void LCDmanage(void) {
    int leftClick   = debounce(buttonLeft, 'B');
    int rightClick  = debounce(buttonRight, 'B');
    int upClick     = debounce(buttonUp, 'E');
    int downClick   = debounce(buttonDown, 'B');
    int centerClick = debounce(buttonCenter, 'D');
    if (!(leftClick || rightClick || upClick || downClick || centerClick)) {
        click = false;
    }

    if (!click) {
        switch (menuState) {
            case menuState_init:
                if (upClick)
                    menuState=menuState_settings;
                if (downClick)
                    menuState=menuState_adc;
                break;
            case menuState_settings:
                if (upClick)
                    menuState=menuState_buttons;
                if (downClick)
                    menuState=menuState_init;
                if (rightClick)
                    menuState=menuState_settings_brightness;
                break;
            case menuState_settings_brightness:
                if (leftClick)
                    menuState=menuState_settings;
                if (upClick) {
                    if ((brightness<=30) && ((brightness+5)<=30)) {
                        brightness = brightness + 5;
                    }
                }
                if (downClick) {
                    if ((brightness>=0) && ((brightness-5)>=0)) {
                        brightness = brightness - 5;
                    }
                }
                break;
            case menuState_adc:
                if (upClick) {
                    menuState=menuState_init;
                }
                if (downClick) {
                    menuState=menuState_buttons;
                }
                break;
            case menuState_buttons:
                if (upClick) {
                    menuState=menuState_adc;
                    button = NULL;
                }
                if (downClick) {
                    menuState=menuState_settings;
                    button = NULL;
                }
                if (leftClick)
                    button = "Left";
                if (rightClick)
                    button = "Right";
                if (centerClick)
                    button = "Center";
                break;
            default:
                break;
        }

        if (leftClick || rightClick || upClick || downClick || centerClick) {
            click = true;
            LCDupdate();
        }
    }
}

static void LCDupdate(void) {
    switch (menuState) {
        case menuState_init:
            LCDwrite("Hello, Josh!");
            break;
        case menuState_settings:
            LCDwrite("Settings:");
            LCDnextLine("Nothing!");
            break;
        case menuState_settings_brightness:
            LCDwrite("S: Brightness");
            sprintf(brightnessStr, "%d", brightness);
            LCDnextLine(brightnessStr);
            LCDbrightnessUpdate();
            break;
        case menuState_adc:
            while (!(ADC_IsReadingComplete())) {};
            adc_value = ADC_GetResult();
            LCDwrite("ADC #1");
            sprintf(adcStr, "%d", adc_value);
            LCDnextLine(adcStr);
            break;
        case menuState_buttons:
            LCDwrite("Buttons!");
            LCDnextLine(button);
            break;
        default:
            break;
    }
}

static void LCDwrite(char* data) {
        Serial_SendByte(0xFE);
        Serial_SendByte(0x01);
        Serial_SendString(data);
}

static void LCDnextLine(char* data) {
        Serial_SendByte(0xFE);
        Serial_SendByte(192);
        Serial_SendString(data);
}

static void LCDbrightnessUpdate(void) {
    Serial_SendByte(0x7C);
    Serial_SendByte(128+brightness);
    _delay_ms(250);
}
