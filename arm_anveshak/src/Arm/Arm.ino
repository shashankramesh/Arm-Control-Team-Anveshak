#define POT1 A8
#define POT2 A7
#define POT3 A10
//#define POT4 A2
#define EN_GRIP 51
#define EN1 43
#define EN2 31
#define EN3 37
#define EN4 25
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
#define PWM_G 6
#define PWM4 4
#define maxSpeed 15.7

#include <ros.h>
#include <ArduinoHardware.h>
#include <geometry_msgs/Vector3.h>
#include <sensor_msgs/Joy.h>

ros::NodeHandle  nh;
int i = 0;
float w1 = 0;
float w2 = 0;
float w3 = 0;

int ankle_pitch = 0, gripp = 0, ungripp = 0 ;

float ankle_rot = 0;


void messageCb( const geometry_msgs::Vector3& om_msg){
  float om1 = om_msg.x;
  float om2 = om_msg.y;
  float om3 = om_msg.z;
  
  w1 = (om1 + (om3*11.0/7.0))*30.0;
  w2 = (om1 - (om3*11.0/7.0))*30.0;
  w3 = om2*30.0;
  
  if(w1 < 0)
  {
    digitalWrite(INB4, HIGH);
    digitalWrite(INA4, LOW);
  }
  else
  {
    digitalWrite(INB4, LOW);
    digitalWrite(INA4, HIGH);
  }
  if(w2 < 0)
  {
    digitalWrite(INB2, HIGH);
    digitalWrite(INA2, LOW);
  }
  else
  {
    digitalWrite(INB2, LOW);
    digitalWrite(INA2, HIGH);
  }
  if(w3 < 0)
  {
    digitalWrite(INB3, HIGH);
    digitalWrite(INA3, LOW);
  }
  else
  {
    digitalWrite(INB3, LOW);
    digitalWrite(INA3, HIGH);
  }
  
  w1 = (255.0/maxSpeed)*abs(w1);
  w2 = (255.0/maxSpeed)*abs(w2);
  w3 = (255.0/maxSpeed)*abs(w3);
  
  constrain(w3, 0, 255);
  map(w3, 0, 255, 0, 70);
  constrain(w1, 0, 255);
  constrain(w2, 0, 255);
  
  int o1 = w1;
  int o2 = w2;
  int o3 = w3;
  
  analogWrite(PWM4, o1);
  analogWrite(PWM2, o2);
  analogWrite(PWM3, o3);
  
}
void joy_cb( const sensor_msgs::Joy& joy){
  
  ankle_pitch = joy.axes[4];
  ankle_rot = joy.axes[5];

  gripp = joy.buttons[4];
  ungripp = joy.buttons[5];
  
}
ros::Subscriber<geometry_msgs::Vector3> sub1("omegas", messageCb );
ros::Subscriber<sensor_msgs::Joy> sub2("joy", joy_cb);

geometry_msgs::Vector3 str_msg;
ros::Publisher chatter("feedback", &str_msg);

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
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(POT3, INPUT);
  pinMode(PWM_G, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
  pinMode(EN_GRIP, OUTPUT);
  pinMode(DIR, OUTPUT);
    
  nh.initNode();
  nh.advertise(chatter);
  nh.subscribe(sub1);
  nh.subscribe(sub2);
  
  pinMode(24, OUTPUT);
  
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
  digitalWrite(EN1, HIGH);
  digitalWrite(EN_GRIP, HIGH);
  analogWrite(24, 90);
}

void loop()
{
  float x1 = analogRead(POT1);
  float x3 = analogRead(POT2);
  float x2 = analogRead(POT3);
  
  float p1 = -4.2857*x1 + 1697.143;
  float p2 = 0.327*x2 - 60.495;
  float p3 = 4.5454*x3 - 1581.818;
    
  str_msg.x = (p1+p3)*3.0/10.0;
  str_msg.y = p2;
  str_msg.z = (p1-p3)*21.0/110.0;
  
  chatter.publish( &str_msg );
  
  if(ankle_pitch == 1)
  {
    digitalWrite(INA1, LOW);
    digitalWrite(INB1, HIGH);
    digitalWrite(DIR, HIGH);
    analogWrite(PWM_G, 25);
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
