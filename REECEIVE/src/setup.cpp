#include "prototypes.h"
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

void SetupESPNOW(){
  Serial.begin (115200);
  for(auto &iii: inputArray) pinMode(iii, INPUT);
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK) {
      Serial.println("ERROR: espnow");
      return;
  }

  esp_now_peer_info_t peer ={};
  memcpy(peer.peer_addr, receptorAddress, 6);
  esp_now_add_peer(&peer);
}


void SetupMacAddress (){
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK){
    std::cerr << "failed to initialize espnow" << std::endl;
    Serial.println("failed to initialize espnow");
  }

  WiFi.begin();
  Serial.print("MAC ADDRESS: ");
  Serial.println(WiFi.macAddress());
}


void LoopMacAddress(){
  delay(100);
  Serial.print("MAC ADDRESS: ");
  Serial.println(WiFi.macAddress()); 

  delay(100);
  Serial.print("AJJA MAC: ");
  Serial.println(WiFi.macAddress());
}



