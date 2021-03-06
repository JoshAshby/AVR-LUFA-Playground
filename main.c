#include "global.h"
#include "menu.h"

/** Contains the current baud rate and other settings of the virtual serial port. While this demo does not use
 *  the physical USART and thus does not use these settings, they must still be retained and returned to the host
 *  upon request or the host will assume the device is non-functional.
 *
 *  These values are set by the host via a class-specific request, however they are not required to be used accurately.
 *  It is possible to completely ignore these value or use other settings as the host is completely unaware of the physical
 *  serial link characteristics and instead sends and receives data in endpoint streams.
 */
static CDC_LineEncoding_t LineEncoding = { .BaudRateBPS = 0,
                                           .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                           .ParityType  = CDC_PARITY_None,
                                           .DataBits    = 8                            };


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
    SetupHardware();

    GlobalInterruptEnable();

    for (;;)
    {
        LCDmanage();
        USB_USBTask();
    }
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
//    USB_Init();

    DDRB  &= ~(1<<buttonLeft)
          &  ~(1<<buttonRight)
          &  ~(1<<buttonDown);
    DDRD  &= ~(1<<buttonCenter);
    DDRE  &= ~(1<<buttonUp);

    PORTB &= ~(1<<buttonLeft)
          &  ~(1<<buttonRight)
          &  ~(1<<buttonDown);
    PORTD &= ~(1<<buttonCenter);
    PORTE &= ~(1<<buttonUp);

    LCDstart();
    ADC_Init(ADC_FREE_RUNNING | ADC_PRESCALE_32);
    ADC_SetupChannel(6);
    ADC_StartReading(ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED | ADC_CHANNEL6);
}

void EVENT_USB_Device_Connect(void) {}
void EVENT_USB_Device_Disconnect(void) {}
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    /* Setup CDC Data Endpoints */
    ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(CDC_RX_EPADDR, EP_TYPE_BULK,  CDC_TXRX_EPSIZE, 1);

    /* Reset line encoding baud rate so that the host knows to send new values */
    LineEncoding.BaudRateBPS = 0;

}
void EVENT_USB_Device_ControlRequest(void)
{
    /* Process CDC specific control requests */
    switch (USB_ControlRequest.bRequest)
    {
        case CDC_REQ_GetLineEncoding:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();

                /* Write the line coding data to the control endpoint */
                Endpoint_Write_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
                Endpoint_ClearOUT();
            }

            break;
        case CDC_REQ_SetLineEncoding:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();

                /* Read the line coding data in from the host into the global struct */
                Endpoint_Read_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
                Endpoint_ClearIN();
            }

            break;
        case CDC_REQ_SetControlLineState:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();

                /* NOTE: Here you can read in the line state mask from the host, to get the current state of the output handshake
                         lines. The mask is read in from the wValue parameter in USB_ControlRequest, and can be masked against the
                         CONTROL_LINE_OUT_* masks to determine the RTS and DTR line states using the following code:
                */
            }

            break;
    }
}


/** Function to manage CDC data transmission and reception to and from the host. */
void CDCsend(char* ReportString) {
    static bool ActionSent = false;

    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
      return;

    /* Flag management - Only allow one string to be sent per action */
    if ((ReportString != NULL) && (ActionSent == false) && LineEncoding.BaudRateBPS)
    {
        ActionSent = true;

        /* Select the Serial Tx Endpoint */
        Endpoint_SelectEndpoint(CDC_TX_EPADDR);

        /* Write the String to the Endpoint */
        Endpoint_Write_Stream_LE(ReportString, strlen(ReportString), NULL);

        /* Remember if the packet to send completely fills the endpoint */
        bool IsFull = (Endpoint_BytesInEndpoint() == CDC_TXRX_EPSIZE);

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();

        /* If the last packet filled the endpoint, send an empty packet to release the buffer on
         * the receiver (otherwise all data will be cached until a non-full packet is received) */
        if (IsFull)
        {
            /* Wait until the endpoint is ready for another packet */
            Endpoint_WaitUntilReady();

            /* Send an empty packet to ensure that the host does not buffer data sent to it */
            Endpoint_ClearIN();
        }
    }
}

uint8_t CDCrecieve(void) {
    /* Select the Serial Rx Endpoint */
    Endpoint_SelectEndpoint(CDC_RX_EPADDR);

    /* Throw away any received data from the host */
    if (Endpoint_IsOUTReceived()) {
        Endpoint_ClearOUT();
    }
    return 0;
}

