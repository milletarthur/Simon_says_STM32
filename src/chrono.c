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
	/* GPIOC.MODER = ... */
}

/* Ne pas optimiser cette fonction : on dépend du nombre
 * approximatif d'instructions qu'elle exécute
 */
__attribute__((optimize("O0")))
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
	/* À compléter */
	return 0;
}

/* Initialisation du timer système (systick) */
/* La fréquence en entrée est en Hz.
 * Elle doit être >= 11 du fait de la taille du registre LOAD */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	/* ATTENTION: la valeur calculée doit tenir dans le registre sur 24 bits */
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

/* Cet attibut n'est plus nécessaire sur l'architecture ARM-Cortex-M4:
 * Les fonctions d'interruption respectent les conventions d'appel
 * de la plateforme.
 * Voir par exemple le paragraphe "Exception Entry & Exit" de
 * https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic
 */
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wattributes"
// __attribute__((interrupt))
void SysTick_Handler(){
// #pragma GCC diagnostic pop
	/* Le fait de définir cette fonction suffit pour
	 * qu'elle soit utilisée comme traitant,
	 * cf les fichiers de compilation et d'édition de lien
	 * pour plus de détails.
	 */
	/* ... */
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

int main() {
  
	//printf("\e[2J\e[1;1H\r\n");
	//printf("\e[01;32m*** Welcome to Nucleo F446 ! ***\e[00m\r\n");

	//printf("\e[01;31m\t%08lx-%08lx-%08lx\e[00m\r\n",
	//       U_ID[0],U_ID[1],U_ID[2]);
	//printf("SYSCLK = %9lu Hz\r\n",get_SYSCLK());
	//printf("AHBCLK = %9lu Hz\r\n",get_AHBCLK());
	//printf("APB1CLK= %9lu Hz\r\n",get_APB1CLK());
	//printf("APB2CLK= %9lu Hz\r\n",get_APB2CLK());
	//printf("\r\n");
	
	int h1 = 0;
	int h2 = 0;
	int m1 = 0;
	int m2 = 0;
	int s1 = 0;
	int s2 = 0;
	while (1){
		printf("\r%d%d:%d%d:%d%d",h2,h1,m2,m1,s2,s1);
		tempo_500ms();
		tempo_500ms();
		s1++;
		if(s1 == 10){
			s1 = 0;
			s2++;
		}
		if(s2 == 6){
			s2 = 0;
			m1++;
		}
		if(m1 == 10){
			m1 = 0;
			m2++;
		}
		if(m2 == 6){
			m2 = 0;
			h1++;
		}
		if(h1 == 10){
			h1 = 0;
			h2++;
		}
		if(h1 == 4 && h2 == 2){
			h1 = 0;
			h2 = 0;
		}
	}
	return 0;
}

