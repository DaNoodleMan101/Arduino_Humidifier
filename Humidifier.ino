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

//Registers for Serial Communication
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

 //Registers for ADC
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

//Registers for GPIO
volatile unsigned char* ddr_a = (unsigned char*) 0x21;
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* pin_a = (unsigned char*) 0x20;
volatile unsigned char *ddr_b = (unsigned char *) 0x24;
volatile unsigned char *port_b =    (unsigned char *) 0x25;
volatile unsigned char* ddr_e = (unsigned char*) 0x2D;
volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* pin_e = (unsigned char*) 0x2C;
volatile unsigned char* port_f = (unsigned char*) 0x31;


int state = 0; // Machine state
unsigned int uInput = 0; //User pot input
int code = 0; //Error Code
int errorInit = 0; //Toggle for error init
unsigned long PMillis = 0; //previous millis 
int blinkCount = 0; // Counts blink for error state
bool ledState = 0; //Stores LED state for error

//DHT11 pin
DHT11 dht11(28);
RTC_DS3231 rtc;

//LCD pins
const int RS = 30, EN = 31, D4 = 35, D5 = 36, D6 = 37, D7 = 38;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);



void setup() {

  U0init(9600); //Serial init

  rtc.begin(); //Start RTC

  adc_init(); //ADC init

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);


  //power button interrupt
  *ddr_e |= 0b00010000;
  *port_e &= ~0b00010000;
  attachInterrupt(digitalPinToInterrupt(2), power, RISING);
  
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
  
  //Off state
  if (state == 0){ 
    date();
    
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

  //Idle state
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

  //On state
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

  //Off state
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

// Power interrupt function
void power(){
  if (state == 0){state = 1;}
  else{}
  
  }

// Displays User input
void User_Input(){

  uInput = adc_read(0);
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
}

//Displays date and time 
void date(){
  DateTime now = rtc.now();

  //Converting date and time to char
  unsigned char month1 = now.month() /10 + '0';
  unsigned char monthT = now.month() %10 + '0';
  unsigned char day1 = now.day() /10 + '0';
  unsigned char dayT = now.day() %10 + '0';
  unsigned char year1 = now.year() %10 + '0';
  unsigned char yearTh = (now.year() / 1000) % 10 +'0';
  unsigned char yearH = (now.year() / 100) % 10 + '0'; 
  unsigned char yearT = (now.year() / 10) % 10 + '0'; 
  unsigned char hour1 = now.hour() /10 + '0';
  unsigned char hourT = now.hour() %10 + '0';
  unsigned char min1 = now.minute() /10 + '0';
  unsigned char minT = now.minute() %10 + '0';
  unsigned char sec1 = now.second() /10 + '0';
  unsigned char secT = now.second() %10 + '0';

  //Display date and time
  U0putchar(month1);
  U0putchar(monthT);
  U0putchar('/');
  U0putchar(day1);
  U0putchar(dayT);
  U0putchar('/');
  U0putchar(yearTh);
  U0putchar(yearH);
  U0putchar(yearT);
  U0putchar(year1);
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
  
}

//Off State
void Off(){
  //Turn light to red
  *port_a |=  0b00001000;
  *port_a &= ~0b00110000;

  //Turn off humidifier
  *port_a |= 0b00000100;
  
}

//Idle State
void Idle(){
  //Turn light to blue
  *port_a |=  0b00100000;
  *port_a &= ~0b00011000;

  //Turn off humidifier
  *port_a |= 0b00000100;
  
  //Read and update user input
  User_Input();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(uInput);
  lcd.print("%");
  
  //Check humidity sensor and display
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

  //Sees if Humidity is below desired level
  if (dht11.readHumidity() < (uInput)){
    state = 2;
  }

  //Error Checking
  if (dht11.readHumidity() >= 100){code = 1;state = 3;errorInit = 1;} // Error 1: Humidity Sensor not Functioning
  
}

// On state
void On(){
  //Turn light to green
  *port_a |=  0b00010000;
  *port_a &= ~0b00101000;

  //Turn on humidifier
  *port_a &= ~0b00000100;
  
  //Read and display humidity level
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
  User_Input();
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

// Error State
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

  //Error code 01
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
  }

 if (*pin_a & 0b00000010 ){state = 1;} //resets error state

  
    
}

// Serial init  
void U0init(unsigned long U0baud)
{

 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0b00001000;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

// Serial Transmitt
void U0putchar(unsigned char U0pdata)
{
  
  while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}

//ADC init
void adc_init() {
    // Enable ADC
    *my_ADCSRA |= 0b10000000;

    // Disable Trigger & Interrupts
    *my_ADCSRA &= 0b11011111;
    *my_ADCSRA &= 0b11110111;

    // Set Prescaler to 128 (111) for stable readings at 16MHz
    *my_ADCSRA |= 0b00000111; 

    // Setup ADCSRB: Clear MUX5 (Bit 3)
    *my_ADCSRB &= 0b11110111;

    // Setup ADMUX: AVCC Ref (01), Right Adjust (0)
    *my_ADMUX &= 0b01111111; 
    *my_ADMUX |= 0b01000000;
    *my_ADMUX &= 0b11011111; // Right adjust
    *my_ADMUX &= 0b11100000; // Clear channel bits
}

// Read ADC data
unsigned int adc_read(unsigned char adc_channel_num) {
    // 1. Clear existing MUX bits (0-4 in ADMUX, 5 in ADCSRB)
    *my_ADMUX &= 0b11100000;
    *my_ADCSRB &= ~(1 << 3); // Clear MUX5

    // 2. Set channel (Assuming channel 0-7 for this logic)
    // If you wanted channel 0, doing nothing here is technically correct
    *my_ADMUX |= (adc_channel_num & 0x07); 
    
    // If channel > 7, you would need to set MUX5 in ADCSRB
    if(adc_channel_num > 7) {
        *my_ADCSRB |= (1 << 3);
    }

    // 3. Start Conversion
    *my_ADCSRA |= 0x40; 

    // 4. Wait for ADSC bit to clear
    while((*my_ADCSRA & 0x40));

    // 5. Return 10-bit result (Must read ADCL then ADCH)
    return (*(volatile unsigned int*)0x78); 
}