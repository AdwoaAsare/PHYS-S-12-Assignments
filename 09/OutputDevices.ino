#include <Adafruit_NeoPixel.h>

#define PIN        6 
#define NUMPIXELS 10 
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int tempSensor = 0;

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels first
  strip.setBrightness(50); // Set BRIGHTNESS low to reduce draw (max = 255)

}//void setup()

void loop() {
  // put your main code here, to run repeatedly:
  strip.clear();

  //getting the voltage reading from the temperature sensor
 int reading = analogRead(tempSensor);  
 
 // multiply by 5v to convert that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 Serial.print(voltage); Serial.println(" volts");
 
 // Celcius temperature
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree with 500 mV offset to degrees ((voltage - 500mV) times 100)
 Serial.print(temperatureC); Serial.println(" degrees C");
 
 //Fahrenheit
 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 Serial.print(temperatureF); Serial.println(" degrees F");

 int color = (int) map(temperatureF, 60, 100, 0, 255);
 Serial.print("Color Val: "); Serial.println(color);
 for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // strip.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    strip.setPixelColor(i, color, color/2, 255-color);

    strip.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }//for(int i=0; i<NUMPIXELS; i++)
 
 delay(1000); //wait one second before next reading              

}//void loop()
