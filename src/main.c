#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include "sys/cm4.h"
#include <stdint.h>

static volatile uint32_t temps_total = 0;

void systick_init(uint32_t freq) {
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
void SysTick_Handler(){
	temps_total+=300;
}
#pragma GCC diagnostic pop

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

void LED_off(volatile struct GPIO_registers* GPIOX, uint32_t port) {
	GPIOX->ODR &= ~(0x1 << port);
}

void init_buzzer() {
	//TODO
}

void initialisation() {
	enable_GPIOA();
	enable_GPIOB();
	enable_GPIOC();
	init_button();

	systick_init(300);

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
	systick_init(300);
	LED_on(&GPIOA,0);
	LED_on(&GPIOA,1);
	LED_on(&GPIOB,10);
	LED_on(&GPIOC,7);
	return 0;
}
