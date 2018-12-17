#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>
#include "nrf.h"

#define CE_PIN	D2
#define CS_PIN	D4

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
	if(this->isMaster) {
		mesh.DHCP();
	}

	// Check for incoming data
	if(network.available()){
		RF24NetworkHeader header;
		network.peek(header);

		uint32_t dat=0;
		switch(header.type){
			case 'M': network.read(header,&dat,sizeof(dat)); Serial.println(dat); break;
			default: network.read(header,0,0); Serial.println(header.type);break;
		}
	}
}
