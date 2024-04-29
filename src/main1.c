#include "sys/devices.h"
#include "sys/init.h"

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
