#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"
#define ssid      "Internet_Waifu_2.4"       // WiFi SSID
#define password  "3219607277"  // WiFi password

const int BUFFER_SIZE = 16;
char buf[BUFFER_SIZE];
ESP8266WebServer server(80);

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return ( (val/10*16) + (val%10) );
}

void handleRoot(){
  String s = webpage;
  server.send(200, "text/html", s);
}
void charToString(char S[], String &D){
  String rc(S);
  D = rc;
}

void update_page_time(){
  //memset(buf, 0, BUFFER_SIZE);
  uint8_t hey[] = "tik-tok\n";
  Serial.write(hey,sizeof(hey));
  if (Serial.available() > 0) {
    Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
    String time;
    charToString(buf, time);
    server.send(200, "text/plain", time);
  }
  else{
  server.send(200, "text/plain", "No Clock Available");
  }
}

void reset_clock(){
  Serial.println("Reset");
    digitalWrite(D6, LOW);
    delay(100);
    digitalWrite(D6, HIGH);
}

void toggle_clock(){
  uint8_t TOG[5] = {'T','E','O','M','\r'};
  Serial.write(TOG,sizeof(TOG));
}

uint16_t servo_angle_16(float ang){
  float min_ang = 0.0;
  float max_ang = 180.0;
  float srv_mx = 500.0;
  float srv_mn = 110;
  float Value;
	if(ang < min_ang) ang = min_ang;
	if(ang > max_ang) ang = max_ang;
  Value = (ang  - min_ang) * (srv_mx - srv_mn) / (max_ang - min_ang) + srv_mn;
  return Value;
}

//Set specific digit
void st(){
uint8_t header;
switch(server.arg(0).toInt()){
  case 0:
    header = 'H';
    break;
  case 1:
    header = 'M';
    break;
  case 2:
    header = 'S';
    break;
  default:
      return;
}
uint8_t br[6] = {header,decToBcd(server.arg(1).toInt()),'E','O','M','\r'};
Serial.write(br,sizeof(br));
}

//Set all digits
void sa(){
uint8_t br[8] = {'A',decToBcd(server.arg(0).toInt()),decToBcd(server.arg(1).toInt()),decToBcd(server.arg(2).toInt()),'E','O','M','\r'};
Serial.write(br,sizeof(br));
}

#define a2i(x) (server.arg(x).toInt())

void ss(){
uint8_t a[6];
for(int i  = 0; i < 4; i++){
  a[i] = (byte)a2i(i);
}
uint16_t ang = (uint16_t)a2i(4);
a[4] = (uint8_t)(ang>>8);
a[5] = (ang&0xff);
uint8_t br[11] = {'Z',a[0],a[1],a[2],a[3],a[4],a[5],'E','O','M','\r'};
Serial.write(br,sizeof(br));
}


void setup(){
  pinMode(D6, OUTPUT);
  digitalWrite(D6, HIGH);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay ( 500 ); 
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.on("/", handleRoot);
  server.on("/get_time", update_page_time);
  server.on("/set_time", st);
  server.on("/set_all", sa);
  server.on("/reset", reset_clock);
  server.on("/set_servo", ss);
  server.on("/toggle", toggle_clock);
  server.begin();
}

void loop(){
  server.handleClient();
}
