//Garrett McSweeney
//Kevin Valadez Marquez

//CPE 301.1001

//This code controls a humidifier using a usier inputted humidity level as reference
// Circuit and documentation are posted on github





#include <DHT11.h> //DHT11 Humidity Sensor Library by Dhruba Saha
#include <RTClib.h> // RTC library by Adafruit
#include <LiquidCrystal.h>

#define RDA 0x80
#define TBE 0x20  




volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;



volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;


volatile unsigned char* ddr_a = (unsigned char*) 0x21;
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* pin_a = (unsigned char*) 0x20;
volatile unsigned char *ddr_b = (unsigned char *) 0x24;
volatile unsigned char *port_b =    (unsigned char *) 0x25;
volatile unsigned char* ddr_e = (unsigned char*) 0x2D;
volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* pin_e = (unsigned char*) 0x2C;

int state = 0;
unsigned int uInput = 0;
bool humid_power = false;

int code = 0;
int errorInit = 0;
unsigned long PMillis = 0;
int blinkCount = 0;
bool ledState = 0;

DHT11 dht11(28);

RTC_DS3231 rtc;

const int RS = 30, EN = 31, D4 = 35, D5 = 36, D6 = 37, D7 = 38;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);



void setup() {

  U0init(9600);

  rtc.begin();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);


  //power button interrupt
  *ddr_e |= 0b00010000;
  *port_e &= ~0b00010000;
  attachInterrupt(digitalPinToInterrupt(2), power, RISING);
  
  // setup the ADC
  //adc_init();
  

  // Set Led to output
  *ddr_a |=  0b00111000;
  // Set humidifier to output
  *ddr_a |=  0b00000100;
  *port_a |= 0b00000100;
  // Set humidity sensor as input
  *ddr_a &= ~0b01000000;
  *port_a &= ~0b01000000;
  // Set reset button as input
  *ddr_a &= ~0b00000010;
  *port_a &= ~0b00000010;
  // Set off button as input
  *ddr_a &= ~0b00000001;
  *port_a &= ~0b00000001;




}


void loop() {
  uInput = analogRead(0);
  //uInput = adc_read(0);
  uInput = map(uInput, 0, 1023, 0, 100);

  unsigned char uInput1 = uInput / 10 + '0';
  unsigned char uInputT = uInput % 10 + '0';

  date();
  U0putchar('U');
  U0putchar('s');
  U0putchar('e');
  U0putchar('r');
  U0putchar(' ');
  U0putchar('I');
  U0putchar('n');
  U0putchar('p');
  U0putchar('u');
  U0putchar('t');
  U0putchar(':');
  U0putchar(' ');
  U0putchar(uInput1);
  U0putchar(uInputT);
  U0putchar('\n');

  
  //Serial.print("User Input: ");
  //Serial.println(uInput);

  if (state == 0){
    date();
    //Serial.println("Off State");
    U0putchar('O');
    U0putchar('f');
    U0putchar('f');
    U0putchar(' ');
    U0putchar('S');
    U0putchar('t');
    U0putchar('a');
    U0putchar('t');
    U0putchar('e');
    U0putchar('\n');

    Off();
  }

  if (state == 1){
    date();
    U0putchar('I');
    U0putchar('d');
    U0putchar('l');
    U0putchar('e');
    U0putchar(' ');
    U0putchar('S');
    U0putchar('t');
    U0putchar('a');
    U0putchar('t');
    U0putchar('e');
    U0putchar('\n');
    //Serial.println("Idle State");
    Idle();
  }

  if (state == 2){
    date();
    U0putchar('O');
    U0putchar('n');
    U0putchar(' ');
    U0putchar('S');
    U0putchar('t');
    U0putchar('a');
    U0putchar('t');
    U0putchar('e');
    U0putchar('\n');
    //Serial.println("On State");
    On();
  }

  if (state == 3){
    date();
    U0putchar('E');
    U0putchar('r');
    U0putchar('r');
    U0putchar('o');
    U0putchar('r');
    U0putchar(' ');
    U0putchar('S');
    U0putchar('t');
    U0putchar('a');
    U0putchar('t');
    U0putchar('e');
    U0putchar('\n');
    //Serial.println("error");
    Error(code);
  }

  if(*pin_a & 0b00000001){state = 0;}

}

