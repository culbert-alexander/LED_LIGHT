// I was inspired by online resources from https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
// This guy is Matt Walker at University of Oklahoma
// I also used the ATTiny85 Datasheet a whole bunch
// Finally, Dr. Palmeri's example code at https://github.com/mlp6/Fundamentals-BME-Design/blob/master/PrototypingSkills/software.ino
// Besides their help the rest is me at Alexander Culbert

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// Look at me preprocess whoo whoo woooo
#define PIN_OUT PCINT0    // Pin 0
#define PIN_IN PCINT4     // Pin 4
#define PWM_Max_Val 255   // PWM Max is 255

// The initial, changeable values
int current_mode = 0;     // 0 = Off, 1 = On, 2 = Medium On, 3 = Low On, 4 = 2 Hz Flashing
int PWM_Current_Val = 0;  // Initally PWM is at 0
unsigned long old_time = 0;
unsigned long current_time = 0;
const long flash_rate = 50;
int flash_state = 0;


void setup() {
  cli(); //Disable interrupts ~briefly~
  // Goal is to enable interrupts and pullup resistors on input pin 4
  // Also set pin 0 as an output

  GIMSK = 0b00100000;                 // Enables interrupts
  PCMSK = 0b000010000;                // Turns on interrupts for Pin 4
  DDRB = 0b111101111;                 // Input or Output (Pin 0 is output)
  pinMode(PIN_OUT, OUTPUT);           // Pin is output
  digitalWrite(PIN_OUT, LOW);         // Initial LED is off
  PINB = 0b000010000;                 // Allows for Pin B Functionality on Pin 4
  PORTB = 0b000010000;                // Enables Pull-Up Resistor on Pin 4

  // DDxn determines if pin is input or output. If DDxn is logic one, Pxn is output. If zero. then input.
  // If PORTxn is login one while pin is input, then you get the pull-up resistor
  // If PORTxn is logic zero when pin is output, then port is driven low (zero). If logic one, then driven high.

  // ADCSRA &= ~_BV(ADEN); // idk if this is needed

  sei(); //Enable interrupts
}

void loop() {
  switch_statement();
}

void switch_statement() {
  switch (current_mode) {
    case 0:
      PWM_Current_Val = 0;
      analogWrite(PIN_OUT, PWM_Current_Val);
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_bod_disable();
      sleep_cpu();
      break;
    case 1:
      sleep_disable();
      PWM_Current_Val = PWM_Max_Val;
      analogWrite(PIN_OUT, PWM_Current_Val);
      break;
    case 2:
      PWM_Current_Val = int(PWM_Max_Val / 2);
      analogWrite(PIN_OUT, PWM_Current_Val);
      break;
    case 3:
      PWM_Current_Val = int(PWM_Max_Val / 4);
      analogWrite(PIN_OUT, PWM_Current_Val);
      break;
    case 4:
      PWM_Current_Val = PWM_Max_Val;
      flash();
      break;
  }
}

ISR(PCINT0_vect) {
  if (digitalRead(PIN_IN)) {
    current_mode = current_mode + 1;
    if (current_mode > 4) {
      current_mode = 0;
    }
  }
}


void flash() {
  current_time = millis();
  if (current_time - old_time >= flash_rate) {
    old_time = current_time;
    if (flash_state == 0) {
      flash_state = PWM_Current_Val;
    }
    else {
      flash_state = 0;
    }
    analogWrite(PIN_OUT, flash_state);
  }
}

