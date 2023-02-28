#include <ECE3.h>


// left_wheel_setup
const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;

//right_wheel_setup
const int right_nslp_pin=11;
const int right_dir_pin=30;
const int right_pwm_pin=39;

uint16_t sensorValues[8]; // right -> left, 0 -> 7

//declare varibale
float Kp = 0.0267;//0.027
float Kd = 0.383;//0.383
float Ki = 0;
float past_error = 0;

int base_speed = 100;//120


bool last_status = false;
bool if_turned = false;

///////////////////////////////////
void setup() {
// put your setup code here, to run once:
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);
//the_wheel_dir
  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);

  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);

  ECE3_Init();
// set the data rate in bits/second for serial data transmission
  
  Serial.begin(9600); 
  delay(2000); //Wait 2 seconds before starting 
  
}

void loop() {
 // put your main code here, to run repeatedly: 

float left_speed = base_speed;
float right_speed = base_speed;
  
// read raw sensor values
  ECE3_read_IR(sensorValues);

// to dertermined to turn around and arrive destination to stop
if (reach_balckline()){

  if (last_status && !if_turned){
    turn_around();
    
    last_status = false;
    if_turned = true;

    accelerate(base_speed);
    }

  if (last_status && if_turned){
    stop_car();
    }
  }
  last_status = reach_balckline();
 
 float error = find_error_value();
 float DeriValue = error - past_error;
 float IntValue = 0;
  IntValue += error;

float SpeedChange =  error * Kp + DeriValue * Kd + IntValue * Ki;
  left_speed -= SpeedChange;
  right_speed += SpeedChange;

  if (left_speed > 255)  {
    left_speed = 254;
  }
  if (right_speed > 255) {
    right_speed = 254;
  }
  if (left_speed < 0)    {
    left_speed = 0;
  }
  if (right_speed < 0)   {
    right_speed = 0;
  }

  analogWrite(left_pwm_pin,left_speed);
  analogWrite(right_pwm_pin,right_speed);

   past_error = error;
  }
//-------the error value---------------
  float find_error_value()
  {
    float error_value = 0;
    float sensor_array[8] = {-1.875, -1.75, -1.5, -1, 1, 1.5, 1.75, 1.875};

    for (int i=0; i<8; i++){
      error_value += sensor_array[i] * sensorValues[i];
      }

    return error_value;
    }

//-------if reach the back line----------
  bool reach_balckline(){
    int sum = 0;
    for (int i = 0; i<8;i++){
      sum += sensorValues[i];
      }
    if (sum > 15000){ 
      return true;
      }
      return false;
    }
//-------reach the black row line to stop------------    
 void decrease_car(){
  digitalWrite(right_dir_pin, HIGH);
  digitalWrite(left_dir_pin, HIGH);
  
  analogWrite(left_pwm_pin, 40);
  analogWrite(right_pwm_pin, 40);

  delay(300);

  digitalWrite(right_dir_pin, LOW);
  digitalWrite(left_dir_pin, LOW);

  analogWrite(left_pwm_pin, 0);
  analogWrite(right_pwm_pin, 0);
  }
//-------To stop the car forever-----------
  void stop_car(){
    decrease_car();
    while(true){}
    }
//----the function to make car turn around-------
  void turn_around(){
  decrease_car();
  digitalWrite(right_dir_pin, LOW);
  digitalWrite(left_dir_pin, HIGH);

  analogWrite(left_pwm_pin, 250);
  analogWrite(right_pwm_pin, 250);

  delay(250);

  digitalWrite(right_dir_pin, LOW);
  digitalWrite(left_dir_pin, LOW);
 }
 //------after turn around increase the speed gradually----
 void accelerate(int baseSpeed)
{
  for (int speed = 0 ; speed <= baseSpeed; speed += 20)
  {
    analogWrite(left_pwm_pin, speed);
    analogWrite(right_pwm_pin, speed);
    delay(20);
  }
}
