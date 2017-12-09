#include <SoftwareSerial.h>
/*----wifi stuff----*/
#define ssid      "dlink"          //wifi name
#define password  "Plotemy1"       //wifi password
/*----thingspeak stuff----*/
//#define ipaddress "thingspeak.com" //184.106.153.149
#define ipaddress "184.106.153.149"
#define port       80              //HTTP port

SoftwareSerial esp(9, 10);         //RX, TX

String GET = "GET /update?api_key=OG37ELFGSAWRDBES";//update?api_key=OG37ELFGSAWRDBES
float value = 100;
String data;


void setup() 
{
  Serial.begin(115200);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  esp.begin(115200);
  Serial.println("Starting");
  send_command("AT"); // check response
  delay(10);
  connect_wifi();
  //  esp.println();
  send_command("AT+CIFSR");
//  send_data(value);
}

void loop() 
{
//  if (esp.available()) {
//    Serial.write(esp.read());
//  }
//  if (Serial.available()) {
//    esp.write(Serial.read());
//  }
  Serial.println(value);
  send_data(value);
  value++;
  delay(2000);
}

void send_command(String cmd) 
{
  Serial.println(cmd);
  //cmd += "\r\n";
  //  esp.print(Serial.read());
  esp.println(cmd);
  if (esp.available()) 
  {
    delay(100);
    Serial.write(esp.read());
  }
  //   delay(10);
}

boolean connect_wifi() 
{
  esp.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";
  send_command(cmd);
  delay(5000);
  if (esp.find("OK")) 
  {
    Serial.println("Connected");
    return true;
  }
  else
  {
    Serial.println("Could not connect");
    return false;
  }
}

void send_data(int value) 
{
  data = String (value);
  String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
  cmd += ipaddress;
  cmd += "\",";
  cmd += port;
  send_command(cmd);
  delay(2000);
  if (esp.find("ERROR")) 
  {
    return;
  }
  if (esp.find("ALREADY CONNECT")) 
  {
    send_command("AT+CIPCLOSE");
  }
  String update_data = GET + "&field1=" + data + "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += update_data.length();
  send_command(cmd);
  delay(1500);
  if (esp.find(">")) 
  {
    send_command(update_data);
  }
  delay(5000);
  if (esp.find("REQUEST_TIMEOUT")) 
  {
    send_command("AT+CIPCLOSE");
  }
}

