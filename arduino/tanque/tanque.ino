#include <AddrList.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
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
    pinMode(D1, OUTPUT);
    digitalWrite(D1, HIGH);
    Serial.println("sleeping 5s");

    // during this period, a simple amp meter shows
    // an average of 20mA with a Wemos D1 mini
    // a DSO is needed to check #2111
    delay(5000);

    Serial.print("Reason wake up: ");
    Serial.println(ESP.getResetReason());
    Serial.println(ESP.getResetReason() == "Power on");

    Serial.println("waking WiFi up, sleeping 5s");
    WiFi.forceSleepWake();

    // amp meter raises to 75mA
    delay(5000);

    Serial.print("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");

    digitalWrite(D1, LOW);

    ESP.deepSleep(5e6);
}

void loop() { server.handleClient(); }
