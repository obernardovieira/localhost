#include "wifi.h"

Wifi wifi;

void setup() {
    Serial.begin(115200);
    wifi.setup();
}

void loop() {
    wifi.loop();
}
