float getTempC()
{
  int reading = analogRead(A5);
  float voltage = (reading*500/1024);
  return voltage;
}

