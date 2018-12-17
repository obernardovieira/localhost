#include <ESP8266WiFi.h>
#include "wifi.h"

const char* ssid     = "DLink-EFDFA9";
const char* password = ".......";
const char* host = "192.168.x.x";

void Wifi::setup() {
	delay(10);
	Serial.print("Connecting to ");
	Serial.println(ssid);

	/*
	 * Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
	 * would try to act as both a client and an access-point and could cause
	 * network-issues with your other WiFi-devices on your WiFi-network.
	 */
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void Wifi::loop() {
	delay(5000);

	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}

	// We now create a URI for the request
	String url = "/";

	Serial.print("Requesting URL: ");
	Serial.println(url);

	// This will send the request to the server
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
			"Host: " + host + "\r\n" +
			"Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			return;
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	while (client.available()) {
		String line = client.readStringUntil('\r');
		Serial.print(line);
	}

	Serial.println();
	Serial.println("closing connection");
}