void power(){
  if (state == 0){state = 1;}
  else{}
  
  }

void date(){
  DateTime now = rtc.now();
  unsigned char month1 = now.month() /10 + '0';
  unsigned char monthT = now.month() %10 + '0';
  unsigned char day1 = now.day() /10 + '0';
  unsigned char dayT = now.day() %10 + '0';
  unsigned char year1 = now.year() /10 + '0';
  unsigned char yearT = now.year() %10 + '0';
  unsigned char hour1 = now.hour() /10 + '0';
  unsigned char hourT = now.hour() %10 + '0';
  unsigned char min1 = now.minute() /10 + '0';
  unsigned char minT = now.minute() %10 + '0';
  unsigned char sec1 = now.second() /10 + '0';
  unsigned char secT = now.second() %10 + '0';

  U0putchar(month1);
  U0putchar(monthT);
  U0putchar('/');
  U0putchar(day1);
  U0putchar(dayT);
  U0putchar('/');
  U0putchar(year1);
  U0putchar(yearT);
  U0putchar(' ');
  U0putchar(hour1);
  U0putchar(hourT);
  U0putchar(':');
  U0putchar(min1);
  U0putchar(minT);
  U0putchar(':');
  U0putchar(sec1);
  U0putchar(secT);
  U0putchar(' ');
  U0putchar('-');
  U0putchar('-');
  U0putchar(' ');

 
  //Serial.print(now.month());
  //Serial.print("/");
  //Serial.print(now.day(), DEC);
  //Serial.print("/");
  //Serial.print(now.year(), DEC);
  //Serial.print(" ");
  //Serial.print(now.hour(), DEC);
  //Serial.print(":");
  //Serial.print(now.minute(), DEC);
  //Serial.print(":");
  //Serial.print(now.second(), DEC);
  //Serial.print(" -- ");
  
}


void Off(){
  //Turn light to red
  *port_a |=  0b00001000;
  *port_a &= ~0b00110000;

  //Turn off humidifier

  
  *port_a |= 0b00000100;
 
  humid_power = false;
  }



void Idle(){
  //Turn light to yellow
  *port_a |=  0b00100000;
  *port_a &= ~0b00011000;




  //Turn off humidifier
  *port_a |= 0b00000100;
  
  
  //Read and update user input
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(uInput);
  lcd.print("%");
  

  //Check humidity sensor
  unsigned char humid1 = dht11.readHumidity() /10 + '0';
  unsigned char humidT = dht11.readHumidity() %10 + '0';
  date();
  U0putchar('H');
  U0putchar('u');
  U0putchar('m');
  U0putchar('i');
  U0putchar('d');
  U0putchar('i');
  U0putchar('t');
  U0putchar('y');
  U0putchar(':');
  U0putchar(' ');
  U0putchar(humid1);
  U0putchar(humidT);
  U0putchar('%');
  U0putchar('\n');

  //Serial.print("Humidity: ");
  //Serial.println(dht11.readHumidity());
  
  if (dht11.readHumidity() < (uInput)){
    state = 2;
  }

  //Error Checking
  if (dht11.readHumidity() >= 100){code = 1;state = 3;errorInit = 1;} // Error 1: Humidity Sensor not Functioning
  
}

void On(){
  //Turn light to green
  *port_a |=  0b00010000;
  *port_a &= ~0b00101000;

  
  

  //Turn on humidifier


  
  *port_a &= ~0b00000100;
  
  
  unsigned char humid1 = dht11.readHumidity() /10 + '0';
  unsigned char humidT = dht11.readHumidity() %10 + '0';
  date();
  U0putchar('H');
  U0putchar('u');
  U0putchar('m');
  U0putchar('i');
  U0putchar('d');
  U0putchar('i');
  U0putchar('t');
  U0putchar('y');
  U0putchar(':');
  U0putchar(' ');
  U0putchar(humid1);
  U0putchar(humidT);
  U0putchar('%');
  U0putchar('\n');

  //Read and update user input
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(uInput);
  lcd.print("%");

  //Check humidity sensor
  
  if (dht11.readHumidity() > (uInput + 5)){
    state = 1;
  }

  //Error Checking
  if (dht11.readHumidity() >= 100){code = 1;state = 3;errorInit = 1;} // Error 1: Humidity Sensor not Functioning
}

