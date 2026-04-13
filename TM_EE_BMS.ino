/*
TM_EE_BMS.ino
- Returns the voltage of the battery and pack as well as temperature.

Functions:
  - When Green Led is >> 50%, battery is in steady-state
  - Serial prints "LOW BATTERY" if battery is below 10%
  - If Red is high, temperature is above 55 deg celcius or below -55 deg C

*/

const int g_led = 2;
const int r_led = 9;
float pack_voltage = 0;
float pack_temp = 0;
float cell_voltage = 0;
const float Vref = 5.0;
float B_val = 3935;
int battery_life = 0;
bool notHotOrCold = true;

//Voltage Divider for cell voltage
const float R1_cell_voltage = 14700; //In ohms
const float R2_cell_voltage = 10000; //In ohms
const int cell_volt_pin = A2; //Analog pin that is read
float Vout_cell = 0;

//Voltage Divider for temperature sensing
const float R1_temp = 2220; //In ohms
float R2_temp = 0; //thermistor resistance: what we measure
float Vout_temp = 0; //measured at A1
const int temp_volt_pin = A1; //Analog pin for temperature sensing application
const float Vin_temp = 5; //constant 5V from 5V pin;
// float last_T_voltage = 0;

//Voltage Divider for pack voltage reading
int Vin_pack_volt = 0;
const float R1_pack_voltage = 14700; //In ohms
const float R2_pack_voltage = 10000; //In ohms
const int pack_volt_pin = A0; //Analog pin that is read
float Vout_pack = 0;
// float last_V_voltage = 0; //For ema filter

void setup() {
  Serial.begin(9600);
  Serial.println("___>_<___");
  delay(100);
  analogReference(DEFAULT);
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(pack_volt_pin, INPUT);
  pinMode(temp_volt_pin, INPUT);
  pinMode(cell_volt_pin, INPUT); 

}

void loop() {
  //pack_voltage = Vout * ((R1 + R2)/R2)
  Vout_pack = analogRead(pack_volt_pin) * Vref / 1024.0;
  pack_voltage = Vout_pack * ((R1_pack_voltage + R2_pack_voltage) / R2_pack_voltage);

  //cell_voltage = Vout * ((R1 + R2)/R2)
  Vout_cell = analogRead(cell_volt_pin) * Vref / 1024.0;
  cell_voltage = Vout_cell * ((R1_cell_voltage + R2_cell_voltage) / R2_cell_voltage);

  //pack_temp = R1 * (Vout/(Vin + Vout))
  Vout_temp = analogRead(temp_volt_pin) * Vref / 1024.0;
  R2_temp = (R1_temp * (Vout_temp / (Vin_temp - Vout_temp))); 
  pack_temp = (1.0 / ( (1.0 / (25 + 273.15 )) + (1.0 / B_val) * log( R2_temp / 1000) )) - 273.15; //Beta formula - result in Celcius

  battery_life = ((cell_voltage - 2.75)/ (4.2 - 2.75) ) * 100;

  if (battery_life < 0) { //Wrong Setup on user-end
    Serial.print("Wrong Battery?? ");
  }

  // UI Features for BMS monitoring
  // Scenario: When battery voltage >> 50 %
  if (battery_life > 50 && notHotOrCold) {
    Serial.print("Battery: ");
    Serial.print(floor(battery_life / 10.0) * 10.0);
    Serial.print("% ");

    // Green Led: High
    digitalWrite(g_led, HIGH);
  }
  else {
    digitalWrite(g_led, LOW);
    if (battery_life < 10) {
      Serial.print("LOW BATTERY!!!");
    }
  }

  // Scenario: When temperature > + - 55 deg C. This is close to the max discharge temperature
  if (abs(pack_temp) > 55) {
    notHotOrCold = false; // double-jeopardy

    //SPECIAL EDGE_CASE: low battery & high temperature
     if (battery_life < 10) {
      digitalWrite(r_led, HIGH);
      delay(500); //Every 0.5 s
    }
    else {
    // Red Led: Blink every 2s 
    digitalWrite(r_led, HIGH);
    delay(1500);
    digitalWrite(r_led, LOW);
    delay(1500);

    }
  }
  else {
    digitalWrite(r_led, LOW);
    notHotOrCold = true;
  }
  Serial.print("Pack Voltage: ");
  Serial.print(pack_voltage);
  Serial.print(" V");
  Serial.print(" | Pack Temperature: ");
  Serial.print(pack_temp);
  Serial.print(" C");
  Serial.print(" | Cell Voltage: ");
  Serial.print(cell_voltage);
  Serial.println("V");


  delay(1000);
}
  
