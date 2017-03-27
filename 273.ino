// SHARP  GP2 Dust Sensor
int shp_Vo = A0; // pin 5 of Sharp to analog pin, sampling pin
int shp_LED = 2; // pin 3 of Sharp to digital pin

int shp_sampling = 280;
int shp_delta = 40;
int shp_sleep = 9680;

float shp_valueVo = 0; // reading value of pin Vo from analog pin A0
float shp_calcVoltage = 0; // converting analog value to voltage
float shp_mcctt = 0; // mass concentration from Sharp


// SHINYEI
int yei_P1 = 8; // ouput pin 4 of Shinyei connected to digital pin 8
unsigned long yei_duration;
unsigned long yei_start;
unsigned long yei_sampling_ms = 30000;
unsigned long yei_lowPulse = 0;
float yei_ratio = 0;
float yei_mcctt = 0;


// Plantower


void setup() {
	// SHARP
	Serial.begin(9600);
	pinMode(shp_LED, OUTPUT);
	
	// SHINYEI
	pinMode(8, INPUT);
	yei_start = millis();
}


// SHARP
void loop() {
	digitalWrite(shp_LED, HIGH);
	delayMicroseconds(shp_sampling);
	
	shp_valueVo = analogRead(shp_Vo); 

	delayMicroseconds(shp_delta);
	digitalWrite(shp_LED, LOW);
	delayMicroseconds(shp_sleep);

	shp_calcVoltage = shp_valueVo*(5.0/1024.0);
	shp_mcctt = (0.17 * shp_calcVoltage - 0.1)*1000;
	
	Serial.print("PM2.5 concentration: ");
	Serial.println(shp_mcctt);
	Serial.println("ug/m3");

	delay(1000);
}


// SHINYEI
void loop() {
	yei_duration = pulseIn(yei_P1, LOW);
	yei_lowPulse = yei_lowPulse + duration;

	if ((millis() - yei_start) > yei_sampling_ms)
	{
		yei_ratio = yei_lowPulse / (yei_sampling_ms * 10.0); // integer percentage 0 => 100
		yei_mcctt = 1.1*pow(yei_ratio,3) - 3.8*pow(yei_ratio,2) + 520*yei_ratio + 0.62;
		
		Serial.print("PM2.5 concentration from SHINYEI: ");
		Serial.println(yei_mcctt);
		Serial.println("ug/m3");
		yei_lowPulse = 0;
		yei_start = millis();
	}
}
