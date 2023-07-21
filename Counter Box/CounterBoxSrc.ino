#define Sensor1 6
#define Sensor2 7
#define ControlLed 8

unsigned int counter = 0;
void setup() {
  pinMode(Sensor1,INPUT_PULLUP); // Using internal resistor PULLUP
  pinMode(Sensor2,INPUT_PULLUP); // Using internal resistor PULLUP
  pinMode(ControlLed,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(Sensor1) == 0)
  {
    while(digitalRead(Sensor1) == 0);
      if(digitalRead(Sensor2) != 0)
      {
        while(digitalRead(Sensor2)!=0);
        counter++;
        // LCD display
      }
    }

  else if (digitalRead(Sensor2) == 0)
  {
     while(digitalRead(Sensor2) == 0);
      if(digitalRead(Sensor1) != 0)
      {
        while(digitalRead(Sensor1)!=0);
          if(counter != 0)
            {
              counter--;  
            }
            // LCD display
      }
  }

    
}
