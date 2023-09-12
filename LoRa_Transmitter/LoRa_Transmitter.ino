#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TP-LINK_26619E";
const char* password = "18670691";

// Time API endpoint
const char* timeApiUrl = "http://worldtimeapi.org/api/timezone/Europe/Madrid";

// Use Hardware Serial on ESP32
HardwareSerial LoRaSerial(1);  // Using Serial1

void setup() {
  Serial.begin(115200);
  LoRaSerial.begin(115200, SERIAL_8N1, 16, 17);  // Using GPIO 16 for RX and GPIO 17 for TX

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

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
  LoRaSerial.println("AT+ADDRESS=120");
  delay(1000);

  Serial.println("Transceiver is ready to send data");
}

void loop() {
  HTTPClient http;
  http.begin(timeApiUrl);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    // Extract the time (adjust this based on the exact format of the API response)
    int timeIndex = payload.indexOf("\"datetime\":\"") + 12;
    String time = payload.substring(timeIndex, timeIndex + 19);  // Format: YYYY-MM-DDTHH:MM:SS
    String hour = time.substring(11, 19); // Extracting only the HH:MM:SS part

    // Send the time over LoRa
    LoRaSerial.print("AT+SEND=0,");
    LoRaSerial.print(hour.length());
    LoRaSerial.print(",");
    LoRaSerial.println(hour);

    Serial.println("Sent time: " + hour);
  }
  http.end();

  delay(10000);  // Fetch and send the time every 1 minute
}
