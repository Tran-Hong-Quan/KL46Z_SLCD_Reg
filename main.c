#include "MKL46Z4.h"                    // Device:Startup
#include "SLCD.h"

void delay(uint32_t value);

int main(void){
	SLCD_Init();
	int num = 0;
	while(1){
		SLCD_DisplayError();
		delay(3000000);
		delay(3000000);
		SLCD_OffDisplay();
		delay(3000000);
		SLCD_OnDisplay();
		delay(3000000);
		SLCD_StartBlink();
		delay(3000000);
		SLCD_StopBlink();
		delay(3000000);
				
		num = 0;
		while(num < 10000)
		{
			SLCD_DisplayDemical(num);
			SLCD_DisplayCOL();
			num++;
			delay(10000);
		}
	}
}

void delay(uint32_t value){
	while(value>0)
	{
		__NOP();
		value--;
	}
}
