////////////////////
//not test
void connectGPRS()
{
  client.println("AT");
  delay(1000);
  ShowSerialData();
//  client.println("AT");
//  delay(1000);
//  ShowSerialData();
//  client.println("AT");
//  delay(1000);
//  ShowSerialData();
//  client.println("AT");
//  delay(1000);
//  ShowSerialData();

  client.println("AT+CIPSTATUS");//APN
  delay(1000);
  ShowSerialData();

  client.println("AT+CIPSTART=\"TCP\",\"172.104.45.207\",8000");
  delay(1000);
  ShowSerialData();

  client.println("AT+CIPSTATUS");
  delay(1000);
  ShowSerialData();



   String data = "";

  String postRequest =
    "POST " + uri + " HTTP/1.0\r\n" +
    "Host: " + server + "\r\n" +
    "Accept: *" + "/" + "*\r\n" +
    "Content-Length: " + final_data.length() + "\r\n" +
    "Content-Type: application/json\r\n" +
    "\r\n" + final_data;

  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  client.print(sendCmd);
  client.println(postRequest.length() );
  delay(500);

  if (client.find(">")) {
    Serial.println("Sending.."); client.print(postRequest);

    if ( client.find("SEND OK")) {
      Serial.println("Packet sent");

      while (client.available()) {
        String tmpResp = client.readString();
        Serial.println(tmpResp);
      }
      // close the connection
      client.println("AT+CIPCLOSE");
    }

  }

 

  ShowSerialData();
  delay(4000);
  
  client.println("at+CIFSR");
  delay(1000);
  ShowSerialData();

  client.println("AT+CIPSHUT");
  delay(1000);
  ShowSerialData();


  client.println("AT+CIPSTATUS");
  delay(1000);
  ShowSerialData();

}

void ShowSerialData()
{
  while (client.available() != 0)
  {
    Serial.write(client.read());
    delay(1);
  }
}
