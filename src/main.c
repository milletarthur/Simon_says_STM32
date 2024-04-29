#include <string.h>
#include <stdlib.h>
#include "sys/devices.h"
#include "sys/init.h"
#include <stdint.h>
#include <string.h>
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

void _puts(char *c){
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

void allumer_led(char led){
    switch (led){
        case 0:
			_putc('0');
            LED_on(&GPIOA,0);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOA,0);
			break;
        case 1:
			_putc('1');
            LED_on(&GPIOA,1);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOA,1);
			break;
        case 2:
			_putc('2');
            LED_on(&GPIOB,10);
            tempo_500ms();
			tempo_500ms();
			LED_off(&GPIOB,10);
			break;
        case 3:
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

void show_sequence(char* sequence, uint8_t counter){
    for(uint8_t i=0; i<counter; i++){
		allumer_led(sequence[i]);
		tempo_250ms();
	}
}

char verif(char* sequence, uint8_t counter, char* proposition){
	for(uint8_t i=0; i<=counter; i++){
		if(sequence[i] != proposition[i])
			return 1;
	}
	return 0;
}

void generate_sequence(char* sequence, uint8_t counter){
    char rand = (temps_total / 100) % 4;
    sequence[counter] = rand;
	allumer_led(rand);
	LED_off(&GPIOA,0);
	LED_off(&GPIOA,1);
	LED_off(&GPIOB,10);
	LED_off(&GPIOC,7);
}

char proposition_led(){
	char pot;
	char led = 0;
	while ((GPIOB.IDR & (0x1 << 8)) != 0){
		pot = mesure_potentiometre();
		// LED_off(&GPIOA,0);
		// LED_off(&GPIOA,1);
		// LED_off(&GPIOB,10);
		// LED_off(&GPIOC,7);
		if (pot < 1024) {
			LED_on(&GPIOA,0);
			led = 0;
			_putc('A');
		}
		else if (pot < 2048) {
			LED_on(&GPIOA,1);
			led = 1;
			_putc('B');
		}
		else if(pot < 3072) {
			LED_on(&GPIOB,10);
			led = 2;
			_putc('C');
		}
		else if (pot <= 4096) {
			LED_on(&GPIOC,7);
			led = 3;
			_putc('D');
		}
		tempo_250ms();
	}
	return led;
}

void recup_proposition(char* proposition, uint8_t counter){
	for(uint8_t i=0; i<=counter; i++){
		proposition[i] = proposition_led();
		tempo_100ms();
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
	uint16_t len = 5;
	char debut = 0;
	char erreur;
	char sequence[len];
	char proposition[len];
	uint8_t counter = 2;
	uint16_t pot;
	char led = -1;

	// while (1) {
	// 	pot = mesure_potentiometre();

	// 	LED_off(&GPIOA,0);
	// 	LED_off(&GPIOA,1);
	// 	LED_off(&GPIOB,10);
	// 	LED_off(&GPIOC,7);
	// 	if (pot < 1024) {
	// 		LED_on(&GPIOA,0);
	// 		_putc('1');
	// 	}
	// 	else if (pot < 2048) {
	// 		LED_on(&GPIOA,1);
	// 		_putc('2');
	// 	}
	// 	else if(pot < 3072) {
	// 		LED_on(&GPIOB,10);
	// 		_putc('3');
	// 	}
	// 	else if (pot <= 4096) {
	// 		LED_on(&GPIOC,7);
	// 		_putc('4');
	// 	}
	// 	// allumer / éteindre buzzer
	// 	// LED_on(&GPIOB, 9);
	// 	// tempo_500ms();
	// 	// tempo_500ms();
	// 	// LED_off(&GPIOB, 9);
	// 	// tempo_500ms();
	// 	// tempo_500ms();
	// }

	// initialisation des tableaux de char
	for(uint16_t i = 0; i<len; i++){
		sequence[i] = -1;
		proposition[i] = -1;
	}

	sequence[0] = 3;
	sequence[1] = 1;
	sequence[2] = 2;
	sequence[3] = 0;
	sequence[4] = 3;

	proposition[0] = 3;
	proposition[1] = 1;

	while (1) {
		// en attente du début de partie
		while(!debut){
			// si le bouton poussoir USER de la carte fille est enfoncé on commence la partie
			if((GPIOB.IDR & (0x1 << 8)) == 0){
				LED_off(&GPIOA,0);
				LED_off(&GPIOA,1);
				LED_off(&GPIOB,10);
				LED_off(&GPIOC,7);
				debut = 1;
				break;
			} else {
				LED_on(&GPIOA,0);
				tempo_250ms();
				LED_off(&GPIOA,0);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOB,10);
				tempo_250ms();
				LED_off(&GPIOB,10);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOC,7);
				tempo_250ms();
				LED_off(&GPIOC,7);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOA,1);
				tempo_250ms();
				LED_off(&GPIOA,1);
			}
		}
		// victoire du joueur, il a complété tous les niveaux
		if(counter == len){
			victoire();
			debut = 0;
			counter = 0;
		}

		// generate_sequence(sequence,counter);
		show_sequence(sequence, counter);
		tempo_500ms();
		// recup_proposition(proposition,counter);
		for(uint8_t i=0; i<=5; i++){
			LED_off(&GPIOA,0);
			LED_off(&GPIOA,1);
			LED_off(&GPIOB,10);
			LED_off(&GPIOC,7);
			while ((GPIOB.IDR & (0x1 << 8)) != 0){
				pot = mesure_potentiometre();
				LED_off(&GPIOA,0);
				LED_off(&GPIOA,1);
				LED_off(&GPIOB,10);
				LED_off(&GPIOC,7);
				if (pot < 1024) {
					LED_on(&GPIOA,0);
					led = 0;
				}
				else if (pot < 2048) {
					LED_on(&GPIOA,1);
					led = 1;
				}
				else if(pot < 3072) {
					LED_on(&GPIOB,10);
					led = 2;
				}
				else if (pot <= 4096) {
					LED_on(&GPIOC,7);
					led = 3;
				}
				tempo_250ms();
			}
			_putc(led);
			proposition[i] = led;
		}

		// victoire();
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

*/

int main() {
	initialisation(300);
	uint16_t len = 4;
	char debut = 0;
	char erreur;
	char sequence[len];
	char proposition[len];
	uint8_t counter = 1;
	uint16_t pot;
	char led = -1;
	uint16_t j=0;


	while (1) {

		// initialisation des tableaux de char
		for(uint16_t i = 0; i<len; i++){
			sequence[i] = -1;
			proposition[i] = -1;
		}
		j=0;
		
		sequence[0] = 0;
		sequence[1] = 1;
		sequence[2] = 2;
		sequence[3] = 3;

		// en attente du début de partie
		while(!debut){
			// si le bouton poussoir USER de la carte fille est enfoncé on commence la partie
			if((GPIOB.IDR & (0x1 << 8)) == 0){
				LED_off(&GPIOA,0);
				LED_off(&GPIOA,1);
				LED_off(&GPIOB,10);
				LED_off(&GPIOC,7);
				debut = 1;
				break;
			} else {
				LED_on(&GPIOA,0);
				tempo_250ms();
				LED_off(&GPIOA,0);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOB,10);
				tempo_250ms();
				LED_off(&GPIOB,10);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOC,7);
				tempo_250ms();
				LED_off(&GPIOC,7);
				if((GPIOB.IDR & (0x1 << 8)) == 0){
					debut = 1;
					break;
				}
				LED_on(&GPIOA,1);
				tempo_250ms();
				LED_off(&GPIOA,1);
			}
		}

		show_sequence(sequence, counter);

		while(j<=counter){
			// récupération de la proposition
			while ((GPIOB.IDR & (0x1 << 8)) != 0){
				pot = mesure_potentiometre();
				LED_off(&GPIOA,0);
				LED_off(&GPIOA,1);
				LED_off(&GPIOB,10);
				LED_off(&GPIOC,7);
				if (pot < 1024) {
					LED_on(&GPIOA,0);
					led = 0;
				}
				else if (pot < 2048) {
					LED_on(&GPIOA,1);
					led = 1;
				}
				else if(pot < 3072) {
					LED_on(&GPIOB,10);
					led = 2;
				}
				else if (pot <= 4096) {
					LED_on(&GPIOC,7);
					led = 3;
				}
			}
			switch(led){
				case 0:
					_putc('0');
					break;
				case 1:
					_putc('1');
					break;
				case 2:
					_putc('2');
					break;
				case 3:
					_putc('3');
					break;
				default:
					break;
			}
			proposition[j] = led;
			tempo_250ms();
			j++;
			if(j == len)
				break;
			counter++;
		}
		erreur = verif(sequence, counter, proposition);
		if(erreur){
			_puts("\r\nFIN");
			return 0;
		}
	}
	
	victoire();
	tempo_500ms();
	tempo_500ms();
	show_sequence(sequence, len);

	return 1;
}	
