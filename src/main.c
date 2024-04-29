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
	for (uint8_t i=0;i<len;i++){
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
		_putc(c);
		dst[i]= c;
		i++;
	} while (i < len || c != '\r');
	_putc('\n');
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

uint8_t difficulte() {
	_puts("Choisissez la difficulté de la partie (nombre entier positif) :\r\n");
	char diff[4];
	_gets(diff);
	return strtol(diff,NULL,0);
}

int main() {
	initialisation(300);
	uint8_t len = difficulte();
	char debut = 0;
	char erreur;
	char sequence[len];
	char proposition[len];
	uint8_t counter = 1;
	uint16_t pot;
	char led = -1;
	uint8_t j=0;

	while (1) {

		// initialisation des tableaux de char
		for(uint8_t i = 0; i<len; i++){
			sequence[i] = -1;
			proposition[i] = -1;
		}
		
		sequence[0] = 0;
		sequence[1] = 1;
		sequence[2] = 2;
		sequence[3] = 3;
		sequence[4] = 0;
		sequence[5] = 1;
		sequence[6] = 2;
		sequence[7] = 3;

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

		while(counter <= len){
			LED_off(&GPIOA,0);
			LED_off(&GPIOA,1);
			LED_off(&GPIOB,10);
			LED_off(&GPIOC,7);
			show_sequence(sequence, counter);
			j=0;

			while(j<=counter-1){
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
			}
			erreur = verif(sequence, counter-1, proposition);
			if(erreur){
				_puts("\r\n\raté !\r\n");
				debut = 0;
			} else {
				counter++;
				_puts("\r\n");
			}
		}
		tempo_500ms();
		victoire();
		debut = 0;
	}

	return 1;
}	
