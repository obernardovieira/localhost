#include <AddrList.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "thereisnospoon"
#endif

// Set these to your desired credentials.
const char *ssid = APSSID;
const char *password = APPSK;

// define pins
const int ledPinOn = D1;
const int ledPinHasConnections = D2;

ESP8266WebServer server(80);

static void onStationModeConnected(const WiFiEventSoftAPModeStationConnected& event)
{
    Serial.print("conectado");
    digitalWrite(ledPinHasConnections, HIGH);
}

static WiFiEventHandler onStationModeConnectedHandler;

/*
 * Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
    // TODO: create form to change inteval
    server.send(200, "text/html", "<h1>You are connected</h1><br />Use /config");
}

// preinit() is called before system startup
// from nonos-sdk's user entry point user_init()

void preinit() {
    // Global WiFi constructors are not called yet
    // (global class instances like WiFi, Serial... are not yet initialized)..
    // No global object methods or C++ exceptions can be called in here!
    // The below is a static class method, which is similar to a function, so
    // it's ok.
    ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
    Serial.begin(115200);
    // setup led indicators
    pinMode(ledPinOn, OUTPUT);
    pinMode(ledPinHasConnections, OUTPUT);
    digitalWrite(ledPinOn, LOW);
    digitalWrite(ledPinHasConnections, LOW);

    // wait a bit
    delay(2000);

    /**
     * if the reset reasons was the power on or the external reset
     * then start wifi and wait for connections, otherwise
     * do the work without turning the wifi on
     */
    if (ESP.getResetReason() == "Power on" || ESP.getResetReason() == "External System") {
        WiFi.forceSleepWake();
        WiFi.softAP(ssid, password);
        server.on("/", handleRoot);
        server.begin();
        digitalWrite(ledPinOn, HIGH);
        onStationModeConnectedHandler = WiFi.onSoftAPModeStationConnected(onStationModeConnected);
        // start waiting for connections
    } else if (ESP.getResetReason() == "Deep-Sleep Wake") {
        // TODO: do the work
    }
}

void loop() { server.handleClient(); }
