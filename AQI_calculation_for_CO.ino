// AQI calculation for CO

float CO_cctt = 0.0;

int CO_AQI_index = 0;

int CO_aqi_high = 0;
int CO_aqi_low = 0;
int CO_up = 0; // used in formula

float CO_brpt_high = 0.0; // breakpoint
float CO_brpt_low = 0.0;
float CO_down = 0.0; // used in formula

void setup(){
  
}

void loop(){
  
}

void CO_Calc(){
  CO_up = CO_aqi_high - CO_aqi_low;
  CO_down = CO_brpt_high - CO_brpt_low;
  CO_AQI_index = (CO_up/CO_down)*(CO_cctt - CO_brpt_low) + CO_aqi_low;
}

void CO(){
  if (0.0 <= CO_cctt <= 4.4)
  {
    CO_aqi_high = 50;
    CO_aqi_low = 0;
    CO_brpt_high = 4.4;
    CO_brpt_low = 0.0;
  }
  else if (4.5 <= CO_cctt <= 9.4)
  {
    CO_aqi_high = 100;
    CO_aqi_low = 51;
    CO_brpt_high = 9.4;
    CO_brpt_low = 4.5;
  }
  else if (9.5 <= CO_cctt <= 12.4)
  {
    CO_aqi_high = 150;
    CO_aqi_low = 101;
    CO_brpt_high = 12.4;
    CO_brpt_low = 9.5;
  }
  else if (12.5 <= CO_cctt <= 15.4)
  {
    CO_aqi_high = 200;
    CO_aqi_low = 151;
    CO_brpt_high = 15.4;
    CO_brpt_low = 12.5;
  }
  else if (15.5 <= CO_cctt <= 30.4)
  {
    CO_aqi_high = 300;
    CO_aqi_low = 201;
    CO_brpt_high = 30.4;
    CO_brpt_low = 15.5;
  }
  else if (30.5 <= CO_cctt <= 40.4)
  {
    CO_aqi_high = 400;
    CO_aqi_low = 301;
    CO_brpt_high = 40.4;
    CO_brpt_low = 30.5;
  }
  else if (40.5 <= CO_cctt <= 50.4)
  {
    CO_aqi_high = 500;
    CO_aqi_low = 401;
    CO_brpt_high = 50.4;
    CO_brpt_low = 40.5;
  }
}


















