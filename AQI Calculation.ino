int AQI_index = 0;
int AQI_high = 0;
int AQI_low = 0;
int AQI_up = 0;

float brpt_high = 0.0;
float brpt_low = 0.0;
float brpt_down = 0.0;
float cctt = 0.0;

void setup() {
  
}

void loop() {
  
}

PM25() {
  if (0.0 <= cctt <= 12.0)
  {
    AQI_high = 50;
    AQI_low = 0;
    brpt_high = 12.0;
    brpt_low = 0.0;
  }
  else if (12.1 <= cctt <= 35.4)
  {
    AQI_high = 100;
    AQI_low = 50;
    brpt_high = 35.4;
    brpt_low = 12.1;
  }
  else if (35.5 <= cctt <= 55.4)
  {
    AQI_high = 150;
    AQI_low = 101;
    brpt_high = 55.4;
    brpt_low = 35.5;
  }
  else if (55.5 <= cctt <= 150.4)
  {
    AQI_high = 200;
    AQI_low = 151;
    brpt_high = 150.4;
    brpt_low = 55.5;
  }
  else if (150.5 <= cctt <= 250.4)
  {
    AQI_high = 300;
    AQI_low = 201;
    brpt_high = 250.4;
    brpt_low = 150.5;
  }
  else if (250.5 <= cctt <= 350.4)
  {
    AQI_high = 400;
    AQI_low = 301;
    brpt_high = 350.4;
    brpt_low = 250.5;
  }
  else if (350.5 <= cctt <= 500.4)
  {
    AQI_high = 500;
    AQI_low = 401;
    brpt_high = 500.4;
    brpt_low = 350.5;
  }
}

calculation() {
  AQI_up = AQI_high - AQI_low;
  brpt_down = brpt_high - brpt_low;
  AQI_index = (AQI_up/brpt_down)*(cctt - brpt_low) + AQI_low;
}
