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
int sleepIntervalSet = false;
// Structure which will be stored in RTC memory.
// First field is CRC32, which is calculated based on the
// rest of structure contents.
// Any fields can go after CRC32.
// We use byte array as an example.
struct {
    uint32_t crc32;
    uint32_t interval;
} rtcData;

// wait for stations to connect
static void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected &event) {
    digitalWrite(ledPinHasConnections, HIGH);
}
static WiFiEventHandler onSoftAPModeStationConnectedHandler;

uint32_t calculateCRC32(const uint32_t *data, size_t length) {
    uint32_t crc = 0xffffffff;
    while (length--) {
        uint32_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1) {
            bool bit = crc & 0x80000000;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= 0x04c11db7;
            }
        }
    }
    return crc;
}

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
        // TODO: write to memory
        // Generate new data set for the struct
        rtcData.interval = (uint32_t)server.arg("interval").toInt();
        // Update CRC32 of data
        rtcData.crc32 = calculateCRC32((uint32_t *)&rtcData.interval, sizeof(rtcData.interval));
        // Write struct to RTC memory
        if (ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData))) {
            Serial.println("Write: ");
            Serial.println("rtcData.interval");
            Serial.println(rtcData.interval);
        }
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
    ESP.deepSleep(rtcData.interval * 1000000);
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

    // Read struct from RTC memory
    if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData))) {
        Serial.println("Read: ");
        Serial.println();
        uint32_t crcOfData = calculateCRC32((uint32_t *)&rtcData.interval, sizeof(rtcData.interval));
        Serial.print("CRC32 of data: ");
        Serial.println(crcOfData, HEX);
        Serial.print("CRC32 read from RTC: ");
        Serial.println(rtcData.crc32, HEX);
        if (crcOfData != rtcData.crc32) {
            Serial.println("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
            rtcData.interval = 5;
        } else {
            Serial.println("CRC32 check ok, data is probably valid.");
            sleepIntervalSet = true;
        }
        Serial.println("rtcData.interval");
        Serial.println(rtcData.interval);
    }

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
        ESP.deepSleep(rtcData.interval * 1000000);
    }
}

void loop() { server.handleClient(); }
