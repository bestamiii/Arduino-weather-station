#include <Wire.h>
#include <RTClib.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <LiquidCrystal_I2C.h>

#include <SPI.h>    //Bibliothek für die Kommnikation mit der SD-Karte
#include <SD.h>     //Bibliothek für die SD-Karte

File Textdatei;   
#define SD_CS_PIN 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
RTC_DS1307 rtc;

LiquidCrystal_I2C lcd(0x27, 20, 4); // Ensure this matches your LCD's I2C address and size

float sensorValues[3];

void setup() {
  Serial.begin(9600); // Serial communication for debugging
  //Wire.begin();       // Initialize I2C
  
  Serial.print("Initialisierung der SD-Karte...");

if (!SD.begin(SD_CS_PIN)) {
  Serial.println("SD-Karte Initialisierung fehlgeschlagen!");
  return;
}
  Serial.println("SD-Karte erfolgreich initialisiert.");

  // Initialize BME280
  if (!bme.begin(0x76)) { // The common address for BME280
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1); // Stay here if initialization fails
  }

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC. Check wiring!");
    while (1); // Stay here if initialization fails
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running! Setting time.");
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the current time
  }

  // Initialize the LCD
  lcd.init();
  lcd.backlight(); // Turn on the LCD backlight
  lcd.clear();     // Clear the LCD screen

//Zeit manuell einstellen (Jahr, Monat, Tag, Stunde, Minute, Sekunde)
//rtc.adjust(DateTime(2024, 6, 17, 10, 12, 20));

}

void messwerte(){
   DateTime jetzt = rtc.now(); 
  sensorValues[0] = bme.readPressure() / 100.0F; 
  sensorValues[1] = bme.readTemperature(); 
  sensorValues[2] = bme.readHumidity(); 



  char Datum[] = "DD.MM.YYYY";
  char Uhrzeit[] = "hh:mm:ss";

  
  Serial.println("Sensorwerte mit Zeitstempel:");
  
  Serial.print("Luftdruck (hPa): ");
  Serial.println(sensorValues[0]);
  Serial.print("Temperatur (°C): ");
  Serial.println(sensorValues[1]);
  Serial.print("Luftfeuchtigkeit (%): ");
  Serial.println(sensorValues[2]);
  Serial.println();
  
  Serial.print("Datum und Uhrzeit: ");
  Serial.println();
  Serial.print(jetzt.toString(Datum));
  Serial.println();
  Serial.print(jetzt.toString(Uhrzeit));

  Serial.println();
  delay(1000);
}

void schreibenSD(){
DateTime jetzt = rtc.now(); 
  
char Datum[] = "DD.MM.YYYY";
 char Uhrzeit[] = "hh:mm:ss";
 

  
  Textdatei = SD.open("Daten.txt", FILE_WRITE);  // Textdatei namens "DATEN.TXT" wird erstellt und geöffnet.
  
   Textdatei.println(millis());
  Textdatei.println("Sensorwerte mit Zeitstempel:");
  
  Textdatei.print("Luftdruck (hPa): ");
  Textdatei.println(sensorValues[0]);
  Textdatei.print("Temperatur (°C): ");
  Textdatei.println(sensorValues[1]);
  Textdatei.print("Luftfeuchtigkeit (%): ");
  Textdatei.println(sensorValues[2]);
  Textdatei.println();
  
  Textdatei.print("Datum und Uhrzeit: ");
  Textdatei.println();
Textdatei.print(jetzt.toString(Datum));
Textdatei.println();
Textdatei.print(jetzt.toString(Uhrzeit));

  Textdatei.println();

  //Daten zum Speichern
delay(1000);

  Textdatei.close();                // Anschließend wird die Textdatei wieder geschlossen...
 // ... und eine erneute Meldung im seriellen Monitor ausgegeben.
}


void loop() {
  // Get the current time from RTC
   DateTime jetzt = rtc.now(); 

  // Read the BME280 sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Pressure in hPa


messwerte();
schreibenSD();

  // Display date and time on the first row
  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(jetzt.day()); 
  lcd.print("/");
  lcd.print(jetzt.month());
  lcd.print("/");
  lcd.print(jetzt.year());

  // Display current time on the second row
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(jetzt.hour());
  lcd.print(":");
  if (jetzt.minute() < 10) {
   lcd.print("0"); // Add leading zero for single-digit minutes
  }
  lcd.print(jetzt.minute());
  lcd.print(":");
  lcd.print(jetzt.second());

  // Display temperature on the third row
  lcd.setCursor(0, 2);
  lcd.print("Temp: ");
  lcd.print(temperature, 1); // Display one decimal place
  lcd.print(" C");

  // Display humidity and pressure on the fourth row
  lcd.setCursor(0, 3);
  lcd.print("H:");
  lcd.print(humidity, 1); // Display one decimal place for humidity
  lcd.print("%");

  lcd.print("Pres:");
  lcd.print(pressure, 1); // Display one decimal place for pressure
  lcd.print("hPa");

  delay(1000); // Update every second
}
