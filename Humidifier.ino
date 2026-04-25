//Garrett McSweeney
//Kevin Valadez Marquez





#include <DHT11.h> //DHT11 Humidity Sensor Library by Dhruba Saha



volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;


#define RDA 0x80
 #define TBE 0x20  
 volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
 volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
 volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
 volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
 volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;


volatile unsigned char* ddr_a = (unsigned char*) 0x21;
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* pin_a = (unsigned char*) 0x20;
volatile unsigned char *ddr_b = (unsigned char *) 0x24;
volatile unsigned char *port_b =    (unsigned char *) 0x25;

int state = 2;
int uInput = 40;
int on_count = 1;

DHT11 dht11(28);

void setup() {

  Serial.begin(9600);
  

// Set Led to output
*ddr_a |=  0b00111000;
// Set humidifier to output
*ddr_a |=  0b00000100;
// Set humidity sensor as input
*ddr_a &= ~0b01000000;
*port_a &= ~0b01000000;

}


void loop() {
  
  

  if (state == 0){
    Serial.println("off");
    Off();
  }

  if (state == 1){
    Serial.println("idle");
    Idle();
  }

  if (state == 2){
    On();
  }

  if (state == 3){
    Serial.println("error");
    Error(5);
  }

  if (state >= 4){
    state = 0;
  }

}

void Off(){
  //Turn light to red
  *port_a |=  0b00001000;
  *port_a &= ~0b00110000;

  delay(5000);
  state += 1;

}

void Idle(){
  //Turn light to yellow
  *port_a |=  0b00100000;
  *port_a &= ~0b00011000;

  delay(5000);
  state += 1;

  //Turn off humidifier

  //Read and update user input

  //Check humidity sensor
  if (dht11.readHumidity() < uInput){
    state = 2;
  }
  
}

void On(){
  //Turn light to green
  *port_a |=  0b00010000;
  *port_a &= ~0b00101000;

  
  

  //Turn on humidifier


  if (on_count == 1){
  *port_a |= 0b00000100;
  delay(50);
  *port_a &= ~0b00000100;
  on_count = 0;
  }
Serial.println(dht11.readHumidity());
  //Read and update user input

  //Check humidity sensor
  if (dht11.readHumidity() > (uInput + 5)){
    state = 1;
  }
}

void Error(int code){
  //Turn light to blinking red
  for (int i = 0; i < code; i++){
    
    *port_a |=  0b00001000;
    *port_a &= ~0b00110000;
    delay(500);
    *port_a &= ~0b00111000;
    //my_delay(100);
    delay(100);
  
  }
  
  

  //display error code
}


void my_delay(unsigned int freq)
{
  // calc period
  double period = 1.0/double(freq);
  // 50% duty cycle
  double half_period = period/2;
  // clock period def
  double clk_period = 0.0000000625;
  // calc ticks
  unsigned int ticks = half_period/clk_period;
  // stop the timer
  *myTCCR1B &= 0xF8;
  // set the counts
  *myTCNT1 = 65536 - ticks; //check slide

  * myTCCR1A = 0x0;
  // start the timer
  * myTCCR1B |= 0b00000001;
  // wait for overflow
  while((*myTIFR1 & 0x01)==0); 
  // stop the timer
  *myTCCR1B &= 0xF8;   
  // reset TOV           
  *myTIFR1 |= 0x01;
  
}

void U0init(unsigned long U0baud)
{
//  Students are responsible for understanding
//  this initialization code for the ATmega2560 USART0
//  and will be expected to be able to intialize
//  the USART in differrent modes.
//
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
//
// Read USART0 RDA status bit and return non-zero true if set
//
unsigned char U0kbhit()
{
  return (*myUCSR0A & RDA);
   
  
  
}
//
// Read input character from USART0 input buffer
//
unsigned char U0getchar()
{
  return *myUDR0;
  
}
//
// Wait for USART0 (myUCSR0A) TBE to be set then write character to
// transmit buffer
//
void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}