void Error(int code){
  //Turn light to blinking red
  if (errorInit == 1){
    PMillis = millis();
    ledState = LOW;
    errorInit = 0;
 
  }
  unsigned long CMillis = millis();
  if (CMillis - PMillis >= 350) {
    PMillis = CMillis; // Save the last time you blinked
    // Toggle the LED state
    if (ledState == LOW) {
        ledState = HIGH;
        *port_a |=  0b00001000;
        *port_a &= ~0b00110000;
    } 
    else {
      ledState = LOW;
      *port_a &= ~0b00111000;
      blinkCount++; // Increment count only after a full ON/OFF cycle
    }

      
  }
  

    
  

  //Turn off humidifier
  *port_a |= 0b00000100;

  if (code == 1)
  {lcd.clear();
  lcd.print("E01");
  date();
  U0putchar('E');
  U0putchar('0');
  U0putchar('1');
  U0putchar(':');
  U0putchar(' ');
  U0putchar('H');
  U0putchar('u');
  U0putchar('m');
  U0putchar('i');
  U0putchar('d');
  U0putchar('i');
  U0putchar('t');
  U0putchar('y');
  U0putchar(' ');
  U0putchar('S');
  U0putchar('e');
  U0putchar('n');
  U0putchar('s');
  U0putchar('o');
  U0putchar('r');
  U0putchar(' ');
  U0putchar('n');
  U0putchar('o');
  U0putchar('t');
  U0putchar(' ');
  U0putchar('F');
  U0putchar('u');
  U0putchar('n');
  U0putchar('c');
  U0putchar('t');
  U0putchar('i');
  U0putchar('o');
  U0putchar('n');
  U0putchar('i');
  U0putchar('n');
  U0putchar('g');
  U0putchar('\n');

  //Serial.println("E01: Humidity Sensor not Functioning");
  }

  if (*pin_a & 0b00000010 ){state = 1;} //resets error state

  
    
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
 *myUCSR0B = 0b00001000;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
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


void adc_init() //write your code after each commented line and follow the instruction 
{
  // setup the A register
 // set bit 7 to 1 to enable the ADC 
 *my_ADCSRA |= 0b10000000;

 // clear bit 5 to 0 to disable the ADC trigger mode

*my_ADCSRA &= 0b11011111;

 // clear bit 3 to 0 to disable the ADC interrupt 

 *my_ADCSRA &= 0b11110111;

 // clear bit 0-2 to 0 to set prescaler selection to slow reading

 *my_ADCSRA &= 0b11111000;

  // setup the B register
// clear bit 3 to 0 to reset the channel and gain bits

*my_ADCSRB &= 0b11110111;

 // clear bit 2-0 to 0 to set free running mode

 *my_ADCSRB &= 0b11111000;

  // setup the MUX Register
 // clear bit 7 to 0 for AVCC analog reference

 *my_ADMUX &= 0b01111111;

// set bit 6 to 1 for AVCC analog reference

*my_ADMUX |= 0b01000000;

  // clear bit 5 to 0 for right adjust result

  *my_ADMUX &= 0b11011111;

 // clear bit 4-0 to 0 to reset the channel and gain bits

 *my_ADMUX &= 0b11100000;

}
unsigned int adc_read(unsigned char adc_channel_num) //work with channel 0
{
  // clear the channel selection bits (MUX 4:0)
 
 *my_ADMUX &= 0b11100000;

  // clear the channel selection bits (MUX 5) hint: it's not in the ADMUX register

*my_ADCSRB &= 0b11110111;
 
  // set the channel selection bits for channel 0

  
 *my_ADMUX &= 0b11110000;

  // set bit 6 of ADCSRA to 1 to start a conversion

  *my_ADCSRA |= 0b01000000; 

  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register and format the data based on right justification (check the lecture slide)
  
  unsigned int val = (*my_ADC_DATA & 0x03FF);
  return val;
}