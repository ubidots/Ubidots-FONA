
#include <Ubidots_FONA.h>
#include <SoftwareSerial.h>


#define APN  ""  // The APN of your operator
#define USER ""  // if your apn doesnt have username just leave it ""
#define PASS ""  // if your apn doesnt have password just leave it ""
#define TOKEN "Your_Token_Here"  // Replace it with your Ubidots token

Ubidots client(TOKEN);

void setup() {
  Serial.begin(115200);
  delay(2000);
  client.setApn(APN, USER, PASS);
}
void loop() {
    float value1 = analogRead(A0);
    float value2 = analogRead(A1);
    char context[25];
    char context_2[45];
    sprintf(context, "lat=%s$lng=%s", char(1.1232), char(4323.123));
    // To send latitude and longitude to Ubidots and see it in a map widget
    // The format of the context is like this, you must send it like this example
    sprintf(context_2, "Temperature=Value_Of_Temperature$Position=Value_Of_Position$Color=Value_Of_Color");
    // You can send multiple context in one variable, to send it you must add a "$" symbol between every context
    client.add("Variable_Name_One", value1, context);  // Change for your variable name
    client.add("Variable_Name_Two", value2,  context_2);
    client.sendAll();
    delay(5000);
}