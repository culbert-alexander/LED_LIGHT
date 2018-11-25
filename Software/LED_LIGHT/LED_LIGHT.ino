// I was inspired by online resources from https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/ 
// This guy is Matt Walker at University of Oklahoma
// I also used the ATTiny85 Datasheet a whole bunch
// Finally, Dr. Palmeri's example code at https://github.com/mlp6/Fundamentals-BME-Design/blob/master/PrototypingSkills/software.ino
// Besides their help the rest is me at Alexander Culbert

// Look at me preprocess whoo whoo woooo
#define FLASH_HZ 2        // Frequency of flash
#define PIN_OUT PCINT0    // Pin 0
#define PIN_IN PCINT4     // Pin 4
#define PWM_Max_Val 255   // PWM Max is 255

// The initial, changeable values
int current_mode = 0;     // 0 = Off, 1 = On, 2 = Medium On, 3 = Low On, 4 = 2 Hz Flashing
bool Button_Push = false; // Initially the button hasn't been pushed
int PWM_Current_Val = 0;  // Initally PWM is at 0


void setup() {
  cli(); //Disable interrupts ~briefly~
  // Goal is to enable interrupts and pullup resistors on input pin 4
  // Also set pin 0 as an output
  
  GIMSK = 0b00100000;         // Enables interrupts
  PCMSK = 0b000010000         // Turns on interrupts for Pin 4
  DDRB = 0b000000001          // Input or Output (Pin 0 is output)
  pinMode(PIN_OUT, OUTPUT);   // Pin is output
  digitalWrite(PIN_OUT, LOW); // Initial LED is off
  PINB = 0b000010000          // Allows for Pin B Functionality on Pin 4
  PORTB = 0b000010000         // Enables Pull-Up Resistor on PIn 4
  
  // DDxn determines if pin is input or output. If DDxn is logic one, Pxn is output. If zero. then input.
  // If PORTxn is login one while pin is input, then you get the pull-up resistor
  // If PORTxn is logic zero when pin is output, then port is driven low (zero). If logic one, then driven high.
  
  sei(); //Enable interrupts
}

void loop() {
  Button_Press(); // Marks the operating mode if the button gets pressed
  Set_PWM();      // Sets the PWM (i.e. what is the LED doing)
  LED_On();       // Turns on LED at PWM Value
}

ISR(PCINT0_vect) {
  // I only think I need this function, but just in case...
}

void Button_Press() {
  if (Button_Push == true) {
    current_mode = current_mode + 1;
    if (current_mode == 5) {
      current_mode = 0;
    }
  }
  Button_Push = false;
}

void Set_PWM() {
  switch (current_mode) {
    case 0:
      PWM_Current_Val = 0;
      break;
    case 1:
      PWM_Current_Val = PWM_Max_Val;
      break;
    case 2:
      PWM_Current_Val = int(PWM_Max_Val/2);
      break;
    case 3:
      PWM_Current_Val = int(PWM_Max_Val/4);
      break;
    case 4:
      PWM_Current_Val = PWM_Max_Val;
      flash();
      break;
  }
}

void flash() {
  analogWrite(PIN_OUT, PWM_Current_Val);
  delay(500);
  analogWrite(PIN_OUT, 0);
  delay(500);
}

void LED_On() {
  analogWrite(PIN_OUT, PWM_Current_Val);
}


