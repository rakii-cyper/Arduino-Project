//RFID
/*
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 10
* RST: Pin 9
*/
#include <SPI.h>
#include <RFID.h>
#include <EEPROM.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

#define ADD 2
#define ERASE 3
#define CANCLE 4

#define SERVO_PIN 5

#define Led_1 6
#define Led_2 7

Servo myservo;
RFID rfid(SS_PIN, RST_PIN);

unsigned char reading_card[5];
int addr;
bool check;

void Init_button() {                //Function for Initialization button
  pinMode(ADD, INPUT_PULLUP);
  pinMode(ERASE, INPUT_PULLUP);
  pinMode(CANCLE, INPUT_PULLUP);
}

void Init_servo(){                 //Function for Initialization servo
  myservo.attach(SERVO_PIN);
  myservo.write(90);
}

void Init_led(){                   //Function for Initialization led
    pinMode(Led_1, OUTPUT);
    pinMode(Led_2, OUTPUT);
    
    digitalWrite(Led_1, LOW);
    digitalWrite(Led_2, LOW);
}

void Add_Value(){                //Function for ADD button
  digitalWrite(Led_2, HIGH);

  int NumOfCard = EEPROM.read(0);     //Ô nhớ 0 của EEPROM dùng để lưu số thẻ đã được thêm
  addr = NumOfCard * 5 + 1;           // Từ đó tính được địa chỉ tiếp theo để lưu giá trị         
                                 // VÍ DỤ: Số thẻ đã được thêm là 0 => NumOfCard = 0;
                                 // Vậy địa chỉ đầu tiên được thêm sẽ là 1 (0 * 5 + 1)
                                 // VÍ DỤ: Số thẻ đã được thêm là 1 => NumOfCard = 1;
                                 // Vậy địa chỉ đầu tiên được thêm sẽ là 6 (1 * 5 + 1)
  if (rfid.isCard()) {          
    if (rfid.readCardSerial()) {  // Nếu đọc được thẻ
      for (int i = 0; i < 5; i++) {  
        EEPROM.write(addr, rfid.serNum[i]); // Lưu từng byte dữ liệu vào từng ô nhớ của EEPROM với ô temp là đầu tiên sau đó cộng lên 1
        addr++;
        delay(5);  // delay 5ms sau mỗi lần ghi giá trị để tránh bị lỗi
      }
       EEPROM.write(0, ++NumOfCard); // Sau khi lưu thẻ thì số thẻ đã lưu sẽ cộng thêm 1
    }
    check = 1;  // Đã thêm thẻ thành công
    while(rfid.readCardSerial()); // Cho đến lúc mà hết đọc được thẻ
    rfid.halt();
  }
  
  if (check){  // Nếu thêm thẻ thành công thì nhấp nháy
    digitalWrite(Led_2, LOW);
    digitalWrite(Led_1, HIGH);
    delay(500);
    digitalWrite(Led_1, LOW);
  }
  
}

void Erase_Value(){
  digitalWrite(Led_2, HIGH);

  int NumOfCard = EEPROM.read(0); // Đọc số thẻ đã được lưu giá trị
  int temp, j;

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      for (int i = 0; i < 5; i++) {
        reading_card[i] = rfid.serNum[i]; // Lưu giá trị của thẻ vào mảng reading_card
      } 
    }
    rfid.halt();
  }

  for (int Target_Card = 0; Target_Card < NumOfCard; Target_Card++) { // Target_Card là số thẻ (ví dụ trong EEPROM đang lưu 2 thẻ thì với Target_Card = 1 sẽ là thẻ thứ nhất)
    addr = 5 * Target_Card + 1; //Địa chỉ đầu tiên lưu giá trị của Target_Card

    for (j = 0; j < 5; j++){ //Kiểm tra giá trị của thẻ đã đọc có giống với giá trị của thẻ đang xét trong EEPROM không
      temp = EEPROM.read(addr);
      addr++;
      if (reading_card[j] != temp) // 1 byte dữ liệu khác nhau thì thoát 
        break;
    }

    if (j == 5){ // Nếu 5 bytes giống nhau hết thì thẻ đang đọc trùng với thẻ đang xét trong EEPROM
      if (NumOfCard - Target_Card == 1){ // Nôm na cái EEPROM như là 1 mảng một chiều thì cái thẻ mà nằm cuối mảng sẽ thỏa điều kiện này
        for (int j = 5 * Target_Card + 1; j <= NumOfCard * 5; j++){ //Lúc này chỉ cần xóa thẻ đó đi
          EEPROM.write(j, 0);
          delay(5);
        }
      }

      else{ // Còn nếu thẻ nằm giữa mảng thì xóa thẻ đó và dời các giá trị của từng thẻ khác lên để bù vị trí của thẻ đó
        for (int j = 5 * Target_Card + 1; j <= NumOfCard * 5; j++){
          temp = EEPROM.read(j + 5);
          EEPROM.write(j, temp);
          delay(5);
        }
      }
      check = 1;  // Đã xóa được thẻ thành công
      temp = EEPROM.read(0);
      EEPROM.write(0, --temp); //Trừ giá trị số thẻ đã thêm đi 1
    }
  }

  if (check){ // Nếu xóa thành công thì đèn nhấp nháy
    digitalWrite(Led_2, LOW);
    digitalWrite(Led_1, HIGH);
    delay(500);
    digitalWrite(Led_1, LOW);
  }
}

void Check_Card(){
  int NumOfCard = EEPROM.read(0);
  int temp, j;
  
  if (rfid.isCard()) { // Tương tự trong hàm Erase_Value
    if (rfid.readCardSerial()) {
      for (int i = 0; i < 5; i++) {
        reading_card[i] = rfid.serNum[i];
      } 
    }
    rfid.halt();

    check = 0;

    for (int Target_Card = 0; Target_Card < NumOfCard; Target_Card++) { // Tương tự trong hàm Erase_Value
      addr = 5 * Target_Card + 1;
  
      for (j = 0; j < 5; j++){
        temp = EEPROM.read(addr);
        addr++;
        if (reading_card[j] != temp)
          break;
      }
  
      if (j == 5){ // Nếu trùng khớp 
        myservo.write(-180); // Mở servo ra góc -180
        digitalWrite(Led_1, HIGH);
        delay(2000);
        myservo.write(90); // Đưa servo về vị trí cũ
        digitalWrite(Led_1, LOW);
        check = 1;
        break;
      }
    }

    if (!check){ //Để tránh lỗi khi mà không làm gì đèn cũng sáng
      digitalWrite(Led_2, HIGH);
      delay(1000);
      digitalWrite(Led_2, LOW);
    }
  }
}

void reset_reading(){
  for (int i = 0; i < 5; i++)
    reading_card[i] = 0;
}

void setup(){
  Serial.begin(9600);
  SPI.begin();
  rfid.init();
  Init_button();
  Init_servo();
  Init_led();
}

void loop(){
  check = 0;
  if (!digitalRead(ADD)){
    while (!digitalRead(ADD));

    while (!check){
      Add_Value();
     
      if (!digitalRead(CANCLE)){
        while(!digitalRead(CANCLE));
        digitalWrite(Led_2, LOW);
        break;
      }
    }
  }

  check = 0;
  if (!digitalRead(ERASE)){
    while (!digitalRead(ERASE));

    while (!check){
      Erase_Value();

      if (!digitalRead(CANCLE)){
        while(!digitalRead(CANCLE));
        digitalWrite(Led_2, LOW);
        break;
      }
    }
  }

  reset_reading();
  
  Check_Card();
}
