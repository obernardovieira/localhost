#include "nrf.h"
#include "wifi.h"

#define NODE_ID 0

NRF nrf;
Wifi wifi;

void setup() {
    Serial.begin(115200);
    nrf.setup(NODE_ID, false);
    wifi.setup();
}

void loop() {
    nrf.loop();
    wifi.loop();
}
