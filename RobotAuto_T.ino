/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

#include <driver/adc.h>
#include "elapsedMillis.h"

#define ESPOK 0

#define SENDINTERVALL 20

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0x48, 0x3F, 0xDA, 0xA4, 0x36, 0x57};
uint8_t broadcastAddress2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress3[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct canal_struct 
{
  uint16_t lx;
  uint16_t ly;
  uint16_t rx;
  uint16_t ry;
 

  int x;
  int y;
} canal_struct;

//Create a struct_message called canaldata
canal_struct canaldata;

elapsedMillis sendtimer;



esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  //Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.print(macStr);
  //Serial.print(" send status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

 
void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  /// register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
   pinMode(5,INPUT);
   pinMode(8,INPUT);
   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
   adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_0);
  
}
 uint8_t loopstatus = 0;
 
void loop() {
  canaldata.x = adc1_get_raw(ADC1_CHANNEL_0);
  canaldata.y = adc1_get_raw(ADC1_CHANNEL_3);
  canaldata.lx = adc1_get_raw(ADC1_CHANNEL_0);
  canaldata.ly = adc1_get_raw(ADC1_CHANNEL_3);

  
 //Serial.print(canaldata.lx);
 // Serial.print(" ");
 // Serial.println(canaldata.ly);
  //if(loopstatus & (1<<ESPOK))
  if (sendtimer > SENDINTERVALL)
  {
    sendtimer = 0;
    esp_err_t result = esp_now_send(0, (uint8_t *) &canaldata, sizeof(canaldata));
  //Serial.print("result: ");
   //Serial.println(result);

  }
 // delay(50);
}
