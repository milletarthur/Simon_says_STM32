#include "sys/devices.h"
#include <stdint.h>
#include "sys/clock.h"

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

void init_systick(uint32_t freq) {
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

void init_input(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->MODER &= ~(0x3 << 2*port);
	GPIOX->PUPDR &= ~(0x3 << 2*port);
}

void init_output(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->MODER = (GPIOX->MODER & (~(0x3 << (2*port)))) | (0x1 << (2*port));
	GPIOX->OTYPER &= ~(0x1 << port);
	GPIOX->OSPEEDR |= 0x3 << (2*port);
}

void init_potentiometre(){
	RCC.AHB1ENR |= (1<<1);
	RCC.APB2ENR |= (1<<8);
	GPIOB.MODER |= (3<<0);

	ADC1.CR2 |= (1<<0);
	ADC1.CR1 &= ~(3<<24);
	ADC1.SMPR2 |= (7<<24);
	ADC1.SQR1 = 0;
	ADC2.SQR2 = 0;
	ADC1.SQR3 = (8<<0);
}

void LED_on(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR |= (0x1 << port);
}

void LED_off(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR &= ~(0x1 << port);
}

void initialisation(uint32_t freq) {
	enable_GPIOA();
	enable_GPIOB();
	enable_GPIOC();
	init_systick(freq);

	/****** OUTPUT ******/

	// LED red = PA0
	init_output(&GPIOA,0);
	// LED yellow = PA1
	init_output(&GPIOA,1);
	// LED green = PB10
	init_output(&GPIOB,10);
	// LED blue = PC7
	init_output(&GPIOC,7);

	//Buzzer
	init_output(&GPIOB, 9);

	/****** INPUT ********/

	// Bouton poussoir
	init_input(&GPIOB, 8);
	// Potentiomètre
	init_potentiometre();	
}

void victoire(){
	for(uint8_t i=0; i<5; i++){
		LED_on(&GPIOA,0);
		LED_on(&GPIOA,1);
		LED_on(&GPIOB,10);
		LED_on(&GPIOC,7);
		tempo_500ms();
		LED_off(&GPIOA,0);
		LED_off(&GPIOA,1);
		LED_off(&GPIOB,10);
		LED_off(&GPIOC,7);
		tempo_500ms();
	}
}