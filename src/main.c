#include <string.h>
#include <stdlib.h>
#include "sys/devices.h"
#include "sys/init.h"
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

void _putc(char c) {
	while ((USART2.SR & 0x80) == 0) ;
	USART2.DR = c;
}

void _puts(const char *c){
	uint8_t len = strlen(c);
	for (int i=0;i<len;i++){
		_putc(c[i]);
	}
}

char _getc(){
	while( (USART2.SR & (0x1 << 5)) == 0);
	return USART2.DR;
}

void _gets(char* dst) {
	uint8_t len = strlen(dst);
	if (len <= 0) return;
	uint8_t i = 0;
	char c;
	do {
		c = _getc();
		dst[i]= c;
		i++;
	} while (i < len || c != '\r');
	if (i >= len) dst[len-1] = '\0';
	else dst[i] = '\0';
}

uint32_t mesure_potentiometre(){
	ADC1.CR2 |= (1<<30);
	while( !(ADC1.SR&(1<<1)) );
	return ADC1.DR;
}

void generate_sequence(char* sequence, uint8_t counter, char index){
    char rand = (temps_total / 100) % 4;
    switch (index){
        case 0:
            // LED_on(&GPIOA,0);
			sequence[counter] = rand;
			break;
        case 1:
			// LED_on(&GPIOA,1);
			sequence[counter] |= (rand & 0x3) << 2;
			break;
        case 2:
			// LED_on(&GPIOB,10);
            sequence[counter] |= (rand & 0x3) << 4;
			break;
        case 3:
			// LED_on(&GPIOC,7);
            sequence[counter] |= (rand & 0x3) << 6;
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

void show_sequence(char* sequence, uint8_t counter, char index){
    char led;
	char tmp;
	char c1, c2, c3;
    uint8_t i;
    for(i=0; i<counter; i++){
		if(i == counter-1){
			c1 = (sequence[i] >> 0x6) & 0x3;
			c2 = (sequence[i] >> 0x4) & 0x3;
			c3 = (sequence[i] >> 0x2) & 0x3;
			if(index == 1)
				allumer_led(c1);
			if(index == 2){
				allumer_led(c1);
				allumer_led(c2);
			}
			if(index == 3){
				allumer_led(c1);
				allumer_led(c2);
				allumer_led(c3);
			}
			break;
		}
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
	// if(index == 1){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// }
	// if(index == 2){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// 	allumer_led((sequence[i] >> 4) & 0x3);
	// }
	// if(index == 3){
	// 	allumer_led((sequence[i] >> 6) & 0x3);
	// 	allumer_led((sequence[i] >> 4) & 0x3);
	// 	allumer_led((sequence[i] >> 2) & 0x3);
	// }
}

char verif(char* sequence, uint8_t counter, char* proposition){
	for(uint8_t i=0; i<counter; i++){
		if(sequence[i] != proposition[i])
			return 1;
	}
	return 0;
}

char proposition_led(){
	char pot;
	char led;
	while(BP pas enfoncé){
		pot = mesure_potentiometre();
		if (pot < 1024) {
			LED_on(&GPIOA,0);
			led = 0;
			// _putc('0');
		}
		else if (pot < 2048) {
			LED_on(&GPIOA,1);
			led = 1;
			// _putc('1');
		}
		else if(pot < 3072) {
			LED_on(&GPIOB,10);
			led = 2;
			// _putc('2');
		}
		else if (pot <= 4096) {
			LED_on(&GPIOC,7);
			led = 3;
			// _putc('3');
		}
	}
	return led;
}

void recup_proposition(char* proposition, uint8_t counter){
	for(uint8_t i=0; i<counter; i++){
		proposition[i] = proposition_led();
	}
}

uint32_t difficulte() {
	_puts("Choisissez la difficulté de la partie (nombre entier positif) :\r\n");
	char diff[5];
	_gets(diff);
	return strtol(diff,NULL,0);
}

int main() {
	initialisation(300);
	char debut = 0;
	char erreur;
	char sequence[100];
	char proposition[100];
	uint8_t counter = 0;
	// char index = 0;

	while (1) {
		// en attente du début de partie
		while(!debut){
			if(BP enfoncé){
				debut = 1;
			}
		}
		// victoire du joueur, il a complété tous les niveaux
		if(counter == 1){
			victoire();
			debut = 0;
			counter = 0;
		}

		generate_sequence(sequence,counter);
		show_sequence(sequence, counter);
		recup_proposition(proposition,counter);
		erreur = verif(sequence, counter, proposition);

		// le joueur s'est trompé dans sa proposition, la partie s'arrête
		if(erreur){
			debut = 0;
			counter = 0;
		// le joueur a mémorisé la bonne proposition, on passe donc au tour suivant
		} else {
			counter++;
		}

	}

	return 0;
}
