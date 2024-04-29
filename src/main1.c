#include "sys/clock.h"
#include "sys/devices.h"
#include "sys/init.h"

void init_PB(){
	enable_GPIOC();
	GPIOC.MODER &= ~(0x3<<26);
	GPIOC.PUPDR &= ~(0x3<<26);
}

void init_LD2(){
	/* on positionne ce qu'il faut dans les différents
	   registres concernés */
	RCC.AHB1ENR |= 0x01;
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFF3FF) | 0x00000400;
	GPIOA.OTYPER &= ~(0x1<<5);
	GPIOA.OSPEEDR |= 0x03<<10;
	GPIOA.PUPDR &= 0xFFFFF3FF;
}

void tempo_500ms(){
	volatile uint32_t duree;
	/* estimation, suppose que le compilateur n'optimise pas trop... */
	for (duree = 0; duree < 5600000 ; duree++){
		;
	}
}

/* Initialisation du timer système (systick) */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
void __attribute__((interrupt)) SysTick_Handler(){
	/* Le fait de définir cette fonction suffit pour
	 * qu'elle soit utilisée comme traitant,
	 * cf les fichiers de compilation et d'édition de lien
	 * pour plus de détails.
	 */
	/* ... */
	compteur ++;
	if (compteur >= tempo) {
		if((GPIOA.IDR & (0x1 << 5)) == 0)
			GPIOA.ODR |= (0x1 << 5);
		else
			GPIOA.ODR &= (~(0x1 <<5));
		compteur = 0;
	}
}
#pragma GCC diagnostic pop

void tempo_2s() {
	tempo_500ms();
	tempo_500ms();
	tempo_500ms();
	tempo_500ms();
}

int main() {
	init_PB();
	init_LD2();
	while(1) {
		if((GPIOC.IDR & (0x1 << 13)) == 0)
			GPIOA.ODR |= (0x1 <<5);
		else {
			GPIOA.ODR |= (0x1 <<5);
			tempo_2s();
			GPIOA.ODR &= (~(0x1 << 5));
			tempo_2s();
		}
	}
}
