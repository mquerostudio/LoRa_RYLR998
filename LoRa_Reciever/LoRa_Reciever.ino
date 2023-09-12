#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_MOSI 33
#define TFT_CLK 26
#define TFT_CS 25
#define TFT_DC 27
#define TFT_RST 32

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Use Hardware Serial on ESP32
HardwareSerial LoRaSerial(1);  // Using Serial1

void setup() {
  Serial.begin(115200);
  LoRaSerial.begin(115200, SERIAL_8N1, 12, 14);  // Using GPIO 12 for RX and GPIO 14 for TX

  // Initialize TFT
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println("Waiting for data...");

  // Set the frequency
  LoRaSerial.println("AT+BAND=868500000");
  delay(1000);

  // Set the RF parameters (example values)
  LoRaSerial.println("AT+PARAMETER=7,7,1,12");
  delay(1000);

  // Set the network ID
  LoRaSerial.println("AT+NETWORKID=6");
  delay(1000);

  // Set the address for the receiver
  LoRaSerial.println("AT+ADDRESS=121");
  delay(1000);

  Serial.println("Receiver is initialized and ready");
}

void loop() {
  while (LoRaSerial.available()) {
    String receivedData = LoRaSerial.readStringUntil('\n');
    if (receivedData.startsWith("+RCV=")) {
      receivedData = receivedData.substring(5);  // Remove the "+RCV=" prefix
      int commaIndex = receivedData.indexOf(',');
      String address = receivedData.substring(0, commaIndex);

      receivedData = receivedData.substring(commaIndex + 1);
      commaIndex = receivedData.indexOf(',');
      String length = receivedData.substring(0, commaIndex);

      receivedData = receivedData.substring(commaIndex + 1);
      commaIndex = receivedData.indexOf(',');
      String data = receivedData.substring(0, commaIndex);

      receivedData = receivedData.substring(commaIndex + 1);
      commaIndex = receivedData.indexOf(',');
      String rssi = receivedData.substring(0, commaIndex);

      String snr = receivedData.substring(commaIndex + 1);

      Serial.println("Address: " + address);
      Serial.println("Length: " + length);
      Serial.println("Data: " + data);
      Serial.println("RSSI: " + rssi);
      Serial.println("SNR: " + snr);

      // Display the received data on the TFT
      tft.fillScreen(ST77XX_BLACK);   // Clear the screen
      tft.setCursor(10, 10);
      tft.println("Data:");
      tft.setCursor(10, 30);
      tft.println(data);
      
    } else {
      Serial.println("Other message: " + receivedData);
    }
  }
}
