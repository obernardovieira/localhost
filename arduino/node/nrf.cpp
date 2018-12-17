#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>
#include "nrf.h"

#define CE_PIN	9
#define CS_PIN	10

RF24 radio(CE_PIN, CS_PIN);
RF24Network network(radio);
RF24Mesh mesh(radio,network);


void NRF::setup(uint32_t nodeId, bool isMaster) {
	mesh.setNodeID(nodeId);
	mesh.begin();
	this->isMaster = isMaster;
}

void NRF::loop() {
	mesh.update();

	// Send to the master node every second
	if (millis() - displayTimer >= 1000) {
		displayTimer = millis();
		if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer))) {
			if ( ! mesh.checkConnection() ) {
				Serial.println("Renewing Address");
				mesh.renewAddress();
			} else {
				Serial.println("Send fail, Test OK");
			}
		} else {
			Serial.print("Send OK: "); Serial.println(displayTimer);
		}
	}
}
