#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"

static volatile char c=0;

void init_LD2(){
	/* on positionne ce qu'il faut dans les différents
	   registres concernés */
	RCC.AHB1ENR |= 0x01;
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFF3FF) | 0x00000400;
	GPIOA.OTYPER &= ~(0x1<<5);
	GPIOA.OSPEEDR |= 0x03<<10;
	GPIOA.PUPDR &= 0xFFFFF3FF;
}

void init_PB(){
	enable_GPIOC();
	GPIOC.MODER &= ~(0x3<<26);
	GPIOC.PUPDR &= ~(0x3<<26);
}

void tempo_500ms(){
	volatile uint32_t duree;
	/* estimation, suppose que le compilateur n'optimise pas trop... */
	for (duree = 0; duree < 5600000 ; duree++){
		;
	}

}

void init_USART(){
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFFF0F) | 0x000000A0;
	GPIOA.AFRL = (GPIOA.AFRL & 0xFFFF00FF) | 0x00007700;
	USART2.BRR = get_APB1CLK()/9600;
	USART2.CR3 = 0;
	USART2.CR2 = 0;
}

void _putc(const char c){
	while( (USART2.SR & 0x80) == 0);  
	USART2.DR = c;
}

void _puts(const char *c){
	int len = strlen(c);
	for (int i=0;i<len;i++){
		_putc(c[i]);
	}
}

char _getc(){
	while( (USART2.SR & (0x1 << 5)) == 0);
	return USART2.DR;
}

/* Initialisation du timer système (systick) */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

void __attribute__((interrupt)) SysTick_Handler(){
	/* Le fait de définir cette fonction suffit pour
	 * qu'elle soit utilisée comme traitant,
	 * cf les fichiers de compilation et d'édition de lien
	 * pour plus de détails.
	 */
	/* ... */
        GPIOA.ODR &= (~(0x1 << 5));
        tempo_500ms();
        GPIOA.ODR |= (0x1 <<5);
        tempo_500ms();
}

/* Fonction non bloquante envoyant une chaîne par l'UART */
int _async_puts(const char* s) {
	/* Cette fonction doit utiliser un traitant d'interruption
	 * pour gérer l'envoi de la chaîne s (qui doit rester
	 * valide pendant tout l'envoi). Elle doit donc être
	 * non bloquante (pas d'attente active non plus) et
	 * renvoyer 0.
	 *
	 * Si une chaîne est déjà en cours d'envoi, cette
	 * fonction doit renvoyer 1 (et ignorer la nouvelle
	 * chaîne).
	 *
	 * Si s est NULL, le code de retour permet de savoir
	 * si une chaîne est encore en cours d'envoi ou si
	 * une nouvelle chaîne peut être envoyée.
	 */
	/* À compléter */
	return 0;
}

void tempo_2s() {
	tempo_500ms();
	tempo_500ms();
	tempo_500ms();
	tempo_500ms();
}

int main() {
	init_PB();
        init_LD2();
	systick_init(500);
	while(1) {
	
	}
	return 0;
}

