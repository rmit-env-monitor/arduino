HTTPClient http; //Declare object of class HTTPClient
http.begin("http://192.168.2.4/api/heartbeat"); //Specify request destination
http.addHeader("Content-Type", "application/json"); //Specify content-type header
int httpCode = http.POST("{\"heartbeat\": 150}"); //Send the request
String payload = http.getString(); //Get the response payload
Serial.println(httpCode); //Print HTTP return code
Serial.println(payload); //Print request response payload
http.end(); //Close connection
