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

void _putc(const char c) {
	while ((USART2.SR & 0x80) == 0) ;
	USART2.DR = c;
}

void tempo_500ms() {
	volatile uint32_t duree;
	for(duree = 0; duree < 5600000 ; duree++) {
		;
	}
}

int main() {
	initialisation(300);
	LED_off(&GPIOA,0);
	LED_off(&GPIOA,1);
	LED_off(&GPIOB,10);
	LED_off(&GPIOC,7);

	uint16_t size = 0;

	for (uint16_t i = 0; i<=size; i++) {
		
	}

	while (1) {
		//uint16_t pot = GPIOB.IDR;
		//if (pot < 100) {
		//	_putc('1');
		//}
		//else if (pot < 6000) {
		//	_putc('2');
		//}
		//else if(pot < 30000) {
		//	_putc('3');
		//}
		//else if (pot <= 65535) {
		//	_putc('4');
		//}
		LED_on(&GPIOB, 9);
		tempo_500ms();
		tempo_500ms();
		LED_off(&GPIOB, 9);
		tempo_500ms();
		tempo_500ms();
	}

	return 0;
}
