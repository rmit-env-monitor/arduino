// SHARP  GP2 Dust Sensor
int shp_Vo = A0; // pin 5 of Sharp to analog pin, sampling pin
int shp_LED = 2; // pin 3 of Sharp to digital pin

int shp_sampling = 280;
int shp_delta = 40;
int shp_sleep = 9680;

float shp_valueVo = 0; // reading value of pin Vo from analog pin A0
float shp_calcVoltage = 0; // converting analog value to voltage
float shp_cctt = 0; // mass concentration from Sharp


// SHINYEI
int yei_P1 = 8; // ouput pin 4 of Shinyei connected to digital pin 8
unsigned long yei_duration;
unsigned long yei_start;
unsigned long yei_sampling_ms = 30000;
unsigned long yei_lowPulse = 0;
float yei_ratio = 0;
float yei_pcs = 0;
float yei_cctt = 0;

// Plantower


// DHT11 Humidity & Temperature
#include <dht.h>
#define dht_dpin A1
int dht_hum = 0;
float dht_hum_crt = 0; // humidity correction
int dht_temp = 0;


void setup() {
	Serial.begin(9600);
	
	// DHT11
	delay(1000);
	
	// SHARP
	pinMode(shp_LED, OUTPUT);
	
	// SHINYEI
	pinMode(8, INPUT);
	yei_start = millis();
}

void loop() {
	sharp();
	shinyei();
}

// DHT11
dht11() {
	DHT.read11(dht_dpin);
	
	DHT.humidity = dht_hum;
	DHT.temperature = dht_temp;
	
	Serial.print("Humidity %: ");
	Serial.println(DHT.humidity);
	
	Serial.print("Temp C: "); // Temperature measured in Celsius Degree
	Serial.println(DHT.temperature);
	delay(2000);
}


// SHARP
sharp() {
	digitalWrite(shp_LED, HIGH);
	delayMicroseconds(shp_sampling);
	
	shp_valueVo = analogRead(shp_Vo);

	delayMicroseconds(shp_delta);
	digitalWrite(shp_LED, LOW);
	delayMicroseconds(shp_sleep);

	shp_calcVoltage = shp_valueVo*(5.0/1024.0);
	shp_cctt = (0.17*shp_calcVoltage - 0.1)*1000;
	
	Serial.print("PM2.5 concentration from SHARP: ");
	Serial.println(shp_cctt);
	Serial.println("ug/m3");

	delay(1000);
}


// SHINYEI
shinyei() {
	yei_duration = pulseIn(yei_P1, LOW);
	yei_lowPulse = yei_lowPulse + duration;

	if ((millis() - yei_start) > yei_sampling_ms)
	{
		yei_ratio = yei_lowPulse/(yei_sampling_ms*10.0); // integer percentage 0 => 100
		yei_pcs = 1.1*pow(yei_ratio,3) - 3.8*pow(yei_ratio,2) + 520*yei_ratio + 0.62; // particle count, pcs/0.01cf
		yei_cctt = yei_pcs*3531.5*(5.89*pow(10,-7))*dht_hum*dht_hum_crt; // converting to ug/m3
		
		Serial.print("PM2.5 concentration from SHINYEI: ");
		Serial.println(yei_cctt);
		Serial.println("ug/m3");
		yei_lowPulse = 0;
		yei_start = millis();
	}
}
