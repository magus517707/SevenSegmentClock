#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"
#define ssid      ""       // WiFi SSID
#define password  ""  // WiFi password
#define a2i(x) (server.arg(x).toInt())

const int BUFFER_SIZE = 16;
char buf[BUFFER_SIZE];
ESP8266WebServer server(80);

// Convert normal decimal numbers to binary coded decimal
//STM32 RTC uses BCD values
byte decToBcd(byte val){
  return ( (val/10*16) + (val%10) );
}

void charToString(char S[], String &D){
  String rc(S);
  D = rc;
}

//Returns webpage in index.h when there is a server request
void handleRoot(){
  String s = webpage;
  server.send(200, "text/html", s);
}

//Queries the Nucleo for the current time and then return this value to the webpage
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

//Toggles the reset pin on the Nucleo Board. Have an issue with the board freezing on powerup
void reset_clock(){
  Serial.println("Reset");
    digitalWrite(D6, LOW);
    delay(100);
    digitalWrite(D6, HIGH);
}

// Indicates to the Nucleo to set the clock digits to the disabled state
void toggle_clock(){
  uint8_t TOG[5] = {'T','E','O','M','\r'};
  Serial.write(TOG,sizeof(TOG));
}

// No longer used, this math is done on the client side via JS
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

//Set specific digit value
void st(){
uint8_t header;
  switch(a2i(0)){
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
uint8_t br[6] = {header,decToBcd(a2i(1)),'E','O','M','\r'};
Serial.write(br,sizeof(br));
}

//Set all digits at the same time
void sa(){
uint8_t br[8] = {'A',decToBcd(a2i(0)),decToBcd(a2i(1)),decToBcd(a2i(2)),'E','O','M','\r'};
Serial.write(br,sizeof(br));
}

//Send encoded list of Servos to adjust and the angle they should be adjusted to.
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
