// Libraries
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// Create instance of digital compass
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// Motor pins
#define EN1 5 // Pin for run the left motor 
#define IN1 12 // Pin for control left motor direction
#define EN2 6 // Pin for run the right motor 
#define IN2 7 // Pin for control right motor direction

// Variables for direction control
float initial_heading;
float target_heading;
float diff_heading;
bool turn_init;
bool target_reached;

// Left & right
#define LEFT 0
#define RIGHT 1

void setup(void) 
{
  // Start Serial
  Serial.begin(9600);
  
  // Declare motor pins as outputs
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(12,OUTPUT); 
  
  // Initialise the sensor
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
 
  // Initialise direction variables
  turn_init = false;
  target_reached = false;
  
}

void loop(void) 
{
  // Read which button is pressed
  int key_value = analogRead(A6);
  
  // KEY1
  if (key_value > 130 && key_value < 140) {
    turn(90, RIGHT);
  }
 
   // KEY2
  if (key_value > 570 && key_value < 580) {
    turn(90, LEFT);
  }

}

// Turn of a given angle
void turn(float angle, bool turn_direction) {
  
  // Get initial heading
  if (!turn_init) {
    Serial.println("Starting turn ...");
    initial_heading = get_heading();
    
    if (turn_direction) {
      target_heading = initial_heading + angle;
    }
    else {
      target_heading = initial_heading - angle;
    }
    
    Serial.print("Initial heading: ");
    Serial.println(initial_heading);
    Serial.print("Target heading: ");
    Serial.println(target_heading);
    
    turn_init = true;
    target_reached = false;
    diff_heading = 0;
  }
  
  float heading = get_heading();
  
  if (turn_direction) {
    diff_heading = target_heading - heading;
  }
  else {
    diff_heading = heading - target_heading;
  }
 
  while (diff_heading > 0 && !target_reached) {
    
    // Move & stop
    if (turn_direction) {
      right(50);
    }
    else {
      left(50);
    }
    delay(100);
    right(0);
    delay(100);
    
    // Measure heading again
    float heading = get_heading();
    
    if (turn_direction) {
      diff_heading = target_heading - heading;
    }
    else {
      diff_heading = heading - target_heading;
    }
    Serial.print("Difference heading (degrees): "); Serial.println(diff_heading);
    
    if (diff_heading < 0 && !target_reached) {
      target_reached = true;
      turn_init = false;
      diff_heading = 0;
    }
  }
  
  // Stop
  Serial.println("Stopping turn ...");
}

// Get heading from the compass
float get_heading() {
  
  // Get a new sensor event 
  sensors_event_t event; 
  mag.getEvent(&event);

  // Calculate heading based on magnetic intensity in X & Y axis
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 
  return headingDegrees;
}

// Function to command a given motor of the robot
void send_motor_command(int speed_pin, int direction_pin, int pwm, boolean dir)
{
  analogWrite(speed_pin,pwm); // Set PWM control, 0 for stop, and 255 for maximum speed
  digitalWrite(direction_pin,dir);
}

// Turn right
void right(int motor_speed) {
  
  send_motor_command(EN1,IN1,motor_speed,0);
  send_motor_command(EN2,IN2,motor_speed,1);
  
}

// Turn left
void left(int motor_speed) {
  
  send_motor_command(EN1,IN1,motor_speed,1);
  send_motor_command(EN2,IN2,motor_speed,0);
  
}
