

/*
This code was developed by the_3d6 from Ultimate Robotics (http://ultimaterobotics.com.ua). 
License: you can use it for any purpose as long as you don't claim that you are its author and
you don't alter License terms and formulations (lines 1-10 of this file). 
You can share modifications of the code if you have properly tested their functionality, 
including confirming correct sensor response on CO concentrations of 0-30ppm, 100-300ppm and 1000-10000ppm.

If you can improve this code, please do so!
You can contact me at aka.3d6@gmail.com
*/  
/*================================= A7 =================================================*/
#include<SoftwareSerial.h>
SoftwareSerial client(9,10); 
//String reading = "{  \" : 1,  \"latitude\" : 12.983027,  \"longitude\" : 80.2594,  \"timestamp\" : 100,  }";
String writeAPIKey = "MLXBILXQTHPIKTR7";
String tsData = "{\"data\": \"";
String uri = "/api/sensors";
String server ="172.104.45.207";
String final_data = "";
/*================================= sound sensors =================================================*/
int sensor1 = 4;              // pin D4,D5,and D6, all are set initially to HIGH
int DetectedVal1 = HIGH;
int sensor2 = 5;
int DetectedVal2 = HIGH;
int sensor3 = 6;
int DetectedVal3 = HIGH;
String DetectedNoise;
/*================================ MQ131 ================================================================*/
/************************Hardware Related Macros************************************/
#define         MQ131_PIN                      (A3)     //define which analog input channel you are going to use
#define         RL_VALUE_MQ131                     (20)     //define the load resistance on the board, in kilo ohms
        
/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in 
                                                     //normal operation
 
/**********************Application Related Macros**********************************/
float const GAS_O3      (0);  

/*****************************Globals***********************************************/
                                                  
 float           function_MQ131[2]  =  {17.569,-0.893};   // the function of the NO2 concentration: ppm = k.x^a, where k: slope, a: constant. 
                                                                             // in this array, the format is (k,a)
                                                                                                      
float           Ro_MQ131           = 648;                 //Ro is initialized to 648 kilo ohms
/*================================ Sharp dust sensor ===================================================*/
#define measurePin  A4
#define ledPower  7


unsigned int samplingTime = 280;      // these parameters are configured based on the datasheet. 
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

/*================================ mics 2714 - NO2 sensor ===============================================*/
/************************Hardware Related Macros************************************/
#define         MICS_PIN                       (A2)     //define which analog input channel you are going to use
#define         RL_VALUE_MICS                     (5.6)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (1)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet
#define         trigger                       (8)     // pin D8 is the triggering pin of the Mosfet 540N.                     
/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation
 
/**********************Application Related Macros**********************************/
float const GAS_NO2      (0);

/*****************************Globals***********************************************/
                                                  
float           function_MICS[2]  =  {6.4903,1.006};   // the function of the NO2 concentration: ppm = k.x^a, where k: slope, a: constant. 
                                                                             // in this array, the format is (k,a)
                                                                                                      
float           Ro_MICS           =  2.7;                 //Ro is initialized to 2.7 kilo ohms
 
/*================================MQ7 - CO sensor========================================================*/
void setTimer2PWM(byte chA, byte chB) //pins D11 and D3
{
  TCCR2A = 0b10100011; //OCA,OCB, fast pwm       // where WGM is set to be fast PWM running from BOTTOM to MAX (mode 3)
                        // OCA and OCB is configured the same: Clear OC0A (and OC0B) on Compare Match, set OC0A( and OC0B) at BOTTOM ( non-inverting mode) 
  TCCR2B = 0b001; //no prescaler which means that the timer frequency is 16 Mhz. Total count is 16Mhz / 256 = 62500 Hz. Sampling frequency. 
  OCR2A = chA; //0..255      // OC2A = pin D11
  OCR2B = chB;              // OC2B = pin D3
}

