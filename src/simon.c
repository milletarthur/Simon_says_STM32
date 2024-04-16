#include: <stdio.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"

uint32_t temps_total = 0;
char buzzer_rouge;
char buzzer_jaune;
char buzzer_vert;
char buzzer_bleu;

void tempo_500ms(){
	tempo_100ms();
	tempo_100ms();
	tempo_100ms();
	tempo_100ms();
	tempo_100ms();
}

void tempo_100ms(){
        volatile uint32_t duree;
        /* estimation, suppose que le compilateur n'optimise pas trop... */
        for (duree = 0; duree < 1120000 ; duree++){
                ;
        }

}

void allumer_led(char led){
	switch (led & 0x3){
		case 0x0:
			allumer led rouge;
			allumer buzzer à la fréquence buzzer_rouge;
			attendre 500ms;
			éteindre led rouge et le buzzer;
		case 0x1:
			allumer led jaune;
			allumer buzzer à la fréquence buzzer_jaune;
			attendre 500ms;
			éteindre led jaune et le buzzer;
		case 0x2:
			allumer led vert;
			allumer buzzer à la fréquence buzzer_vert;
			attendre 500ms;
			éteindre led verte et le buzzer;
		case 0x3:
			allumer led bleu;
			allumer buzzer à la fréquence buzzer_bleu;
			attendre 500ms;
			éteindre led bleu et le buzzer;
		default:
	}
}

void generate_sequence(char* sequence, char counter, char index){
	char rand = temps_total % 4;
	char tmp;
	rand = rand & 0x3;
	switch (index){
		case 0:
			sequence[counter] = rand;
		case 1:
			sequence[counter] |= (rand & 0x3) << 2;
		case 2:
			sequence[counter] |= (rand & 0x3) << 4;
		case 3:
			sequence[counter] |= (rand & 0x3) << 6;
		default:
	}
}

void show_sequence(char* sequence, char counter, char index){
	char led;
	char i;
	for(i=0; i<counter; i++){
		for(int j=0; j<3; j++){
			led = sequence[i] << 2*j;
			allumer_led(led);
		}
	}
	for(int k=0; k<index; k++){
		led = sequence[i] << 2*k;
		allumer_led(led);    
	}
}

//TODO
char proposition_led(){
	uint16_t temps = 0;
	while(BP enfoncé)
		temps += 100;		
	if(temps >= 3000)
		return ...
	if(temps >= 2000)
		return ...
	if(temps >= 1000)
		return ...
	return ...
}

void recuperation_proposition(char* proposition, char counter, char index){
	char tour = 4*counter+index;
	char led;
	char decalage;
	for(int i=0; i<tour; i++){
		led = proposition_led();
		// A CORRIGER en utilisant le décalage
		proposition[i] = led;
	}
}


// 1 : erreur et 0 : pas d'erreur
char verif(char* sequence, char counter, char index, char* proposition){
	for(int i=0; i<counter; i++){
		if(sequence[i] != proposition[i])
			return 1;
	}
	return 0;
}

void victoire(){
	//TODO
	faire clignoter pendant 2 secondes toutes les LEDs
		musique victoire avec buzzer
}

void traitant_interruption(){
	temps_total += 0.3;
}

int main(){
	init_systick();
	init_button();
	init_LEDs();
	init_buzzer();
	char sequence[32];                         	// 1 char = 1 octet = 8 bits = 4 tours
	char counter;                               	// 0 <= counter <= 31
	char index;                                    	// 0 <= index <= 3
	char debut_partie = 0;                 		// 0 : attente début partie et 1 : partie en cours
	char erreur = 0;                             	// 0 : pas d'erreur et 1 : erreur détectée
	char proposition[32];                    	// séquence proposé par le joueur
	while(1){
		while(!debut_partie){               	// attente du début de partie
			if(BP enfoncé)
				debut_partie = 1;
		}
		if(counter == 31 et index == 3){       	// le joueur a remporté la partie
			victoire();
			debut_partie = 0;
			counter = 0;
			index = 0;
		}

		generate_sequence(sequence, counter, index);             	// génération aléatoire de la séquence
		show_sequence(sequence, counter, index);                   	// éclaire les LEDs décrivant la séquence
		recuperation_proposition(proposition, counter, index);          // séquence proposée par le joueur
		erreur = verif(sequence, counter, index, proposition);    	// vérification de la séquence du joueur

		if(erreur == 1){                				// le joueur s'est trompé
			debut_partie = 0;
			counter = 0;
			index = 0;
		} else {                                			// le joueur a remporté le tour
			index++;
			if(index == 4){
				index = 0;
				counter++;
			}
		}
	}
}
