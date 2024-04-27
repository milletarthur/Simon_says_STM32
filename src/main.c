#include "sys/devices.h"
#include "sys/init.h"
#include "sys/cm4.h"
#include <stdint.h>
#include "libsimon.c"

static volatile uint32_t temps_total = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
void SysTick_Handler(){
	temps_total+=300;
	if(temps_total >= 3000)
		temps_total = 0;
}
#pragma GCC diagnostic pop

void init_USART() {
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFFF0F) | 0xA0;
	GPIOA.AFRL = (GPIOA.AFRL & 0xFFFF00FF) | 0x7700;
	USART2.BRR = get_APB1CLK()/9600;
	USART2.CR3 = 0;
	USART2.CR2 = 0;
}

void _putc(char c) {
	while ((USART2.SR & 0x80) == 0) ;
	USART2.DR = c;
}

uint32_t mesure_potentiometre(){
	ADC1.CR2 |= (1<<30);
	while( !(ADC1.SR&(1<<1)) );
	return ADC1.DR;
}

void generate_sequence(char* sequence, uint8_t counter, char index){
    char rand = (temps_total / 100) % 4;
    switch (index){
        case 0:
            // LED_on(&GPIOA,0);
			sequence[counter] = rand;
			break;
        case 1:
			// LED_on(&GPIOA,1);
			sequence[counter] |= (rand & 0x3) << 2;
			break;
        case 2:
			// LED_on(&GPIOB,10);
            sequence[counter] |= (rand & 0x3) << 4;
			break;
        case 3:
			// LED_on(&GPIOC,7);
            sequence[counter] |= (rand & 0x3) << 6;
			break;
        default:
			break;
	}
}

void allumer_led(char led){
    switch (led){
        case 0x0:
			_putc('0');
            LED_on(&GPIOA,0);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOA,0);
			break;
        case 0x1:
			_putc('1');
            LED_on(&GPIOA,1);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOA,1);
			break;
        case 0x2:
			_putc('2');
            LED_on(&GPIOB,10);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOB,10);
			break;
        case 0x3:
			_putc('3');
            LED_on(&GPIOC,7);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOC,7);
			break;
        default:
			break;
	}
}

void show_sequence(char* sequence, uint8_t counter, char index){
    char led;
	char tmp;
	char c1, c2, c3;
    uint8_t i;
    for(i=0; i<counter; i++){
		if(i == counter-1){
			c1 = (sequence[i] >> 0x6) & 0x3;
			c2 = (sequence[i] >> 0x4) & 0x3;
			c3 = (sequence[i] >> 0x2) & 0x3;
			if(index == 1)
				allumer_led(c1);
			if(index == 2){
				allumer_led(c1);
				allumer_led(c2);
			}
			if(index == 3){
				allumer_led(c1);
				allumer_led(c2);
				allumer_led(c3);
			}
			break;
		}
			tmp = sequence[i];
			led = (tmp >> 6) & 0x3;
			allumer_led(led);
			led = (tmp >> 4) & 0x3;
			allumer_led(led);
			led = (tmp >> 2) & 0x3;
			allumer_led(led);
			led = tmp & 0x3;
			allumer_led(led);
    }
	// if(index == 1){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// }
	// if(index == 2){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// 	allumer_led((sequence[i] >> 4) & 0x3);
	// }
	// if(index == 3){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// 	allumer_led((sequence[i] >> 4) & 0x3);
	// 	allumer_led((sequence[i] >> 2) & 0x3);
	// }
}

char verif(char* sequence, uint8_t counter, char* proposition){
	for(uint8_t i=0; i<counter; i++){
		if(sequence[i] != proposition[i])
			return 1;
	}
	return 0;
}

int main() {
	initialisation(300);
	uint32_t pot = mesure_potentiometre();
	char debut = 0;
	char sequence[3];
	char proposition[3];
	uint8_t counter = 0;
	char index = 0;

	// for(index=0; index<3; index++){
	// 	generate_sequence(sequence, counter, index);
	// }
	
	// show_sequence(sequence, counter, index);

	sequence[0] = 0xEC;
	sequence[1] = 0x98;
	sequence[2] = 0x54;
	
	for(int i=0; i<3; i++){
		proposition[i] = sequence[i];
	}

	char c = verif(sequence, 2, 1, proposition);
	if(c == 0)
		_putc('0');
	else
		_putc('1');

	// while (1) {


		/*---------------ALEATOIRE-------------*/
		// generate_sequence();	// pour un rendu plus rapide modifier le tempo 500 ms
		// tempo_500ms();

		// LED_off(&GPIOA,0);
		// LED_off(&GPIOA,1);
		// LED_off(&GPIOB,10);
		// LED_off(&GPIOC,7);

		/*---------------LECTURE SEQUENCE-------------*/

		// show_sequence(sequence, 3, 3);

		/*---------------POTENTIOMETRE-------------*/

		// pot = mesure_potentiometre();
		
		// if (pot < 1024) {
		// 	LED_on(&GPIOA,0);
		// }
		// else if (pot < 2048) {
		// 	LED_on(&GPIOA,1);
		// 	_putc('2');
		// }
		// else if(pot < 3072) {
		// 	LED_on(&GPIOB,10);
		// 	_putc('3');
		// }
		// else if (pot <= 4096) {
		// 	LED_on(&GPIOC,7);
		// 	_putc('4');
		// }
		// LED_on(&GPIOB, 9);
		// tempo_500ms();
		// tempo_500ms();
		// LED_off(&GPIOB, 9);
		// tempo_500ms();
		// tempo_500ms();

	// }

	return 0;
}