float opt_voltage = 0;
byte opt_width = 240; //default reasonable value ?? why 240

void pwm_adjust()
//this function tries various PWM cycle widths and prints resulting
//voltage for each attempt, then selects best fitting one and
//this value is used in the program later
{
  float previous_v = 5.0; //voltage at previous attempt
  float raw2v = 5.0 / 1024.0;//coefficient to convert Arduino's 
  //analogRead result into voltage in volts
  for(int w = 0; w < 250; w++)  // why 250 ?????
  {
    setTimer2PWM(0, w); // set  OCR2A = 0 means, according to the datasheet, creates a narrow spike for each MAX +1 timer clock cycle. (needs confirmation here)
                        // Or for simplication sake, pin D11 is constantly LOW. 
                        // set OCR2B = w means that at the count of w, the output ( or pin D3) changes state, from HIGH to LOW. since we set them to be non-inverting.  
                        // The duration (from the start - BOTTOM) at which the output pin changes state increments as the for loop goes on.  
    float avg_v = 0;    // reset avg_v for the next calculation.
    for(int x = 0; x < 100; x ++) //measure over about 100ms to ensure stable result 
    {
      avg_v += analogRead(A1);
      delay(1);
    }
    avg_v *= 0.01;
    avg_v *= raw2v;
    Serial.print("adjusting PWM w=");
    Serial.print(w);
    Serial.print(", V=");
    Serial.println(avg_v);
    if(avg_v < 3.6 && previous_v > 3.6) //we found optimal width    // why you measure 3.6V instead of 1.4V 
    {
      float dnew = 3.6 - avg_v; //now we need to find if current one
      float dprev = previous_v - 3.6;//or previous one is better
      if(dnew < dprev) //if new one is closer to 1.4 then return it
      {
        opt_voltage = avg_v;
        opt_width = w;
        return;
      }
      else //else return previous one
      {
        opt_voltage = previous_v;
        opt_width = w-1;
        return;
      }
    }
    previous_v = avg_v;
  }
}

float reference_resistor_kOhm = 10.0; // the load resistor's value. 

float sensor_reading_clean_air = 620; //fill raw sensor value at the end of measurement phase (before heating starts) in clean air here! That is CRITICAL for proper calculation
float sensor_reading_100_ppm_CO = -1; //if you can measure it 
//using some CO meter or precisely calculated CO sample, then fill it here
//otherwise leave -1, default values will be used in this case

float sensor_100ppm_CO_resistance_kOhm; //calculated from sensor_reading_100_ppm_CO variable
float sensor_base_resistance_kOhm; //calculated from sensor_reading_clean_air variable

byte phase = 0; //1 - high voltage, 0 - low voltage, we start from measuring
String state[3] = {"LOW----1.4V----MEASURING----","HIGH-----5V-----HEATING-----"};     // informing the state of sensor
int printIndex = 0;

unsigned long prev_ms = 0; //milliseconds in previous cycle
unsigned long sec10 = 0;     // set the initial sec10 timer, this is based on experiment 
float measureCoef = 0.9474;    // these are for calibrating the time in real life vs the time in program, since they are not the same, due to the existance of many sensors' codes. 
float heatingCoef = 0.9524;    // and these are collected from experiments. 

unsigned long high_on = 0; //time when we started high temperature cycle
unsigned long low_on = 0; //time when we started low temperature cycle
unsigned long last_print = 0; //time when we last printed message in serial

float sens_val = 0; //current smoothed sensor value
float last_CO_ppm_measurement = 0; //CO concentration at the end of previous measurement cycle

