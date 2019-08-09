
// REQUIRES the following Arduino libraries: (Tools>>Manage Libraries)
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor//++++++++++++++++++++ ANALOG AND DIGITAL IDS ++++++
int tempSensor = 2;//DigitalPin_2
int waterPump = 3;//DigitalPin_3

int tempLEDpin = 6;// DigitalPin_6
int humidityLEDpin = 7;//DigitalPin_7
int soilPin = A0;//AnalogPin_0
int waterLevel = A4;//AnalogPin_4
#include <Adafruit_NeoPixel.h>
#include "DHT.h"
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht = DHT(tempSensor, DHTTYPE);
// initialize the library with the numbers of the interface pins
Adafruit_NeoPixel tempLEDs(5, tempLEDpin, NEO_GRB + NEO_KHZ800);//(number of leds in string, pin, type of led string)
Adafruit_NeoPixel humidLEDs(5, humidityLEDpin, NEO_GRB + NEO_KHZ800);
//++++++++++++++++++ DATA +++++++++++
const int AirValue = 550;   //you need to replace this value with Value_1
const int WaterValue = 150;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;
byte temperature = 0;
byte humidity = 0;
byte data[40] = {0};
long waterL;   //variable for the result of the tx_rx measurement.
boolean empty = true;
/*int sensorVal;
  unsigned long DELAY_TIME = 10000; // 10.00 sec
  unsigned long delayStart = 0; // the time the delay started
  bool delayRunning = false; // true if still waiting for delay to finish
  bool pumpOn = false; // keep track of the led state
*/
void setup() {
  // =====SENSORS======
  dht.begin();
  pinMode(4, OUTPUT);

  //=====ACTUATORS=====
  tempLEDs.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  tempLEDs.show();            // Turn OFF all pixels ASAP
  humidLEDs.begin();
  humidLEDs.show();
  pinMode(waterPump, OUTPUT);
  tempLEDs.setBrightness(25);
  humidLEDs.setBrightness(25);

  //=====OTHER=====
  Serial.begin(9600);
  /* pumpOn = false;

    // Start Delay:
    delayStart = millis();
    delayRunning = true;
  */
}//void setup()

/** VOID LOOP
   1. check water level
   2. check humidity & temp. set LEDs
   3. check if soil needs to be watered


*/
void loop() {
  /**
     1. check WATER LEVEL in storage tank
  */
  waterL = tx_rx();
  Serial.println();
  Serial.print("Water Level: "); Serial.print(waterL);
  Serial.println();
  if (waterL < 0)
    empty = true;
  else
    empty = false;
  /**
     2. check HUMIDOTY & TEMP, set LEDs accordingly
  */
  tempHumidityRead();
  /**
     3. WATERING SOIL
  */
  if (empty == false) {
    soilMoistureValue = 0;
    for (int x = 0; x < 20; x++) {
      soilMoistureValue += analogRead(soilPin);
    }
    soilMoistureValue = soilMoistureValue / 20;
    
    
    if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
    {
      Serial.println("Soil Moisture: Very Wet");
      digitalWrite(waterPump, LOW);
    }
    else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
    {
      Serial.println("Soil Moisture: Wet");
      digitalWrite(waterPump, LOW);
    }
    else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
    {
      Serial.println("Soil Moisture: Dry");
      digitalWrite(waterPump, HIGH);
      delay(5000);
    }
    else {
      Serial.println("unknown soil moisture ");
      digitalWrite(waterPump, LOW);
    }
  }
  delay(100);

}//void loop()
/**
   measures capacitance for 100 samples and averages data to reduce noise
   @return average out inputs from capacitive measurement
*/
long tx_rx() {        //Function to execute rx_tx algorithm and return a value
  //that depends on coupling of two electrodes.
  //Value returned is a long integer.
  int read_high;
  int read_low;
  int diff;
  long int sum;
  int N_samples = 100;    //Number of samples to take.  Larger number slows it down, but reduces scatter.
  sum = 0;
  for (int i = 0; i < N_samples; i++) {
    digitalWrite(4, HIGH);             //Step the voltage high on conductor 1.
    read_high = analogRead(waterLevel);        //Measure response of conductor 2.
    delayMicroseconds(100);            //Delay to reach steady state.
    digitalWrite(4, LOW);              //Step the voltage to zero on conductor 1.
    read_low = analogRead(waterLevel);         //Measure response of conductor 2.
    diff = read_high - read_low;       //desired answer is the difference between high and low.
    sum += diff;                       //Sums up N_samples of these measurements.
  }//for (int i = 0; i < N_samples; i++)
  return sum / N_samples;
}                         //End of tx_rx function.
/**
   reads humidity and temp then sets variables
*/
void tempHumidityRead() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }//if any values are null
  else {
    temperature = (int)t;
    humidity = (int)h;
    int rcolor = random (0, 255);
    int gcolor = random (0, 125);
    int bcolor = random (0, 255);
    //TEMPERATURE
    if (temperature >= 0) {
      for (int i = 0; i < (temperature / 10); i++) {
        tempLEDs.setPixelColor(i, tempLEDs.Color(rcolor, gcolor, 0));
      }//for each led
      tempLEDs.show();
    }//temperature>0
    else {
      for (int i = 0; i < (abs(temperature / 10)); i++) {
        tempLEDs.setPixelColor(i, tempLEDs.Color(rcolor, gcolor, 0));
      }//for each LED
      tempLEDs.show();
    }//temperature<0
    //HUMIDITY
    for (int i = 0; i < (humidity / 20) - 1; i++) {
      humidLEDs.setPixelColor(i, humidLEDs.Color(rcolor, gcolor, bcolor));
    }//for each LED
    humidLEDs.show();
  }//values not null


  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));


}
