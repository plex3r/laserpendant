#include <Wire.h>
#include <VL6180X.h>
#include <EEPROM.h>

// variables will change:
  const int numReadings = 100;
  //int buttonState = 0;         // variable for reading the pushbutton status
  int cButtonState = 0;
  int zButtonState = 0;
  int previousButtonState = 0;
  int xAxis = 0;
  int yAxis = 0;
  int ledState = LOW;
  int readings[numReadings];      // the readings from the analog input
  int readIndex = 0;              // the index of the current reading
  int total = 0;                  // the running total
  int average = 0;                // the average
  int calibrated = 0;
  //int donecalibration = 0;
  int loopcount = 0;
  const long interval = 1000;   
  unsigned long previousMillis = 0;
  VL6180X sensor;
// the setup function runs once when you press reset or power the board
void setup() {
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
     readings[thisReading] = 0;
  }
  Serial.begin(115200);
  Wire.begin();
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(500);
  nunchuck_setpowerpins(); // use analog pins 2&3 as fake gnd & pwr
  nunchuck_init(); // send the initilization handshake
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  average = EEPROM.read(0);
  Serial.print("Stored Calibration Value: ");
  Serial.print(average);
}

// the loop function runs over and over again forever
void loop() {

   nunchuck_get_data();
   nunchuck_get_buttons();
   
  if (cButtonState == 1) {  
   //while loop time is less than interval this code will run
   //read sensor to array
   for(loopcount=1; loopcount<=100; loopcount++) {
   unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
     // save the last time you blinked the LED
     previousMillis = currentMillis;
    }
     // if the LED is off turn it on and vice-versa:
     if (ledState == LOW) {
       ledState = HIGH;
     } else {
       ledState = LOW;
     }
   // set the LED with the ledState of the variable:
   digitalWrite(4, ledState);
   Serial.print("Current Sensor Value: ");
   Serial.print(sensor.readRangeSingleMillimeters());
   if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
   Serial.print("\r\n");
   // subtract the last reading:
   total = total - readings[readIndex];
   // read from the sensor:
   readings[readIndex] = sensor.readRangeSingleMillimeters();
   // add the reading to the total:
   total = total + readings[readIndex];
   // advance to the next position in the array:
   readIndex = readIndex + 1;
   // if we're at the end of the array...
   if (readIndex >= numReadings) {
   // ...wrap around to the beginning:
     readIndex = 0;
   }
   }
   calibrated=!calibrated;
   cButtonState=!cButtonState;
   }
   else {
   digitalWrite(4, HIGH);
    }
   
   if (calibrated == 1) {
   // calculate the average:
   average = total / numReadings;
   Serial.println("Sensor Average Value: ");
   Serial.println(average);
   Serial.println("\r\n");
   EEPROM.write(0, average);
   calibrated=!calibrated;
   }
   }

//
// Nunchuck functions
//

static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize        
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
void nunchuck_init()
{ 
  //Wire.begin();                  // join i2c bus as master
  Wire.beginTransmission(0x52); // transmit to device 0x52
  Wire.write(0x40);    // sends memory address
  Wire.write(0x00);    // sends sent a zero.  
  Wire.endTransmission(); // stop transmitting
}

// Send a request for data to the nunchuck
// was "send_zero()"
void nunchuck_send_request()
{
  Wire.beginTransmission(0x52); // transmit to device 0x52
  Wire.write(0x00);    // sends one byte
  Wire.endTransmission(); // stop transmitting
}

// Receive data back from the nunchuck, 
int nunchuck_get_data()
{
    int cnt=0;
    Wire.requestFrom (0x52, 6); // request data from nunchuck
    while (Wire.available ()) {
      // receive byte as an integer
      nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.read());
      cnt++;
    }
    nunchuck_send_request();  // send request for next data payload
    // If we recieved the 6 bytes, then go print them
    if (cnt >= 5) {
     return 1;   // success
    }
    return 0; //failure
}


int nunchuck_get_buttons()
{ 
  static int i=0; 
  int z_button = 0;
  int c_button = 0;
  // byte nunchuck_buf[5] contains bits for z and c buttons
  // it also contains the least significant bits for the accelerometer data
  // so we have to check each bit of byte outbuf[5]
  if ((nunchuck_buf[5] >> 0) & 1) 
    z_button = 1;
if (z_button == 0) 
  zButtonState = !zButtonState;
  if ((nunchuck_buf[5] >> 1) & 1)
    c_button = 1;
if (c_button == 0)
    cButtonState = 1;
   delay(100);
   // Serial.print(cButtonState);
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}
