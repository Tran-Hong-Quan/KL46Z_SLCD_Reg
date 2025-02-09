////////////////////////////////////////
////Tac gia/author: Tran Hong Quan//////
////////////////////////////////////////

#include "MKL46Z4.h"                    // Device:Startup
#include "SLCD.h"
#include <stdbool.h>

void SLCD_Init(void){

	MCG->C1 |= MCG_C1_IRCLKEN_MASK 	//Internal Reference Select
		| MCG_C1_IREFSTEN_MASK;				//Internal Reference Stop Enable
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK 
		| SIM_SCGC5_PORTC_MASK 
		| SIM_SCGC5_PORTD_MASK 
		| SIM_SCGC5_PORTE_MASK 
		| SIM_SCGC5_SLCD_MASK; 
		
	LCD->GCR |= LCD_GCR_RVEN(0) //Regulated voltage disabled
		| LCD_GCR_RVTRIM(0)				//Regulated Voltage Trim 0
		| LCD_GCR_CPSEL(1)				//LCD charge pump is selected. Resistor network disabled
		| LCD_GCR_LADJ(3)					//Load Adjust, Slowest clock source for charge pump (LCD glass capacitance 1000 pF or 500pF or lower if FFR is set)	
		| LCD_GCR_VSUPPLY(0)			//Drive VLL3 internally from VDD
		| LCD_GCR_PADSAFE(0)			//LCD frontplane and backplane functions enabled according to other LCD control bits
		| LCD_GCR_FDCIEN(0)				//LCD Fault Detection Complete Interrupt Disenable, No interrupt request is generated by this event
		| LCD_GCR_ALTDIV(0)				//LCD AlternateClock Divider, Divide factor = 1 (No divide)
		| LCD_GCR_ALTSOURCE(0) 		//Select Alternate Clock Source 1 (default)
		| LCD_GCR_FFR(0)					//Fast Frame Rate Select, Standard Frame Rate LCD Frame Freq: 23.3 (min) 73.1 (max)
		| LCD_GCR_LCDDOZE(0)			//LCD Doze Disable, Allows the LCD driver, charge pump, resistor bias network, and voltage regulator to continue running during Doze mode
		| LCD_GCR_LCDSTP(0)				//LCD Stop, Allows the LCD driver, charge pump, resistor bias network, and voltage regulator to continue running during Stop mode
		| LCD_GCR_LCDEN(1)				//LCD Driver Enable, Starts LCD controller waveform generator
		| LCD_GCR_SOURCE(1)				//LCD Clock Source Select the default clock as the LCD clock source.
		| LCD_GCR_LCLK(4)					//LCD Clock Prescaler, Used as a clock divider to generate the SLCD frame frequency,LCD controller frame frequency = LCD clock/((DUTY + 1) � 8 � (4 + LCLK[2:0]) � Y)
		| LCD_GCR_DUTY(3);				//LCD duty select, 011 Use 4 BP (1/4 duty cycle)
	
	LCD->PEN[0] = 0x000E0D80; 	//Low Front plane for LCD pin 7 8 10 11 17  
	LCD->PEN[1] = 0x00300160; 	//High Front plane for LCD pin 37 38 53
	
	LCD->BPEN[0] = 0x000C0000;	//Low Back plane for 19 18 
	LCD->BPEN[1] = 0x00100100;	//High Back plane for 40 52
	
	//Set Wave phase for back plane
	LCD->WF8B[SLCD_COM_0] |= SLCD_WAVE_PHASE_A;
	LCD->WF8B[SLCD_COM_1] |= SLCD_WAVE_PHASE_B;
	LCD->WF8B[SLCD_COM_2] |= SLCD_WAVE_PHASE_C;
	LCD->WF8B[SLCD_COM_3] |= SLCD_WAVE_PHASE_D;
}

void SLCD_DisplayAll(){
	LCD->WF8B[SLCD_DIG_1_0] |= 0x0F;
	LCD->WF8B[SLCD_DIG_1_1] |= 0x0F;
	LCD->WF8B[SLCD_DIG_2_0] |= 0x0F;
	LCD->WF8B[SLCD_DIG_2_1] |= 0x0F;
	LCD->WF8B[SLCD_DIG_3_0] |= 0x0F;
	LCD->WF8B[SLCD_DIG_3_1] |= 0x0F;
	LCD->WF8B[SLCD_DIG_4_0] |= 0x0F;
	LCD->WF8B[SLCD_DIG_4_1] |= 0x0F;
}

void SLCD_DisplayError(){
	LCD->WF8B[SLCD_DIG_1_0] = SLCD_SEG_D | SLCD_SEG_E | SLCD_SEG_F | SLCD_SEG_G;
	LCD->WF8B[SLCD_DIG_1_1] = SLCD_SEG_A;
	LCD->WF8B[SLCD_DIG_2_0] = SLCD_SEG_E | SLCD_SEG_G;
	LCD->WF8B[SLCD_DIG_2_1] = SLCD_NO_WAVE_PHASE;
	LCD->WF8B[SLCD_DIG_3_0] = SLCD_SEG_E | SLCD_SEG_G;
	LCD->WF8B[SLCD_DIG_3_1] = SLCD_NO_WAVE_PHASE;
	LCD->WF8B[SLCD_DIG_4_0] = SLCD_SEG_D | SLCD_SEG_E | SLCD_SEG_G;
	LCD->WF8B[SLCD_DIG_4_1] = SLCD_SEG_C;
}

