#include "Adafruit_APDS9960.h"
#include <avr/io.h> //per utiizzare DDC0 ecc


Adafruit_APDS9960 apds;

byte FirstROW = 0b11111110;  // 0 FOR OPEN
byte FirstCOL = 0b00000001;  // 1 FOR OPEN
uint8_t ValRows = 0b00000000;
uint8_t ValCols = 0b00000000;

void init_register(){
  DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2); // Set pin 8, 9, and 10 to output mode
  DDRC |= (1<<DDC0) | (1<<DDC1) | (1<<DDC2); // Set pin 14, 15, and 16 to output mode
  
}

int start = 1;

// *****    SHIFT REGISTER FUNCTIONS    ***** //

void Shift(volatile uint8_t *port, uint8_t data) {
  
  for (int i = 7; i >= 0; i--) {
    uint8_t bit = (data >> i) & 1;
    *port &= ~(1 << 0); // set data low
    if (bit) {
      *port |= (1 << 0); // set data high
    }
      *port ^= 0b00000100;
      *port ^= 0b00000100;
  }
  
}

void ShiftRegisterROWS(byte value) {
  PORTB = (0 << 1);
  uint8_t bit;
  Shift(&PORTB, value);
  //Latch high - show pattern
  PORTB = (1 << 1);
}

void ShiftRegisterCOLS(byte value) {
  PORTC = (0 << 1);
  uint8_t bit;
  Shift(&PORTC, value);
  //Latch high - show pattern
  PORTC = (1 << 1);
}
// ***** APDS GESTURE MANAGEMENT FUNCTIONS *****//

byte gest_right(uint8_t value) {
  if(value == 128){ // 128 = 0b10000000
    return value;
  }else{
  value = (value << 1);
  return value;
  }
}

byte gest_left(uint8_t value) {
  if(value == 1){
    return value;
  }else{
  value = (value >> 1);
  return value;
  }
}

byte gest_down(uint8_t value) {
  if(value == 127){
    return value;
  }else{
    value = value << 1;
    value ^= 1;
    return value;
    }
}

byte gest_up(uint8_t value) {
  if(value == 254){
    return value;
  }else{
    value = value >> 1;
    value = value ^ 0b10000000;
    return value;
    }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!apds.begin()) {
    Serial.println("failed to initialize device! Please check your wiring.");
  } else Serial.println("Device initialized!");
  //boolean begin(uint16_t iTimeMS = 10, apds9960AGain_t = APDS9960_AGAIN_4X,uint8_t addr = APDS9960_ADDRESS, TwoWire *theWire = &Wire)
  //gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);
  init_register();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (start == 1) {
    ShiftRegisterROWS(FirstROW);
    ShiftRegisterCOLS(FirstCOL);
    ValRows = FirstROW;
    ValCols = FirstCOL;
    start = 0;
  }

  uint8_t gesture = apds.readGesture();
  if (gesture == APDS9960_DOWN) {
      ValRows = gest_down(ValRows);
      ShiftRegisterROWS(ValRows);
  }
  if (gesture == APDS9960_UP) {
      ValRows = gest_up(ValRows);
      ShiftRegisterROWS(ValRows);
    }
  if (gesture == APDS9960_LEFT) {
    ValCols = gest_left(ValCols);
      ShiftRegisterCOLS(ValCols);
  }
  if (gesture == APDS9960_RIGHT) {
    ValCols = gest_right(ValCols);
    ShiftRegisterCOLS(ValCols);
  }
}