
/****************** MQ131_ResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/ 
float MQ131_ResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE_MQ131*(1023-raw_adc)/raw_adc));
}

/*****************************  MQ131_Read *********************************************
Input:   MICS__pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQ131_ResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different concentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQ131_Read(int MQ131_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQ131_ResistanceCalculation(analogRead(MQ131_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}

/*****************************  MQ131_GetGasPercentage **********************************
Input:   ro_rs_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(ro_rs_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
float  MQ131_GetGasPercentage(float ro_rs_ratio, float *func)
{
  return (pow(ro_rs_ratio/func[0],(1/(func[1]))));
}