void SLCD_StartBlink(){
	LCD->AR = LCD_AR_BLINK(1) //Starts blinking at blinking frequency specified by LCD blink rate calculation
		| LCD_AR_BMODE(0)        //Display blank during the blink period
		| LCD_AR_BRATE(0);       //LCD controller blink rate = LCD clock /2^(12 + BRATE)
}

void SLCD_StopBlink(){
	LCD->AR = LCD_AR_BLINK(0); //Disables blinking.
}

void SLCD_OffDisplay(){
	LCD->AR = LCD_AR_BLANK(1); //Blank display mode
}

void SLCD_OnDisplay(){
	LCD->AR = LCD_AR_BLANK(1); //Normal or alternate display mode
}

void SLCD_SetNum(uint8_t dig, uint8_t num, bool isShowZero){
	if(num > 9 || dig > 5 || dig == 0){
		SLCD_DisplayError();
		return;
	}
	
	int pin0,pin1;
	switch(dig){
		case 1:
			pin0 = SLCD_DIG_1_0;
			pin1 = SLCD_DIG_1_1;
			break;
		case 2:
			pin0 = SLCD_DIG_2_0;
			pin1 = SLCD_DIG_2_1;
			break;
		case 3:
			pin0 = SLCD_DIG_3_0;
			pin1 = SLCD_DIG_3_1;
			break;
		case 4:
			pin0 = SLCD_DIG_4_0;
			pin1 = SLCD_DIG_4_1;
			break;
	}
	
	switch(num){
		case 0:
			if(isShowZero)
			{
				LCD->WF8B[pin0] = SLCD_SEG_E | SLCD_SEG_D | SLCD_SEG_F;
				LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B | SLCD_SEG_C;
			}
			else
			{
				LCD->WF8B[pin0] = SLCD_NO_WAVE_PHASE;
				LCD->WF8B[pin1] = SLCD_NO_WAVE_PHASE;
			}	
			break;
		case 1:
			LCD->WF8B[pin0] = SLCD_NO_WAVE_PHASE;
			LCD->WF8B[pin1] = SLCD_SEG_B | SLCD_SEG_C;
			break;
		case 2:
			LCD->WF8B[pin0] = SLCD_SEG_E | SLCD_SEG_D | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B;
			break;
		case 3:
			LCD->WF8B[pin0] = SLCD_SEG_D | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B | SLCD_SEG_C;
			break;
		case 4:
			LCD->WF8B[pin0] = SLCD_SEG_F | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_B | SLCD_SEG_C;
			break;
		case 5:
			LCD->WF8B[pin0] = SLCD_SEG_D | SLCD_SEG_F | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_C;
			break;
		case 6:
			LCD->WF8B[pin0] = SLCD_SEG_E | SLCD_SEG_D | SLCD_SEG_F | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_C;
			break;
		case 7:
			LCD->WF8B[pin0] = SLCD_NO_WAVE_PHASE;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B | SLCD_SEG_C;
			break;
		case 8:
			LCD->WF8B[pin0] = SLCD_SEG_E | SLCD_SEG_D | SLCD_SEG_F | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B | SLCD_SEG_C;
			break;
		case 9:
			LCD->WF8B[pin0] = SLCD_SEG_D | SLCD_SEG_F | SLCD_SEG_G;
			LCD->WF8B[pin1] = SLCD_SEG_A | SLCD_SEG_B | SLCD_SEG_C;
			break;
	}
}

void SLCD_DisplayDemical(uint16_t val){
	if(val > 9999)
	{
		SLCD_DisplayError();
		return;
	}
	uint8_t seg1 = val / 1000;
	uint8_t seg2 = (val - (val / 1000) * 1000) / 100;
	uint8_t seg3 = (val - (val / 100) * 100) / 10;
	uint8_t seg4 = val - (val / 10) * 10;
	
	SLCD_SetNum(1, seg1, false);
	SLCD_SetNum(2, seg2, seg1 > 0);
	SLCD_SetNum(3, seg3, seg1 > 0 || seg2 > 0);
	SLCD_SetNum(4, seg4, true);
}

void SLCD_DisplayDP1(){
	LCD->WF8B[SLCD_DIG_1_1] |= SLCD_SEG_DP_COL;
}

void SLCD_StopDisplayDP1(){
	LCD->WF8B[SLCD_DIG_1_1] &= ~SLCD_SEG_DP_COL;
}

void SLCD_DisplayDP2(){
	LCD->WF8B[SLCD_DIG_2_1] |= SLCD_SEG_DP_COL;
}

void SLCD_StopDisplayDP2(){
	LCD->WF8B[SLCD_DIG_2_1] &= ~SLCD_SEG_DP_COL;
}

void SLCD_DisplayDP3(){
	LCD->WF8B[SLCD_DIG_3_1] |= SLCD_SEG_DP_COL;
}

void SLCD_StopDisplayDP3(){
	LCD->WF8B[SLCD_DIG_3_1] &= ~SLCD_SEG_DP_COL;
}

void SLCD_DisplayCOL(){
	LCD->WF8B[SLCD_DIG_4_1] |= SLCD_SEG_DP_COL;
}

void SLCD_StopDisplayCOL(){
	LCD->WF8B[SLCD_DIG_4_1] &= ~SLCD_SEG_DP_COL;
}






















