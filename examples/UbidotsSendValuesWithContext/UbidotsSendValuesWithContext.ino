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
    float value3 = analogRead(A2);
    char context[25];
    char context_2[25];
    char context_3[45];
    sprintf(context, "lat=1.2343$lng=132.1233");
    // To send latitude and longitude to Ubidots and see it in a map widget
    sprintf(context_2, "Name_Of_Context=Value_Of_Context");
    // The format of the context is like this, you must send it like this example
    sprintf(context_3, "Name_Of_Context=Value_Of_Context$Name_Of_Context_2=Value_Of_Context_2$Name_Of_Context_3=Value_Of_Context_3");
    // You can send multiple context in one variable, to send it you must add a "$" symbol between every context
    ubidots.add("Variable_Name_One", value1, context);  // Change for your variable name
    ubidots.add("Variable_Name_Two", value2, context_2);
    ubidots.add("Variable_Name_Three", value3,  context_3);
    ubidots.sendAll();
    delay(5000);
}