float raw_value_to_CO_ppm(float value)
{
  if(value < 1) return -1; //wrong input value
  sensor_base_resistance_kOhm = reference_resistor_kOhm * 1023 / sensor_reading_clean_air - reference_resistor_kOhm;  // simply voltage divider
  if(sensor_reading_100_ppm_CO > 0)
  {
    sensor_100ppm_CO_resistance_kOhm = reference_resistor_kOhm * 1023 / sensor_reading_100_ppm_CO - reference_resistor_kOhm;
  }
  else
  {
    sensor_100ppm_CO_resistance_kOhm = sensor_base_resistance_kOhm * 0.5;
//This seems to contradict manufacturer's datasheet, but for my sensor it 
//if you have calibrated sensor to produce reference 100 ppm CO, then
//use it instead    
  }
  float sensor_R_kOhm = reference_resistor_kOhm * 1023 / value - reference_resistor_kOhm;
  float R_relation = sensor_100ppm_CO_resistance_kOhm / sensor_R_kOhm;    
  float CO_ppm = 100 * (exp(R_relation) - 1.648);                             // how exactly you came up with this?
  if(CO_ppm < 0) CO_ppm = 0;
  return CO_ppm;
}

void startMeasurementPhase()
{
  phase = 0;
  low_on = sec10;
  setTimer2PWM(0, opt_width);
    printIndex = 0;
}

void startHeatingPhase()
{
  phase = 1;
  high_on = sec10;
  setTimer2PWM(0, 255);
    printIndex = 1;
}
/*==================================== SETUP===================================*/
void setup() {

//WARNING! Each sensor is different!
//You MUST calibrate sensor manually and
//set proper sensor_reading_clean_air value before using
//it for any practical purpose!

  Serial.begin(115200);
  client.begin(115200);
  delay(1000);

  if (client.available())
  {
    Serial.print("Connected");
  }
  else
  {
    Serial.print("NotConnected");
  }

  //initSIM();
//  connectGPRS();
  // connectHTTP();
  client.println("AT");

  ShowSerialData(); 
  
  analogReference(DEFAULT);

  pinMode(A0, INPUT);       // A0 for MQ7 - CO sensor's output.
  pinMode(A1, INPUT);       // adjusting PWM
  pinMode(A5,INPUT);        // LM35 - temperature sensor. 
  pinMode (sensor1, INPUT) ;
  pinMode (sensor2, INPUT) ;
  pinMode (sensor3, INPUT) ;
  
  pinMode(trigger, OUTPUT); // triggering the MOSFET for the mics 2714 to be warm up. 
  pinMode(ledPower,OUTPUT); // the LED of the infrared dust sensor
  pinMode(3, OUTPUT);
//    /*************** NO2 sensor**************/
//  digitalWrite(trigger,HIGH);                   // NO2 sensor: start the heating period for 30 seconds, and then turn to LOW after that. 
//  Serial.println("Heating up.... wait 30 seconds!!");
//  delay(30000);
//  digitalWrite(trigger, LOW);
//
//  Serial.print("Calibrating...\n");                
//  Ro_MICS = MICS_Calibration(MICS_PIN);                       //Calibrating the sensor. Please make sure the sensor is in clean air 
//                                                         //when you perform the calibration         
//        
//  Serial.print("Calibration is done...\n"); 
//  Serial.print("Ro of mics-2714=");
//  Serial.print(Ro_MICS);
//  Serial.print("kohm");
//  Serial.print("\n");
  /*************** CO sensor**************/
  pwm_adjust();

  Serial.print("PWM result: width ");
  Serial.print(opt_width);
  Serial.print(", voltage ");
  Serial.println(opt_voltage);
  Serial.println("Data output: raw A0 value, heating on/off (0.1 off 1000.1 on), CO ppm from last measurement cycle");

  startMeasurementPhase(); //start from measurement
}
/*============================================ LOOP =========================================*/
void loop() 
{
  
  /************** sound sensors ************/
  DetectedVal1 = digitalRead (sensor1) ;
  DetectedVal2 = digitalRead (sensor2) ;
  DetectedVal3 = digitalRead (sensor3) ;
   if (DetectedVal1 == HIGH && DetectedVal2 == LOW) // If we hear a sound
  {
    DetectedNoise=">60db, annoying";
  }
  else if (DetectedVal2 == HIGH && DetectedVal3 == LOW) 
  {
   DetectedNoise= ">80db, loud";
  }
  else if (DetectedVal3 == HIGH)
  {
    DetectedNoise= ">90db, danger";
  }
  else
  {
    DetectedNoise= "<60db, normal";
  }
/*************** dust sensor **************/  
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.166*calcVoltage-0.09;     // derives from the function y( v-out) = 6.023x(dust density) +0.026, from the graph in datasheet. 

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }
/*************** CO sensor ************/
  unsigned long ms = millis();
  int dt = ms - prev_ms;

