#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial GPS_A7(D7, D8); // RX, TX
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
int pm =0;
String temp_nmea = "";
String temp_nmea2 = "";



String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};


// Replace with your network credentials
const char* ssid = "duy's iPhone";
const char* password = "38220839";
const char* host = "evn-monitor.herokuapp.com";
const int httpsPort = 443;
String request_raw_1 = "{\"utcDateTime\": \"";
String request_raw_2 = "\",\"latitude\": " ;
String request_raw_3 = ",\"longitude\": ";
String request_raw_4 = ",\"no\": 45,\"so2\": 43,\"pm\": 100,\"o3\": 3423,\"sound\": 10";
String request="";
int number;
// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";
const char* fingerprint = "F5:69:8C:CA:29:68:5E:47:26:38:C5:1A:18:F1:8A:6A:EA:60:56:4D";
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  GPS_A7.begin(9600);

  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet, dns1, dns2);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("connecting to ");
  Serial.println(host);

  
}

void loop() {
  Serial.flush();
  GPS_A7.flush();
  while (GPS_A7.available() > 0)
  {
    GPS_A7.read();

  }
  if (GPS_A7.find("$GPRMC,")) {
    String tempMsg = GPS_A7.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    for (int i = 0; i < 9; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }

  }
  else {

    failedUpdates++;

  }

  request = request_raw_1 + nmea[0] + request_raw_2 + nmea[2] + request_raw_3 + nmea[4] + request_raw_4 ;       //cut the request into smaller components and add value from sensors to it. 
  request += "}";
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  if (nmea[0] != NULL  and nmea[2] != ".000000"){
  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  String url = "/arduino/locations";
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n" +
               "Accept: */*\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + request.length() + " \r\n"
               "\r\n" +
               String(request)) ;
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers ");
      break;
    }
  }
    String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.println(request);
  delay(10000);
  }
  else {
    Serial.println ("Fail to find GPS signal,try again later");
    delay(5000);
  }
  nmea[0] =temp_nmea; 
  nmea[2] =temp_nmea2;
    
  stringplace = 0;
  pos = 0;
}






String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}


