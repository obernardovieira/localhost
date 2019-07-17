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
// webserver
ESP8266WebServer server(80);

// define pins
const int ledPinOn = D1;
const int ledPinHasConnections = D2;

// default interval is one second (should load from memory)
int sleepInterval = 1;

// wait for stations to connect
static void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected& event)
{
    digitalWrite(ledPinHasConnections, HIGH);
}
static WiFiEventHandler onSoftAPModeStationConnectedHandler;

/*
 * Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
    // TODO: create form to change inteval
    server.send(200, "text/html", "<h1>You are connected</h1><br />Use /config");
}

// handle /config server call
void handleConfig() {
    if (server.hasArg("interval")) {
        // change led state according to value sent
        bool change_to = server.arg("led") == "1";
        // TODO: write to memory
        // response from server
        server.send(200, "text/plain", "updated!");
    } else {
        server.send(400, "text/plain", "missing parameter!");
    }
}
// handle /finish server call
void handleFinish() {
    digitalWrite(ledPinOn, LOW);
    digitalWrite(ledPinHasConnections, HIGH);
    server.send(200, "text/plain", "finishing...");
    ESP.deepSleep(interval * 1000000);
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
        server.on("/config", HTTP_GET, handleConfig);
        server.on("/finish", HTTP_GET, handleFinish);
        server.begin();
        digitalWrite(ledPinOn, HIGH);
        // start waiting for connections
        onSoftAPModeStationConnectedHandler = WiFi.onSoftAPModeStationConnected(onSoftAPModeStationConnected);
    } else if (ESP.getResetReason() == "Deep-Sleep Wake") {
        // TODO: do the work
        // load interval from memory
    }
}

void loop() { server.handleClient(); }
