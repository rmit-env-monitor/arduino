#include <ESP8266WiFi.h>
// Replace with your network credentials
const char* ssid = "QuocDuy";
const char* password = "38220839";
const char* host = "evn-monitor.herokuapp.com";
const int httpsPort = 443;
String request_raw = "{\"utcDateTime\": \"19930821\",\"latitude\": 10.729331,\"longitude\": 106.69429,\"no\": 45,\"so2\": 43,\"pm\": 342,\"o3\": 3423,\"sound\": ";
String request="";
int number;
// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";
const char* fingerprint = "F5:69:8C:CA:29:68:5E:47:26:38:C5:1A:18:F1:8A:6A:EA:60:56:4D";
/*
  // Update these with values suitable for your network.
  IPAddress ip(192,168,1,229); //Node static IP
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  IPAddress dns1(208,67,222,222);
  IPAddress dns2(208,67,220,220);
*/
// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;
void setup() {
  pinMode(A0, INPUT);
  Serial.begin(115200);
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

  request = request_raw + analogRead(A0);
  request += "}";
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
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

//"F5:69:8C:CA:29:68:5E:47:26:38:C5:1A:18:F1:8A:6A:EA:60:56:4D";
//123
// client.print(String("POST ") + "{\"utcDateTime\": \"19930821\",\"latitude\": 10.729331,\"longitude\": 106.69429,\"no\": 45,\"so2\": 43,\"pm\": 342,\"o3\": 3423,\"sound\": 4}" + " HTTP/1.1\r\n" );

//"{\"utcDateTime\": \"19930821\",\"latitude\": 10.729331,\"longitude\": 106.69429,\"no\": 45,\"so2\": 43,\"pm\": 342,\"o3\": 3423,\"sound\": 4}";
