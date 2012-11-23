#include "menu.h"
#include "global.h"

static enum menuStates_t menuState=menuState_gyroX;

void LCDstart(void) {
    _delay_ms(1000);
    Serial_Init(9600, false);
    LCDupdate();
    brightness = 15;
    LCDbrightnessUpdate();

    ADC_SetupChannel(4);
    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL4);
    TWI_WritePacket(G_ADDRESS, 10, &DLPF_FS, sizeof(DLPF_FS), &gyroSetup, sizeof(gyroSetup));
    TWI_WritePacket(G_ADDRESS, 10, &SMPLRT_DIV, sizeof(SMPLRT_DIV), &gyroSampleRate, sizeof(gyroSampleRate));
}

void ButtonManage(void) {
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
                if (rightClick)
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
                    menuState=menuState_gyro;
                }
                if (rightClick) {
                    ADC_SetupChannel(4);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL4);
                    menuState=menuState_x;
                }
                break;
            case menuState_x:
                if (leftClick) {
                    menuState=menuState_adc;
                }
                if (rightClick) {
                    ADC_SetupChannel(5);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL5);
                    menuState=menuState_y;
                }
                break;
            case menuState_y:
                if (leftClick) {
                    ADC_SetupChannel(4);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL4);
                    menuState=menuState_x;
                }
                if (rightClick) {
                    ADC_SetupChannel(6);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL6);
                    menuState=menuState_z;
                }
                break;
            case menuState_z:
                if (leftClick) {
                    ADC_SetupChannel(5);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL5);
                    menuState=menuState_y;
                }
                if (rightClick) {
                    ADC_SetupChannel(7);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL7);
                    menuState=menuState_pot;
                }
                break;
            case menuState_pot:
                if (leftClick) {
                    ADC_SetupChannel(6);
                    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL6);
                    menuState=menuState_z;
                }
                if (rightClick) {
                    menuState=menuState_adc;
                }
                break;
            case menuState_gyro:
                if (upClick)
                    menuState=menuState_adc;
                if (downClick)
                    menuState=menuState_buttons;
                if (rightClick)
                    menuState=menuState_gyroX;
                break;
            case menuState_gyroX:
                InternalReadAddress = GX_L;
                InternalReadAddressHigh = GX_H;
/*                if (leftClick)
                    menuState=menuState_gyro;
                if (rightClick)
                    menuState=menuState_gyroY;*/
                break;
            case menuState_gyroY:
                InternalReadAddress = GY_L;
                InternalReadAddressHigh = GY_H;
                if (leftClick)
                    menuState=menuState_gyroX;
                if (rightClick)
                    menuState=menuState_gyroZ;
                break;
            case menuState_gyroZ:
                InternalReadAddress = GZ_L;
                InternalReadAddressHigh = GZ_H;
                if (leftClick)
                    menuState=menuState_gyroY;
                if (rightClick)
                    menuState=menuState_gyro;
                break;
            case menuState_buttons:
                if (upClick) {
                    menuState=menuState_gyro;
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
            update = true;
        }
    }
    switch (menuState) {
        case menuState_x:
        case menuState_y:
        case menuState_z:
        case menuState_pot:
            while (!(ADC_IsReadingComplete())) {};
            adc_value = ADC_GetResult();
            sprintf(adcStr, "%d", adc_value);
            LCDnextLine(adcStr);
            break;
        case menuState_gyroX:
        case menuState_gyroY:
        case menuState_gyroZ:
            TWI_ReadPacket(G_ADDRESS, 10, &InternalReadAddressHigh, sizeof(InternalReadAddressHigh), &GyroPacketHigh, sizeof(GyroPacketHigh));
            TWI_ReadPacket(G_ADDRESS, 10, &InternalReadAddress, sizeof(InternalReadAddress), &GyroPacket, sizeof(GyroPacket));
            gyroValue = GyroPacket | (GyroPacketHigh<<8);
            sprintf(gyroStr, "%d", gyroValue);
            LCDnextLine(gyroStr);
            _delay_ms(15);
            while (!(ADC_IsReadingComplete())) {};
            dacValue = ADC_GetResult();
//            dacValue = gyroValue;
            TWI_WritePacket(MCP_ADDRESS, 10, &dacOutput, sizeof(dacOutput), &dacValue, sizeof(dacValue));
            break;
        default:
            break;
    }
}

void LCDupdate(void) {
    if (update) {
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
                LCDwrite("ADC values");
                break;
            case menuState_gyro:
                LCDwrite("Gyro values");
        break;
            case menuState_x:
            case menuState_gyroX:
                LCDwrite("X");
                break;
            case menuState_y:
            case menuState_gyroY:
                LCDwrite("Y");
                break;
            case menuState_z:
            case menuState_gyroZ:
                LCDwrite("Z");
                break;
            case menuState_pot:
                LCDwrite("Pot");
                break;
            case menuState_buttons:
                LCDwrite("Buttons!");
                LCDnextLine(button);
                break;
            default:
                break;
        }
        update = false;
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
