#include "nrf.h"
NRF nrf;


#define NODE_ID 1
void setup() {
    Serial.begin(115200);
    nrf.setup(NODE_ID, false);
}

void loop() {
    nrf.loop();
}
