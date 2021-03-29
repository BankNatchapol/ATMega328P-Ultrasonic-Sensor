#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void TIMER0_Init(uint8_t count, uint8_t dim) {
    TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (1 << WGM02) | (1 << CS01) | (1 << CS00);
    OCR0A = count;
    OCR0B = dim;
}

int main(void) {
    
    DDRD |= (1 << DDD5);
    PORTD &= ~(1 << PORTD5);
    
    TIMER0_Init(31, 31);
    int i = 1;
    int j = -1;
    while (1) {
        if(i == 31 || i == 0){
            j *= -1;
        }
        TIMER0_Init(31, 31 - i);
        _delay_ms(50);
        i += 1*j;
    }
}
