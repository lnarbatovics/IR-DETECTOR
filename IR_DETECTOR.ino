#include <CurieEEPROM.h>






/*
infrared sensor detector. connect signal of infrared to analog pin 0. as the distance
from an object to sensor increases/decreases, you will increase/decrease
speed of led blinks from HIGH to LOW

*/

#include <CurieBLE.h>
#include <Adafruit_NeoPixel.h>
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUM_LEDS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);


BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService motionService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
BLECharCharacteristic ir1Characteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLECharCharacteristic ir2Characteristic("19B10001-E8F2-537E-4F6C-D104768A1215", BLERead | BLENotify);
BLEIntCharacteristic centralWritingCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1216", BLERead | BLEWrite | BLENotify);
BLEUnsignedIntCharacteristic ledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1217", BLERead | BLEWrite | BLENotify);

int time;  // the variable used to set the Timer

bool centralWriting = 0;
bool ledState = 0; // The LED status toggle

const int totalSecInUsec = 180000000;    // total timemout
const int tenSecInUsec = 20000000;   // twenty seconds in mirco second unit.
                 
struct config_t
{
    char localname;
    
} configuration;


int IR_Pin1 = 8;
int IR_Pin2 = 9;   // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int IR_Value1 = 0;
int IR_Value2 = 0; // variable to store the value coming from the sensor



void setup() {
/*  EEPROM_readAnything(0, configuration);
  if (localname == 0){
    const char BLELocalName[] = {"PAL_BLE"};
  } */
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  Serial.begin(9600);
 
  
  blePeripheral.setLocalName("BLELocalName");
  blePeripheral.setAdvertisedServiceUuid(motionService.uuid());
  blePeripheral.addAttribute(motionService);
  blePeripheral.addAttribute(ir1Characteristic);
  blePeripheral.addAttribute(ir2Characteristic);
  blePeripheral.addAttribute(centralWritingCharacteristic);
  blePeripheral.addAttribute(ledCharacteristic);  
  ir1Characteristic.setValue(0);
  ir2Characteristic.setValue(0);
  centralWritingCharacteristic.setValue(0);
  ledCharacteristic.setValue(0);
  blePeripheral.begin();
  
  // Initiate Light Strip
  pixels.begin();
  pixels.show();


}

void loop() {
    // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

    
    // Predefined Colors
  uint32_t lowWhite = pixels.Color(25,25,25);
  uint32_t medWhite = pixels.Color(127, 127, 127);
  uint32_t highWhite = pixels.Color(255, 255, 255);
  uint32_t lowRed = pixels.Color(25,0,0);
  uint32_t medRed = pixels.Color(127, 0, 0);
  uint32_t highRed = pixels.Color(255, 0, 0);
  uint32_t lowBlue = pixels.Color(0,0,25);
  uint32_t medBlue = pixels.Color(0, 0, 127);
  uint32_t highBlue = pixels.Color(0, 0, 255);
  uint32_t lowYellow = pixels.Color(25,10,0);
  uint32_t medYellow = pixels.Color(127, 50, 0);
  uint32_t highYellow = pixels.Color(255, 100, 0);
  uint32_t blankLED = pixels.Color(0,0,0);
  
  
  // read the value from the sensor:
  delay(100);
  IR_Value1 = digitalRead(IR_Pin1);    
  IR_Value2 = digitalRead(IR_Pin2);    
  delay(100);
  Serial.print("IR Values= ");
  Serial.print(IR_Value1);
  Serial.print(":");
  Serial.println(IR_Value2);

 
  if (IR_Value1 == 1 && IR_Value2 == 1){
      ir1Characteristic.setValue(0);
      ir2Characteristic.setValue(0);
      if (centralWritingCharacteristic.value() == 1){
        ledControl(0, ledCharacteristic.value());
      } else {    
        ledControl(0, medWhite); // Moderately bright green color.
        uint32_t color = pixels.getPixelColor(0);
        ledCharacteristic.setValue(color);
      }
     }
  else if (IR_Value1 == 1){
    ir1Characteristic.setValue(0);
    if (centralWritingCharacteristic.value() == 1){
      ledControl(0, ledCharacteristic.value());
    } else {    
    ledControl(0, lowWhite); // Moderately bright green color.
    uint32_t color = pixels.getPixelColor(0);
    ledCharacteristic.setValue(color);
    }
  }
  else if (IR_Value2 == 1){
    ir2Characteristic.setValue(0);
    if (centralWritingCharacteristic.value() == 1){
      ledControl(0, ledCharacteristic.value());
    } else {    
    ledControl(0, lowWhite); // Moderately bright green color.
    uint32_t color = pixels.getPixelColor(0);
    ledCharacteristic.setValue(color);
    }
  }
  else {
    ir1Characteristic.setValue(1);
    ir2Characteristic.setValue(1);
    if (centralWritingCharacteristic.value() == 1){
      ledControl(0, ledCharacteristic.value());
    } else {
    ledControl(0, blankLED); // 
    uint32_t color = pixels.getPixelColor(0);
    ledCharacteristic.setValue(color);
    }
  }
  
  
  delay(100); 
  // stop the program for <sensorValue> milliseconds:
     
  // turn the ledPin off:        

  // stop the program for for <sensorValue> milliseconds:
      
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

        // while the central is still connected to peripheral:
    if (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (ledCharacteristic.written()) {
        if (ledCharacteristic.value()) {   // any value other than 0
          Serial.println("LED on");
          ledControl(0, ledCharacteristic.value());
          Serial.println(ledCharacteristic.value()); // will turn the LED on
          
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          ledControl(0, blankLED);         // will turn the LED off
          
       }
     }
   }
  }
}

void ledControl(int stripCase, uint32_t stripColor){
  switch(stripCase){
    case 0: setAll(stripColor); break;
  }

}

void showStrip() { 
   pixels.show(); 
}
 
void setPixel(int Pixel, uint32_t stripColor) { 
  pixels.setPixelColor(Pixel, stripColor);
}
void setAll(uint32_t stripColor){ 
  for(int i = 0; i < NUM_LEDS; i++ ) { 
  setPixel(i, stripColor);
 } 
  showStrip();
}



