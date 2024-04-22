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

void tempo_100ms() {
	volatile uint32_t duree;
	for(duree = 0; duree < 1120000 ; duree++) {
		;
	}
}

void tempo_250ms() {
	volatile uint32_t duree;
	for(duree = 0; duree < 2800000 ; duree++) {
		;
	}
}

void tempo_500ms() {
	volatile uint32_t duree;
	for(duree = 0; duree < 5600000 ; duree++) {
		;
	}
}

uint32_t mesure_potentiometre(){
	ADC1.CR2 |= (1<<30);
	while( !(ADC1.SR&(1<<1)) );
	return ADC1.DR;
}

void generate_sequence(){
    char rand = (temps_total / 100) % 4;
    switch (rand){
        case 0:
            LED_on(&GPIOA,0);
			break;
        case 1:
			LED_on(&GPIOA,1);
			break;
        case 2:
            LED_on(&GPIOB,10);
			break;
        case 3:
            LED_on(&GPIOC,7);
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

void show_sequence(char* sequence, char counter, char index){
    char led;
	char tmp;
    uint8_t i;
    for(i=0; i<counter; i++){
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
	if(index >= 1){
		allumer_led((sequence[i] >> 6) & 0x3);
	}
	if(index >= 2){
		allumer_led((sequence[i] >> 4) & 0x3);
	}
	if(index >= 3){
		allumer_led((sequence[i] >> 2) & 0x3);
	}
}

int main() {
	initialisation(300);
	uint32_t pot = mesure_potentiometre();
	char debut = 0;
	char sequence[3];

	sequence[0] = 0xEC;
	sequence[1] = 0x98;
	sequence[2] = 0x54;

	while (1) {

		/*---------------ALEATOIRE-------------*/
		// generate_sequence();	// pour un rendu plus rapide modifier le tempo 500 ms
		// tempo_500ms();

		// LED_off(&GPIOA,0);
		// LED_off(&GPIOA,1);
		// LED_off(&GPIOB,10);
		// LED_off(&GPIOC,7);

		/*---------------LECTURE SEQUENCE-------------*/

		show_sequence(sequence, 2, 3);

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

	}

	return 0;
}
