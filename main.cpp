
//  Based on Neil Kolban's example file: https://github.com/nkolban/ESP32_BLE_Arduino
 
#include <Arduino.h>
#include "BLEDevice.h"
static BLEAddress *pServerAddress;

#define RELAIS 26                    
#define uS_TO_S_FACTOR 1000000        /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_SHORT  5        /* Time ESP32 will go to sleep (in seconds) */
#define TIME_TO_SLEEP_LONG  180       /* Time ESP32 will go to sleep (in seconds) */
BLEScan* pBLEScan;
BLEClient*  pClient;
bool deviceFound = false;
String knownAddresses[] = { "7c:2f:80:ad:cb:ae", "  :  :  :  :  :  "};
//                                            Espace ajoute pour tester la consommation..
//                                            Le Gigaset n est jamais detecte....
unsigned long entry;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
*         Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());

      bool known = false;
      for (int i = 0; i < (sizeof(knownAddresses) / sizeof(knownAddresses[0])); i++) {
        if (strcmp(pServerAddress->toString().c_str(), knownAddresses[i].c_str()) == 0) known = true;
      }
      if (known) {
        if (advertisedDevice.getRSSI() > -200) deviceFound = true;
        else deviceFound = false;
        advertisedDevice.getScan()->stop();
      }
    }
}; 

void setup()
{ pinMode(RELAIS, OUTPUT);
  digitalWrite(RELAIS, 1); // Logique négative pour le relais !!!
  BLEDevice::init("");
  pClient  = BLEDevice::createClient();
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  deviceFound = false;
  BLEScanResults scanResults = pBLEScan->start(2);
  
  if (deviceFound) {
    digitalWrite(RELAIS, 0); // Logique négative pour le relais !!!
    delay(1000);
    digitalWrite(RELAIS, 1);// Logique négative pour le relais !!!
    //Sleep 3 minutes.
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_LONG * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  }
  else {
  //slepp 5 secondes
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_SHORT * uS_TO_S_FACTOR); 
  esp_deep_sleep_start();
    } 
}
void loop() {
  }
