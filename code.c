#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdio.h>
#include<util/delay.h>
#define SET_BIT(PORT,BIT) PORT |= (1<<BIT)
#define CLR_BIT(PORT,BIT) PORT &=~(1<<BIT)
int pwm_int();
int pwm();
int b_speed,n_speed;
volatile int adc_value,n_adc_value;
int sw1,sw2,sw3,sw4;
int main()
{
  SET_BIT(DDRB,2);
  SET_BIT(DDRB,3);
  SET_BIT(DDRB,4);
  SET_BIT(DDRB,5);
  SET_BIT(DDRD,7);
  SET_BIT(DDRD,5);
  
  CLR_BIT(DDRD,PD3); // int1
  SET_BIT(PORTD,PD3);

  CLR_BIT(DDRD,PD2); //int0
  SET_BIT(PORTD,PD2);
  
  CLR_BIT(DDRD,PD1); //EXT2
  SET_BIT(PORTD,PD1);
  
  CLR_BIT(DDRB,PB1); //PCINT 1
  SET_BIT(PORTB,PB1);

  EICRA |=(1<<ISC10);
  EICRA |=(1<<ISC00);

  EIMSK |=((1<<INT1)|(1<<INT0));
  SREG |=(1<<7);

  PCICR |= ((1<<PCIE2)|(1<<PCIE0));
  PCMSK2 |= (1<<PCINT17); //17
  PCMSK0 |= (1<<PCINT1); //1
  SREG |=(1<<7);


  pwm_int();
 //n_pwm_int();
  while(1)
  {
    while(sw4==1)
    {
      if(sw1==1) // lean mode
      {
          b_speed=50;//25%
          pwm();
          SET_BIT(PORTB,2);
          _delay_ms(14200);
          SET_BIT(PORTB,PD3);
          _delay_ms(500);
          CLR_BIT(PORTB,2);
          CLR_BIT(PORTB,3);
          _delay_ms(1000);
      }
      else if(sw2==1) //rich mode
      {
          b_speed=250;//95%
          pwm();
          SET_BIT(PORTB,2);
          _delay_ms(12700);
          SET_BIT(PORTB,3);
          _delay_ms(2000);
          CLR_BIT(PORTB,2);
          CLR_BIT(PORTB,3);
          _delay_ms(1000);
      }
      else //normal mode
      {
          b_speed=128;
          pwm();
          SET_BIT(PORTB,2);
          _delay_ms(13700);
          SET_BIT(PORTB,3);
          _delay_ms(2000);
          CLR_BIT(PORTB,2);
          CLR_BIT(PORTB,3);
          _delay_ms(1000);
      }
    
    //Arun
    
    if(sw3==1)
       {
           CLR_BIT(PORTB,4);//motor off
           SET_BIT(PORTB,5);//led on        
       }
       else
       {
           CLR_BIT(PORTB,5);//led off
           SET_BIT(PORTB,4);//motor on
       }
    
    
    //ATG
     adc();
     if(adc_value<=500)
     {
       SET_BIT(PORTD,7); //ele
       CLR_BIT(PORTD,5);//gas
     }
    else
    {
      SET_BIT(PORTD,5);//gas
      CLR_BIT(PORTD,7);
    }
    
     //Nishanth   
     n_adc();
    if(n_adc_value <= 100)
    {
      n_speed=240;
      n_pwm();
    }
    else if(n_adc_value<=200)
    {
      n_speed=150;
      n_pwm();
    }
    else
    {
      n_speed=80;
      n_pwm();
    }
  
    
     }
    CLR_BIT(PORTB,2);
    CLR_BIT(PORTB,3);
    CLR_BIT(PORTB,4);
    CLR_BIT(PORTB,5);
    CLR_BIT(PORTD,7);
    CLR_BIT(PORTD,5);
    CLR_BIT(PORTC,0);
    CLR_BIT(PORTC,1);
    CLR_BIT(PORTC,3);
  }
    
 }

ISR(INT1_vect) //toggle the switch1
 {
    if(sw1==1)
    {
      sw1=0;
    }
    else
    {
       sw1=1;
    }
 }
ISR(INT0_vect) //toggle the switch2
{
  if(sw2==1)
  {
     sw2=0;
  }
  else
  {
     sw2=1;
  }
}
ISR(PCINT2_vect)
{
    if(sw3==1)
    {
        sw3=0;
    }
    else
    {
      sw3=1;
    }
}


ISR(PCINT0_vect)
{
    if(sw4==1)
    {
       sw4=0;
    }
    else
    {
      sw4=1;
    }
}

int pwm_int()
{
    SET_BIT(DDRC,PC1);
    TCCR0A |=(1<<WGM01)|(1<<WGM00); //FAST PWN
    TCCR0A |= (1<<COM0A1);
    TCNT0=0X00;
    sei();
    return 0;
}
int n_pwm_int()
{
 SET_BIT(DDRC,PC3);
 TCCR2A |=((1<<WGM21)|(1<<WGM20)); //FAST PWN
 TCCR2B |= ((1<<WGM22));
 TCCR2A |= (1<<COM2A1);
 TCNT2=0X00;
 sei();
 return 0;
}
int pwm()
{
    OCR0A=b_speed;
    TCCR0B |=(1<<CS00)|(1<<CS02);
    TCCR0B &= (~(1<<CS01));
    TIMSK0|=((1<<OCIE0A)|(1<<OCIE0B));
 }
int n_pwm()
{
    OCR2A=n_speed;
    TCCR2B |=(1<<CS20)|(1<<CS22)|(1<<CS21);
   // TCCR2B &= (~(1<<CS21));
    TIMSK2|=((1<<OCIE2A)|(1<<OCIE2B));
 }


ISR(TIMER0_COMPA_vect)
{
  SET_BIT(PORTC,1);
}
ISR(TIMER0_COMPB_vect)
{
  CLR_BIT(PORTC,1);
}

ISR(TIMER2_COMPA_vect)
{
 SET_BIT(PORTC,3);
}
ISR(TIMER2_COMPB_vect)
{
 CLR_BIT(PORTC,3);
}


int adc()
{
  ADMUX &=0X00;
  ADMUX |=(1<<REFS0);
  ADCSRA |=(1<<ADEN);
  ADCSRA |=(1<<ADSC);
  while(ADCSRA & (1<<ADSC));//wait upto conversation complete
  adc_value=ADC; //is a macro is has dig output
  return 0;
}

int n_adc()
{
  ADMUX &=0X00;
  ADMUX |=(1<<MUX1);
  ADMUX |=(1<<REFS0);
  ADCSRA |=(1<<ADEN);
  ADCSRA |=(1<<ADSC);
  ADC=0;
  while(ADCSRA & (1<<ADSC));//wait upto conversation complete
  n_adc_value=ADC; //is a macro is has dig output  
  return 0;
}


