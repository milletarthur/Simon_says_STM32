#include "sys/devices.h"
void init_systick() {
	//TODO
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

void LED_on(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR |= (0x1 << port);
}

void LED_off(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR &= ~(0x1 << port);
}

void initialisation() {
	enable_GPIOA();
	enable_GPIOB();
	enable_GPIOC();
	init_systick();

	/****** OUTPUT ******/

	// LED red = PA0
	init_output(&GPIOA,4);
	// LED yellow = PA1
	init_output(&GPIOA,5);
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
	init_input(&GPIOB, 0);
}

int main() {
	initialisation();
	LED_off(&GPIOA,4);
	LED_on(&GPIOA,5);
	//LED_on(&GPIOB,10);
	//LED_on(&GPIOC,7);
	//GPIOA.ODR = 0;
	while(1) ;
	return 0;
}
