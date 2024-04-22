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

int main() {
	initialisation(300);
	//LED_on(&GPIOA,0);
	//LED_on(&GPIOA,1);
	//LED_on(&GPIOB,10);
	//LED_on(&GPIOC,7);
	uint32_t pot = 0;

	while (1) {
		pot = mesure_potentiometre();

		LED_off(&GPIOA,0);
		LED_off(&GPIOA,1);
		LED_off(&GPIOB,10);
		LED_off(&GPIOC,7);
		if (pot < 1024) {
			LED_on(&GPIOA,0);
		}
		else if (pot < 2048) {
			LED_on(&GPIOA,1);
			_putc('2');
		}
		else if(pot < 3072) {
			LED_on(&GPIOB,10);
			_putc('3');
		}
		else if (pot <= 4096) {
			LED_on(&GPIOC,7);
			_putc('4');
		}
		LED_on(&GPIOB, 9);
		tempo_500ms();
		tempo_500ms();
		LED_off(&GPIOB, 9);
		tempo_500ms();
		tempo_500ms();
	}

	return 0;
}
