
#include <stdint.h>
#include <avr/io.h>
#include "adc.h"

// initialize adc
void adc_init()
{
    // AREF = AVcc
    ADMUX = (1<<REFS0);
  
    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    //ADCSRA |= (1 << ADFR);
}

uint16_t analogRead(uint8_t ch)
{
	uint16_t val = 0;
	ch     &= 0b00000111;
	ADMUX   = (ADMUX & 0xF8) | ch;
	ADCSRA |= (1<<ADSC);
	while( ADCSRA & (1 << ADSC) );
	val = ADC;
	return val;
}