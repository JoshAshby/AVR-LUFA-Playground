//-------------------------------------------
/*
GLOBAL.h
2011 - Josh Ashby
joshuaashby@joshashby.com
http://joshashby.com
http://github.com/JoshAshby
freenode/#linuxandsci - JoshAshby
*/
//-------------------------------------------
#ifndef GLOBAL_H
#define GLOBAL_H
//-----------------------------------------------
//Libraries
//-----------------------------------------------
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/power.h>

#include <compat/twi.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <LUFA/Drivers/Peripheral/ADC.h>
#include <LUFA/Drivers/Peripheral/TWI.h>

//-----------------------------------------------
//LUFA Stuff
//-----------------------------------------------

/* Function Prototypes: */
  void SetupHardware(void);
  void CDC_Task(void);

  void EVENT_USB_Device_Connect(void);
  void EVENT_USB_Device_Disconnect(void);
  void EVENT_USB_Device_ConfigurationChanged(void);
  void EVENT_USB_Device_ControlRequest(void);

//-----------------------------------------------
//Josh's Stuff
//-----------------------------------------------
//Debounce Stuff
#define DEBOUNCE_TIME       2
#define SAMPLE_FREQUENCY    10
#define MAXIMUM             (DEBOUNCE_TIME * SAMPLE_FREQUENCY)

//Debug
//========================

//CPU power and Debug LEDs
//========================

//Status LEDs
//========================

//Buttons
//========================
#define buttonLeft   0b00100100 //PORTB pin 2
#define buttonRight  0b00001000 //PORTB pin 3

#define buttonUp     0b01000000 //PORTE pin 6
#define buttonDown   0b00010000 //PORTB pin 4

#define buttonCenter 0b00010000 //PORTD pin 4

//Variables
//========================

//TWI addresses
//========================
#define MCP_ADDRESS 0xC0 //address of the MCP I2C DAC
//addresses of the IGT I2C gyroscope
#define G_ADDRESS 0x68
#define GX_H    0x1D
#define GX_L    0x1E
#define GY_H    0x1F
#define GY_L    0x20
#define GZ_H    0x21
#define GZ_L    0x22

//Functions
//========================
inline int debounce(int inputPin, char port) {
    uint8_t integrator = 0;
    int output         = 0;
    int pin            = 0;

    if (port == 'B')
        pin = (!(PINB & inputPin));
    else if (port == 'C')
        pin = (!(PINC & inputPin));
    else if (port == 'D')
        pin = (!(PIND & inputPin));
    else
        pin = (!(PINE & inputPin));

    for(int i=0; i < MAXIMUM; i++) {
        if (pin) {
            if (integrator > 0) {
                integrator--;
            }
        } else if (integrator < MAXIMUM) {
            integrator++;
        }
    }
    if (integrator == 0) {
        output = 0;
    } else if (integrator >= MAXIMUM) {
        integrator = MAXIMUM;
        output = 1;
    }
    return output;
}

#endif
