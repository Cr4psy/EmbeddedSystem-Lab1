/********************************************************
Name : main.c

CAN example

**********************************************************/

#include <asf.h>
#include "can.h"

#define CAN_1000kbps 1
#define CAN_500kbps 5
#define CAN_250kbps 7
#define CAN_125kbps 10

UINT32 Ident;
UINT8 msg[8], mSize;

int main(void) {
	//spidatareadpointer=&spidataread;
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
	
	// Configures the MCP2515 SPI communication.
	config_SPI_SPARE();

	// Enables receive interrupts.
	Disable_global_interrupt();
	INTC_init_interrupts();
	Enable_global_interrupt();
	
	// Delay to let the Oscillator get started
	delay_init( FOSC0 );
	
	// Initializes the display
	config_dpi204();
	dip204_init(100,1);
	dip204_clear_display();
	
	UINT16 Mask = 0; 
	UINT16 flt = 0;
	UINT16 Flt[] = {flt,flt,flt,flt,flt,flt};
	InitializeCAN(0, CAN_250kbps, Mask, Flt);
	
	dip204_set_cursor_position(1,1);
	dip204_printf_string("ES1 CAN EXAMPLE");
	dip204_hide_cursor();

	while(1){
		//Clear memory contents
		ClearMessages(msg);
		//Read any message available
		if(CANRxReady(0)){
			if( CANGetMsg(0, &Ident, msg, &mSize )) // Gets message and returns //TRUE if message received.
			{	
				// Evk1100PrintDisplay prints 4 message values, the Identifier and the data size on the display
				dip204_clear_display();
				Evk1100PrintDisp(&Ident, msg, &mSize );
				delay_ms(1000);
				dip204_clear_display();
			}
		}
		// Send messages if possible
		if(CANTxReady(0))
		{
			msg[0]=0;
			msg[1]=100;
			msg[2]=100;
			msg[3]=0;
			msg[4]=100;
			msg[5]=0x06;
			msg[6]=0x07;
			msg[7]=50;
			// Channel, Identifier (max 0x1fffffff (29 bits)), Message, Number of bytes, R //or 0 (Remote frame or no remote frame).
			CANSendMsg( 0, 0x01, msg, 8, 0 );
			delay_ms(1000);
		}
	}
	return 0;
}