/* 
1: GND
2: OUTPUT (P2)
3: 5VDC 90mA
4: OUTPUT (P1)
5: INPUT (T1) ... FOR THRESHOLD FOR [P2]
*/


int P1 = 3; // pm2.5
int P2 = 9; // pm10

unsigned long duration_P1;
unsigned long duration_P2;

unsigned long start;
unsigned long sampletime_ms = 30000;

unsigned long lowpulse_P1 = 0;
unsigned long lowpulse_P2 = 0;

float ratio_P1 = 0;
float ratio_P2 = 0;

void setup(){
  Serial.begin(9600);
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  start = millis();
}

void loop(){
  duration_P1 = pulseIn(P1, LOW);
  duration_P2 = pulseIn(P2, LOW);
  
  lowpulse_P1 = lowpulse_P1 + duration_P1;
  lowpulse_P2 = lowpulse_P2 + duration_P2;
  
  if ((millis() - start) > sampletime_ms)
  {
    ratio_P1 = lowpulse_P1/(sampletime_ms*10.0);
    ratio_P2 = lowpulse_P2/(sampletime_ms*10.0);
    
    Serial.print("low pulse occupancy PM2.5: ");
    Serial.print(lowpulse_P1);
    Serial.print("low pulse occupancy PM10: ");
    Serial.print(lowpulse_P2);
    
    Serial.print("ratio PM2.5: ");
    Serial.print(ratio_P1);
    Serial.print("ratio PM10: ");
    Serial.print(ratio_P2);
    
    lowpulse_P1 = 0;
    lowpulse_P2 = 0;
    start = millis();
  }
}
