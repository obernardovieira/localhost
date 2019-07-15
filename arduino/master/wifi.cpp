#include "wifi.h"
#include "config.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

ESP8266WebServer server(80);
const int ledPin = D1;
const int sensorTemperaturePin = A0;

/**
 * get temperature in C
 */
float getTemperatureC() {
    float tmp = (analogRead(sensorTemperaturePin) * 2.9 / 1024.0) - 0.5;
    return (tmp / 0.01);
}

/**
 * handle root server call
 */
void handleRoot() {
    String message = "hello from esp8266!\n\n";
    message += "current temperature in C: ";
    message += getTemperatureC();
    //
    server.send(200, "text/plain", message);
}

/**
 * handle /config server call
 */
void handleConfig() {
    // this changes the led state, only an example
    if (server.hasArg("led")) {
        // change led state according to value sent
        bool change_to = server.arg("led") == "1";
        digitalWrite(ledPin, change_to);
        // response from server
        server.send(200, "text/plain", "updated!");
    } else {
        server.send(400, "text/plain", "missing parameter!");
    }
}

/**
 * handle not found endpoint
 */
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

/**
 * fetch config from server
 */
void fetchConfigFromServer() {
    Serial.print("connecting to ");
    Serial.println(serverHost);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(serverHost, serverPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/";

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + serverHost + "\r\n" + "Connection: close\r\n\r\n");
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
        // TODO: do something with config data
        Serial.print(line);
    }

    Serial.println("closing connection");
}

/**
 * main setup
 */
void Wifi::setup() {
    // setup led
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, 0);
    // setup temperature sensor
    pinMode(sensorTemperaturePin, INPUT);

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

    //
    fetchConfigFromServer();

    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_POST, handleConfig);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

/**
 * main loop
 */
void Wifi::loop() {
    server.handleClient();
}
