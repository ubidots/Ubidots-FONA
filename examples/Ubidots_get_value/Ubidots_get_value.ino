#include <Ubidots_FONA.h>


#include <SoftwareSerial.h> 
#include <Adafruit_FONA.h>


#define APN "Your_apn_of_your_SIM_here" 
#define USER "Your_username_here"  // if your apn doesnt have username just put ""
#define PASS "Your_password_here"  // if your apn doesnt have password just put ""
#define TOKEN "Your_token_here"  // Remplace it with your token
#define ID "Your_id_here" // Remplace it with your variable ID

Ubidots client(TOKEN);  
  
void setup() {
  Serial.begin(115200);
  delay(2000);
  client.gprsNetwork(F(APN),F(USER),F(PASS));
}

void loop() {
  client.flushSerial();
  client.checkFona();
  client.gprsOnFona();
  float value = client.get_value(ID);  
  delay(600);  // 600 milliseconds 
  Serial.println(value);
}