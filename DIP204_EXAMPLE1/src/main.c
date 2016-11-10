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


#  define EXAMPLE_ADC_POTENTIOMETER_CHANNEL   1
#  define EXAMPLE_ADC_POTENTIOMETER_PIN       AVR32_ADC_AD_1_PIN
#  define EXAMPLE_ADC_POTENTIOMETER_FUNCTION  AVR32_ADC_AD_1_FUNCTION

/** GPIO pin/adc-function map. */
const gpio_map_t ADC_GPIO_MAP = {
	{EXAMPLE_ADC_POTENTIOMETER_PIN, EXAMPLE_ADC_POTENTIOMETER_FUNCTION}
};



int main(void) {
	signed short adc_value_pot   = -1;
	/* Init system clocks */
	sysclk_init();

	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADC_GPIO_MAP, sizeof(ADC_GPIO_MAP) /
			sizeof(ADC_GPIO_MAP[0]));

	AVR32_ADC.mr |= 0x1 << AVR32_ADC_MR_PRESCAL_OFFSET;
	adc_configure(&AVR32_ADC);

	adc_enable(&AVR32_ADC, EXAMPLE_ADC_POTENTIOMETER_CHANNEL);

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
	dip204_printf_string("ES42 CAN EXAMPLE");
	dip204_hide_cursor();
	float velocity;
	while(1){
		adc_start(&AVR32_ADC);
		/* Get value for the potentiometer adc channel */
		adc_value_pot = adc_get_value(&AVR32_ADC, EXAMPLE_ADC_POTENTIOMETER_CHANNEL);
		/* Display value to user */
		velocity = ((float)adc_value_pot/1023)*100;
		dip204_clear_display();
		dip204_set_cursor_position(1,1);
		dip204_printf_string("%f", velocity);
		
		//Clear memory contents
		ClearMessages(msg);
		//Read any message available
		if(CANRxReady(0)){
			if( CANGetMsg(0, &Ident, msg, &mSize )) // Gets message and returns //TRUE if message received.
			{	
				// Evk1100PrintDisplay prints 4 message values, the Identifier and the data size on the display
				Evk1100PrintDisp(&Ident, msg, &mSize );
				delay_ms(100);
				
			}
		}
		// Send messages if possible
		if(CANTxReady(0))
		{
			msg[0]=0;
			msg[1]=0;
			msg[2]=0;
			msg[3]=0;
			msg[4]=0;
			msg[5]=0;
			msg[6]=0;
			msg[7]=velocity;
			// Channel, Identifier (max 0x1fffffff (29 bits)), Message, Number of bytes, R //or 0 (Remote frame or no remote frame).
			CANSendMsg( 0, 0x0cfe6cee, msg, 8, 0 );
			delay_ms(100);
		}
		dip204_clear_display();
	}
	
	return 0;
}