// Heltec ESP32 Dev-Boards - Version: Latest
#include "thingProperties.h"

#include "heltec.h"
#include "images.h"

#include <DHT.h>
#include "WiFi.h"

#include <iostream>
#include <vector>

std::vector<String> screenOutput;

// void writeLine(String str, int row = 10, bool clear = true, bool writeToSerial = true);
void writeLines(std::vector<String> lines, bool clear = true, bool writeToSerial = true);

#define DHTPIN 5 // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  serialInit();

  setupDisplay();

  cloudInit();

  dht.begin();
}

void loop()
{
  screenOutput.clear();

  ArduinoCloud.update();

  random_value = random(0, 500);
  screenOutput.push_back("Random Value: " + String(random_value));

  processDht();

  writeLines(screenOutput);
  delay(1500);
}

void processDht()
{
  humidity = dht.readHumidity();
  tempCel = dht.readTemperature();
  tempFahr = dht.readTemperature(true);

  if (isnan(humidity) || isnan(tempCel || isnan(tempFahr)))
  {
    screenOutput.push_back("Failed to read from DHT sensor!");
    return;
  }

  screenOutput.push_back("Humidity: " + String(humidity) + " %");
  screenOutput.push_back("Temperature (C): " + String(tempCel) + " *C");
  screenOutput.push_back("Temperature (F): " + String(tempFahr) + " *F");
}

void onLedSwitchChange()
{
  if (led_switch)
  {
    screenOutput.push_back("Toggled: LED ON (" + String(led_switch) + ")");
  }
  else
  {
    screenOutput.push_back("Toggled: LED OFF (" + String(led_switch) + ")");
  }
}

void setupDisplay()
{
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);

  logo();
  delay(1000);

  Heltec.display->clear();
}

void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, (const unsigned char *)logo_bits);
  Heltec.display->display();
}

/**
 * @brief This function writes a vector of strings and outputs them to the OLED display
 *
 * @param vector<String> lines: The strings to be displayed on screen
 * @param bool clear: If true, the display will not be cleared before writing the lines
 * @param bool writeToSerial: If true, the lines will also be written to the serial output
 * @return void
 */
void writeLines(std::vector<String> lines, bool clear, bool writeToSerial)
{
  if (clear)
  {
    Heltec.display->clear();
  }

  int row = 10;
  for (int i = 0; i < lines.size(); i++)
  {
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, row, lines[i]);
    row += 10;

    if (writeToSerial)
    {
      Serial.println(lines[i]);
    }
  }

  Heltec.display->display();
}

void serialInit()
{
  // // Initialize serial and wait for port to open:
  // https://bit.ly/3CI7m59
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
}

void cloudInit()
{
  // !!! MUST BE CALLED BEFORE ANYTHING ELSE about cloud
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}
