#include <tfa433.h>

TFA433 tfa = TFA433();

void setup() {
  Serial.begin(115200);
  tfa.start(2); //Input pin where 433 receiver is connected.
}

void loop() {
  if (tfa.isDataAvailable()) {
    // Pointer results:
    // byte type = 0, id = 0, channel = 0, humidity = 0;
    // bool battery = false;
    // float temperature = 0;
    // tfa.getData(type, id, battery, channel, temperature, humidity);
    // char temperatureStr[5];
    // dtostrf(temperature, 0, 1, temperatureStr);
	  // char txt[100];
    // sprintf(txt, "type: %d, id: %d, battery: %s, channel: %d, temperature: %s C, humidity: %d%%", type, id, (battery ? "OK" : "NOK"), channel, temperatureStr, humidity);
    // Serial.println(txt);
    // Struct results:
    tfaResult result = tfa.getData();
    char temperatureStr[5];
    dtostrf(result.temperature, 0, 1, temperatureStr);
	  char txt[100];
    sprintf(txt, "type: %d, id: %d, battery: %s, channel: %d, temperature: %s C, humidity: %d%%", result.type, result.id, (result.battery ? "OK" : "NOK"), result.channel, temperatureStr, result.humidity);
    Serial.println(txt);
  }
}
