#define EN1 43
#define EN2 31
#define EN3 37
#define EN4 25
#define EN_GRIP 51
#define INA1 45
#define INA2 33
#define INA3 39
#define INA4 27
#define DIR 49
#define INPUT1 10
#define INPUT2 9
#define INB1 47
#define INB2 35
#define INB3 41
#define INB4 29
#define PWM1 8
#define PWM2 5
#define PWM3 7
#define PWM4 4
#define PWM_G 6
#define maxSpeed 2.0

#include <ros.h>
#include <ArduinoHardware.h>
#include <sensor_msgs/Joy.h>

ros::NodeHandle  nh;
int i = 0;
float w1 = 0;
float w2 = 0;
float w3 = 0;

int ankle_pitch = 0, gripp = 0, ungripp = 0 ;

float om1 = 0, om2 = 0, om3 = 0, ankle_rot = 0;

void messageCb( const sensor_msgs::Joy& joy){
  om1 = joy.axes[0];
  om2 = joy.axes[2];
  om3 = joy.axes[1];
  //om4 = joy.buttons[3];
  //om5 = joy.axes[1];
  
  ankle_pitch = joy.axes[4];
  ankle_rot = joy.axes[5];

  gripp = joy.buttons[4];
  ungripp = joy.buttons[5];
  
}

ros::Subscriber<sensor_msgs::Joy> sub("ns1/joy", messageCb );

void setup()
{ 
  for(i = 25; i<=47; i+=2)
  {
    pinMode(i,OUTPUT);
  }
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(PWM_G, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
  pinMode(EN_GRIP, OUTPUT);
  pinMode(DIR, OUTPUT);
  nh.initNode();
  nh.subscribe(sub);
  pinMode(24, OUTPUT);
  
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN1, HIGH);
  digitalWrite(EN_GRIP, HIGH);
  analogWrite(24, 140);
}

void loop()
{
  if(om1 < 0)
  {
    digitalWrite(INB4, HIGH);
    digitalWrite(INA4, LOW);
    digitalWrite(INB2, HIGH);
    digitalWrite(INA2, LOW);
    analogWrite(PWM4, -om1*170);
    analogWrite(PWM2, -om1*170);
  }
  else
  {
    if(om1 == 0)
    {
      if(om2 < 0)
      {
        digitalWrite(INB4, LOW);
        digitalWrite(INA4, HIGH);
        digitalWrite(INB2, HIGH);
        digitalWrite(INA2, LOW);
        analogWrite(PWM4, -om2*180);
        analogWrite(PWM2, -om2*255);
      }
      else
      {
        digitalWrite(INB4, HIGH);
        digitalWrite(INA4, LOW);
        digitalWrite(INB2, LOW);
        digitalWrite(INA2, HIGH);
        analogWrite(PWM4, om2*255);
        analogWrite(PWM2, om2*180);
      }
    }
    else
    {
      digitalWrite(INB4, LOW);
      digitalWrite(INA4, HIGH);
      digitalWrite(INB2, LOW);
      digitalWrite(INA2, HIGH);
      analogWrite(PWM4, om1*255);
      analogWrite(PWM2, om1*255); 
    } 
  }
      /*if(om3 == 1)
      {
        digitalWrite(INB4, HIGH);
        digitalWrite(INA4, LOW);
        digitalWrite(INB2, LOW);
        digitalWrite(INA2, HIGH);
        digitalWrite(PWM4, HIGH);
        digitalWrite(PWM2, HIGH);
      }
      else
      {
        digitalWrite(PWM4, LOW);
        digitalWrite(PWM2, LOW);
        if(om4 == 1)
        { 
          digitalWrite(INB4, LOW);
          digitalWrite(INA4, HIGH);
          digitalWrite(INB2, HIGH);
          digitalWrite(INA2, LOW);
          digitalWrite(PWM4, HIGH);
          digitalWrite(PWM2, HIGH);
        }
        else
        {
          digitalWrite(PWM4, LOW);
          digitalWrite(PWM2, LOW); 
        } 
      }*/
  if(om3 < 0)
  {
    digitalWrite(INB3, HIGH);
    digitalWrite(INA3, LOW);
    analogWrite(PWM3, -om3*255);
  }
  else
  {
    digitalWrite(INB3, LOW);
    digitalWrite(INA3, HIGH);
    analogWrite(PWM3, om3*255);
  }
  if(ankle_pitch == 1)
  {
    digitalWrite(INA1, LOW);
    digitalWrite(INB1, HIGH);
    digitalWrite(DIR, HIGH);
    analogWrite(PWM_G, 27);
    analogWrite(PWM1, 17);
  }
  else
  {
    if(ankle_pitch == 0)
    {
      if(ankle_rot < 0)
      {
        digitalWrite(INA1, HIGH);
        digitalWrite(INB1,LOW);
        digitalWrite(DIR, HIGH);
        analogWrite(PWM_G, -ankle_rot*25);
        analogWrite(PWM1, -ankle_rot*15);
      }
      else
      {
        digitalWrite(INA1, LOW);
        digitalWrite(INB1,HIGH);
        digitalWrite(DIR, LOW);
        analogWrite(PWM_G, ankle_rot*25);
        analogWrite(PWM1, ankle_rot*15);
      }
    }
    else
    {
      digitalWrite(INA1, HIGH);
      digitalWrite(INB1, LOW);
      digitalWrite(DIR, LOW);
      analogWrite(PWM_G, 25);
      analogWrite(PWM1, 17);
    }
  }
  if(gripp == 1)
  {
    digitalWrite(INPUT1, HIGH);
    digitalWrite(INPUT2, LOW);
  }
  else
  {
    if(ungripp == 1)
    {
      digitalWrite(INPUT1, LOW);
      digitalWrite(INPUT2, HIGH);
    }
    else
    {
      digitalWrite(INPUT1, LOW);
      digitalWrite(INPUT2, LOW);
    }
  }
  nh.spinOnce();
}
