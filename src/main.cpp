#include <Arduino.h>
#include <FastLED.h>

#include "settings.h"
#include <UIPEthernet.h>
// The connection_data struct needs to be defined in an external file.
#include <UIPServer.h>
#include <UIPClient.h>
#include "utility/logging.h"

uint8_t mac[6] = {MACADDRESS};
uint8_t myIP[4] = {MYIPADDR};
uint8_t myMASK[4] = {MYIPMASK};
uint8_t myDNS[4] = {MYDNS};
uint8_t myGW[4] = {MYGW};

#define SS 5

EthernetServer server = EthernetServer(LISTENPORT);

CRGB leds[LED_COUNT];

uint32_t lastMessage = 0;
uint32_t counter = 0;
uint8_t espIndex = 0;
uint32_t size = 0;

uint32_t lastClientMessage = 0;

void showStatus(CRGB led1, CRGB led2, CRGB led3, CRGB led4) {
    FastLED.clear();
    leds[0] = led1;
    leds[1] = led2;
    leds[2] = led3;
    leds[3] = led4;
    FastLED.show();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Started");
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(50);
  Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
  // start listening for clients
  server.begin();
  showStatus(CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red);
  delay(1000);
  showStatus(CRGB::Red, CRGB::Red, CRGB::Red, CRGB::White);
  delay(1000);
  showStatus(CRGB::Red, CRGB::Red, CRGB::White, CRGB::White);
  delay(1000);
  showStatus(CRGB::Red, CRGB::White, CRGB::White, CRGB::White);
  delay(1000);
  showStatus(CRGB::White, CRGB::White, CRGB::White, CRGB::White);
  delay(1000);

}

void loop() {

    if (EthernetClient client = server.available()) {
      showStatus(CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue);
      lastClientMessage = millis();
      if (client) {
          while((size = client.available()) > 0) {
              uint8_t* msg = (uint8_t*)malloc(size);
              size = client.read(msg,size);
              #if ACTLOGLEVEL>=LOG_INFO
                LogObject.uart_send_buf_len(msg,size);
              #endif

              Serial.write(msg,size);
              client.write(msg,size);
              free(msg);
            }
        }
    }
    if (millis() - lastClientMessage < 500) {
      showStatus(CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue);
    } else {
      showStatus(CRGB::White, CRGB::White, CRGB::White, CRGB::White);
    }
}