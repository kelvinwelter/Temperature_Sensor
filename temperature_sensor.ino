/**
  **************************************************************************************************
  * @file    temperature_sensor.ino
  * @author  Edson Jeske / Gridya Tecnologia
  * @brief   NANOFOX IoT Kit - Smoke Sensor Application Example. 
  * 
  *     - Every 15 minutes sends temperature and humidity values. 
  *           
  * License: Public Domain
  **************************************************************************************************
  */


/*-----HEADER FILES--------------------------------------------------------------------------------*/
#include <Nanofox.h>   //Nanofox Library
#include "Adafruit_HTU21DF.h"


/*------Global Variables---------------------------------------------------------------------------*/
unsigned long timeref_ms;           //Reference time for delay calculations
unsigned long timeref_per_ms;         //Reference time for delay calculations

float     temperature;
float     humidity;

uint8_t Uplink_Buffer[12];    //Buffer for uplink Payload
uint8_t Downlink_Buffer[8];   //Buffer for Downlink Payload

uint16_t *Pointer_16; //16 bits - Pointer

/*------Objects -----------------------------------------------------------------------------------*/
Nanofox MyNanofox;    //Nanofox Object Instance
Adafruit_HTU21DF htu; //Temperature and Humidity sensor Object Instance
/*-------------------------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(9600);   //Init Debug serial port
  htu.begin();          //Init sensor HTU21DF 
  MyNanofox.Init_ArduinoNano_IO();  //Setup arduino Nano IO
  MyNanofox.Init_Modem_WISOL(RC2);  //Initialize WISOL Sigfox Modem
  Serial.println("Temperature and humidty sensor example with NANOFOX IoT Kit!");
  Serial.println("");
  timeref_ms = millis();    // Init time reference 
  timeref_per_ms = millis();  // Init time reference 
}

void loop() {
  delay(10);
  //Real time monitoring
  if((millis() - timeref_ms) > 1000) { 
    timeref_ms = millis(); //restart time reference
    Read_Sensor();
  }
  
  //Periodically send data to Sigfox
  if((millis() - timeref_per_ms) > ((60000)*15)){ 
    Serial.println("Enviando dados");
    timeref_per_ms = millis(); //restart time reference
    Build_Uplink_Msg();
    MyNanofox.Send_Payload_Sigfox(&Uplink_Buffer[0],4,&Downlink_Buffer[0],0);
  }
}

void Build_Uplink_Msg(void){
  //Converting to integer format and loading Uplink Sigfox Buffer
  Pointer_16 = (uint16_t*)&Uplink_Buffer[0];
  *Pointer_16 = (uint16_t)(temperature*100);
  Pointer_16++;
  *Pointer_16 = (uint16_t)(humidity*100);
}

void Read_Sensor(void){
    temperature = htu.readTemperature();
    humidity = htu.readHumidity();
    Serial.print("Temperature: ");
    Serial.println(String(temperature) + " C ");
    Serial.print("Humidity: ");
    Serial.println(String(humidity) + " % ");
}
