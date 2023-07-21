#include <SoftwareSerial.h>
#include <Servo.h>

#define TX 2
#define RX 4

#define IN1 3
#define IN2 5
#define IN3 6
#define IN4 7

#define Servo_Pin 9

#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 155
#define pos 85

SoftwareSerial blue(RX, TX);
Servo servo;

void Blue_Init(){
  blue.begin(9600);  
}

void Motor_Init(){
  pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
}

void Servo_Init(){
  servo.attach(Servo_Pin);

  servo.write(pos);
}

void Stop(){
  digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);

  servo.write(pos);
}

void Back(int speed){
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);
  digitalWrite(IN2, HIGH);
  analogWrite(IN1, 255 - speed);

  analogWrite(IN3, speed);
	digitalWrite(IN4, LOW);
}

void Forward(int speed){
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);
  digitalWrite(IN2, LOW);
  analogWrite(IN1, speed);

  analogWrite(IN3, 255 - speed);
	digitalWrite(IN4, HIGH);
}

void Turn_Left(){
  servo.write(pos - 35);
}

void Turn_Right(){
  servo.write(pos + 35);
}


void setup() {
  Serial.begin(9600);
  Blue_Init();
  Motor_Init();
  Servo_Init();
}

void loop() {
  char read;
  int speed = MIN_SPEED;
Serial.println(servo.read());
  if (blue.available()){
    read = blue.read();

    if (read >= '0' && read <= '9'){
      if (read == '0')
        speed = MIN_SPEED;
      else 
        speed += (int(read) - 48)*10;
    }

    if (read == 'q')
      speed = MAX_SPEED;
    Serial.println(read);
    
    switch (read){
      case 'S':{
        Stop();
        Serial.println("STOP");
        break;
      }
      case 'F':{
        Forward(speed);
        Serial.println("Forward");
        break;
      }
      case 'B':{
        Back(speed);
        Serial.println("BACK");
        break;
      }
      case 'L':{
        Turn_Left();
        Serial.println("LEFT");
        break;
      }
      case 'R':{
        Turn_Right();
        Serial.println("RIGHT");
        break;
      }
      case 'G':{
        Turn_Left();
        delay(10);
        Forward(speed);
        Serial.println("FL");
        break;
      }
      case 'I':{
        Turn_Right();
        delay(10);
        Forward(speed);
        Serial.println("FR");
        break;
      }
      case 'H':{
        Turn_Left();
        delay(10);
        Back(speed);
        Serial.println("BL");
        break;
      }
      case 'J':{
        Turn_Right();
        delay(10);
        Back(speed);
        Serial.println("BR");
        break;
      }
    }
  }
  delay(100);
}
