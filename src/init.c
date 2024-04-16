#include "sys/devices.h"
void init_systick() {
	//TODO
}

void init_button() {
	//TODO
}

void init_LED(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->MODER = (GPIOX->MODER & (~(0x3 << 2*port))) | (0x1 << 2*port);
	GPIOX->OTYPER &= ~(0x1 << port);
	GPIOX->OSPEEDR |= 0x3 << 2*port;
}

void LED_on(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR |= 0x1 << port;
}

void init_buzzer() {
	//TODO
}

void initialisation() {
	enable_GPIOA();
	enable_GPIOB();
	enable_GPIOC();
	init_systick();
	init_button();

	// LED red = PA0
	init_LED(&GPIOA,0);
	// LED yellow = PA1
	init_LED(&GPIOA,1);
	// LED green = PB10
	init_LED(&GPIOB,10);
	// LED blue = PC7
	init_LED(&GPIOC,7);

	init_buzzer();
}

int main() {
	initialisation();
	LED_on(&GPIOA,0);
	LED_on(&GPIOA,1);
	LED_on(&GPIOB,10);
	LED_on(&GPIOC,7);
	return 0;
}
