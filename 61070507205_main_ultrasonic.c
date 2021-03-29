#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

void USART_Init(unsigned int ubrr) { 
    /* Set baud rate */
    UBRR0 = ubrr;
    /* Enable receiver and transmitter */
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data*/ 
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); 
}

void USART_Transmit( unsigned char data ) { 
    /* Wait for empty transmit buffer */ 
    while ( !( UCSR0A & (1 << UDRE0)) );
    /* Put data into buffer, sends the data */ 
    UDR0 = data; 
}

void TIMER1_Init() {
    TCCR1B |= (1 << ICNC1) | (1 << CS11);
}

int main(void) {
    
    //set trig port to output, low
    DDRD |= (1<<DDD7); 
    PORTD &=  ~(1<<PORTD7);
            
    USART_Init(51);
    TIMER1_Init();
    char output_string[20] = {};
    char str[32] = "Temperature is ", temp_char[32];
    while (1) {
        //phase 1: start
        //trig high
        PORTD |=  (1<<PORTD7);
        //delay 10 us
        _delay_us(10);
        
        //trig low
        PORTD &=  ~(1<<PORTD7);
        
        //phase 2: capture wave rising
        //set to capture rising edge
        TCCR1B |= (1<<ICES1);
                
        //check ICF1 high for capture event
        while ( !( TIFR1 & (1 << ICF1)));
        
        //set TCNT1 to 0
        TCNT1 = 0;
        
        //reset ICF1 flag
        TIFR1 |= (1 << ICF1);
        
        //phase 3: capture wave falling
        //set to capture falling edge
        TCCR1B &= ~(1<<ICES1);
        //check ICF1 high for capture event
        while ( !( TIFR1 & (1 << ICF1)));
        //copy 16-bit data out from ICR1
        uint16_t output_data = ICR1;
        //reset ICF1 flag
        TIFR1 |= (1 << ICF1);
        
        //phase 4: convert and output
        //convert
        uint16_t distance = ((output_data / 1000000.0) * 340 * 100) / 2.0;
        
        //output
        sprintf(temp_char, "distance is %d \n", distance);
        for(int i = 0; i < strlen(temp_char); ++i){
            USART_Transmit(temp_char[i]);
        }
        _delay_ms(1000);
    }
}