// update the sec10 times every 800ms, the remaining time in 1000 ms ( 1s) is for the calculations
  if(dt > 500 || dt < 0) 
  {
    prev_ms = ms; //store previous cycle time
    sec10+=5; //increase counter for 1 second every 800ms, but afterall calculations, the time elapsed in real life is still around 1s.  
    Serial.print("sec10=====");
    Serial.println(sec10);
  }
  
  if(phase == 1 && sec10 - high_on > 600*heatingCoef) //60 seconds of heating ended?
    startMeasurementPhase();
  if(phase == 0 && sec10 - low_on > 900*measureCoef) //90 seconds of measurement ended?
  {
    last_CO_ppm_measurement = raw_value_to_CO_ppm(sens_val);
    startHeatingPhase();
  }

  float v = analogRead(A0); //reading value
  sens_val *= 0.999; //applying exponential averaging using formula
  sens_val += 0.001*v; // average = old_average*a + (1-a)*new_reading
   /***************** temperature sensor  ********************/
  float tempC = getTempC();   // get temperature from the sensor
    /***************** NO2 sensor ********************/
  float NitrogenDioxide = MICS_GetGasPercentage((MICS_Read(MICS_PIN)/Ro_MICS),function_MICS);
    /***************** O3 sensor ********************/
  float Ozone = MQ131_GetGasPercentage(Ro_MQ131/(MQ131_Read(MQ131_PIN)),function_MQ131);
  /***************************************** THE PRINTING PROCESS ************************************/
  if(sec10 - last_print > 9)         //print measurement result into serial 1 times per second, 9 = 900ms since sec10 and lastprint are incrementing every 800ms. 
  {
      /*************** CO sensor**************/
    last_print = sec10;
    Serial.println("========== CO =========");
    Serial.print(state[printIndex]);
    Serial.println(last_CO_ppm_measurement);
    Serial.println();
      /*************** temperature sensor**************/
    Serial.println("========== Temp =========");
    Serial.print(tempC);
    Serial.println(" oC");
    Serial.println();
      /*************** NO2 sensor**************/
   Serial.println("========== NO2 =========");
   Serial.print("NO2:"); 
   Serial.print(NitrogenDioxide);
   Serial.print( "ppm" );
   Serial.print("\n");
    /***************** dust sensor ************/
  Serial.println("Dust Density:");
  Serial.print(dustDensity);
  Serial.println(" mg/m3");
  /********************* O3 sensor******************/
   Serial.println("==================== O3 ================");
   Serial.print("O3 concentration:"); 
   Serial.print(Ozone );
   Serial.print( "ppb" );
   Serial.print("\n");
   /******************* sound sensors **********************/
  Serial.println("======================= sound level================");
 Serial.println(DetectedNoise);
 
   final_data = tsData  + last_CO_ppm_measurement +  "|" + tempC + "|" + NitrogenDioxide + "|" + Ozone  + "|" + dustDensity + "|" + DetectedNoise + "\"}";
  }
//   bool a= false;
//   if (last_CO_ppm_measurement != 0 && tempC != 0  && NitrogenDioxide != 0){
    connectGPRS();
//   }
//   else {
//    connectGPRS();
//Serial.println(postRequest);
                            
//   }
}     // loop end

