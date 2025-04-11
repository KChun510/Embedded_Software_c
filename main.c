/*
 * embeddedSysProject.c
 * Created: 4/9/2025 7:20:24 PM
 * Author : westcoast
 */
 #include "avr.h"
 
 unsigned char valid_press = 0x00;

 void check_button_press(){
	 avr_wait(3000);
	 if(!(PINB & 0x02)){
		 valid_press = 0x01;
		 return;
	}
	else{
		 return;
	}
}

int main(void){
	// If you wanted to get access to the A series of ports you would use DDRA 
	// // Use PORT for writing
	// USE PIN for reading
	DDRB = DDRB | 0x01;
	avr_wait(3000);
	    while (1) {
		if(!(PINB & 0x02)){
			check_button_press();
		}
		if(valid_press){
			for(unsigned char i = 0; i < 0x03; ++i){
				PORTB = 0x01;
				avr_wait(500);
				PORTB = 0x00;
				avr_wait(500);
			}
			valid_press = 0x00;
		}else{
			PORTB = 0x00;
		}
    }
}

