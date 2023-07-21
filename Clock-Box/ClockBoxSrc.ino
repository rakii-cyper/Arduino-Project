#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h> 

DS3231  rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x3F, 16, 2); 

#define Toggle_Up 2
#define Set_time 3
#define Set_date 4

String temp[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"}; //Thứ trong tuần đầy đủ
String shot_dow[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"}; //Thứ trong tuần rút gọn
bool check_inter_0, check_inter_1;

String day_of_week_display(){ 
  Time Return_time; //Biến thời gian Return_time
  Return_time = rtc.getTime(); //Biến thời gian sẽ lấy giá trị từ DS3231

  return shot_dow[Return_time.dow - 1]; //Return_time.dow sẽ trả về số (Thứ 2 = 1 và CN = 7) mà mảng thì từ 0 nên phải trừ đi 1 
  // Nếu không rút gọn thì k đủ chỗ in trên lcd
}

int Convert_Str_To_Int_Months(String temp_month){ // đổi giá trị của tháng từ String sang số cho dễ tính (tháng 1 là 1 còn tháng 12 là 2)
  String months_str[12] = {"January", "February", "March", "April", "May",
                          "June", "July", "August", "September", "October", "November", "December"}; //Tháng trong 1 năm

  for (int i = 0; i < 12; i++){
    if (temp_month == months_str[i]) 
      return i + 1;
  } 
}

void Toggle_Up_Value(){  //Int tăng giá trị
  check_inter_0 = true;
}

void Toggle_Down_Value(){ //Int giảm giá trị (chỉ xài nút này khi chỉnh năm)
  check_inter_1 = true;
}

void set_date_button(){
  Time Return_time; // biến thời gian Return_time
  Return_time = rtc.getTime(); //lấy giá trị từ DS3231
  
  int Test_case = 0;
  int ArrayOfValue[3] = {Return_time.date, Convert_Str_To_Int_Months(rtc.getMonthStr()) ,Return_time.year}; // Lưu giá trị của ngày tháng năm vào mảng
  // DO hàm rtc.getMonthStr() nó trả về String (VÍ DỤ là January) mà muốn set thời gian cho ds3231 thì p dùng số nên mới phải dùng cái hàm phía trên Convert_Str_To_Int_Months
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Date:");
  
	while (Test_case < 3){ // CHo đến khi chỉnh được hết đủ 3 giá trị ngày tháng năm
    if (!digitalRead(Set_date)) { // Cái này là pooling nè
      while (!digitalRead(Set_date));
      Test_case++;
    }

    lcd.setCursor(3, 1);
    lcd.print(rtc.getDateStr()); //xuất ngày tháng năm
    delay (500);

    switch (Test_case){ // Làm cho nhấp nháy cái chỗ mà mình đang chỉnh
      case 0:{          // ví dụ: nó in ra là 12.1.2000 thì với test_case = 0 thì nó sẽ là   .1.2000 rồi 12.1.2000
        lcd.setCursor(3,1);
        lcd.print("  ");
        delay(500);
        break;
      }
  
      case 1:{
        lcd.setCursor(6,1);
        lcd.print("  ");
        delay(500);
        break;
      }
  
      default:{
        lcd.setCursor(9,1);
        lcd.print("    ");
        delay(500);
        break;
      }
    }

    lcd.setCursor(3, 1);
    lcd.print(rtc.getDateStr()); // XUất giá trị ngày thnág năm

    if (!digitalRead(Set_date)) { // Cái này là pooling
      while (!digitalRead(Set_date));
      Test_case++;
    }
    
    if (check_inter_0) { // Int tăng giá trị sẽ làm cái check_inter_0 nó thành true
      while (!digitalRead(Toggle_Up));

      if (Test_case == 0){ //Test_case 0 là ngày 1 là tháng 2 là năm
        if (ArrayOfValue[Test_case] == 31)
          ArrayOfValue[Test_case] = 1;
        else 
          ArrayOfValue[Test_case]++;
      }

      else if (Test_case == 1){
        if (ArrayOfValue[Test_case] == 12)
          ArrayOfValue[Test_case] = 1;
        else 
          ArrayOfValue[Test_case]++;
      }
      
      else
        ArrayOfValue[Test_case]++;
        
      rtc.setDate(ArrayOfValue[0], ArrayOfValue[1], ArrayOfValue[2]); // Set lại thời gian ds3231
      check_inter_0 = false; 
    }

    if (Test_case == 2){ // Int giảm giá trị (chỉ dùng cho năm)
      if (check_inter_1){
        while(!digitalRead(Set_time));
        ArrayOfValue[Test_case]--; 
      }
      rtc.setDate(ArrayOfValue[0], ArrayOfValue[1], ArrayOfValue[2]); // Set lại thời gian ds3231
      check_inter_1 = false;
    }
    
    if (!digitalRead(Set_date)) { // Cái này là pooling
      while (!digitalRead(Set_date));
      Test_case++;
    }
	}
 
  lcd.clear();
  lcd.print("Done!");
  delay(500);
  lcd.clear();
}

void set_dow_button(){
  Time Return_time;
  Return_time = rtc.getTime();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Day Of Week:");
  lcd.setCursor(5, 1);
  lcd.print(rtc.getDOWStr()); //Xuất thứ trong tuần lên lcd
  
  int Test_case = 0, dow_target = Return_time.dow; //Lấy giá trị day_of_week từ ds3231

  while (Test_case < 1){
    lcd.setCursor(0, 0);
    lcd.print("Set Day Of Week:");
    lcd.setCursor(5, 1);
    lcd.print(rtc.getDOWStr());//Xuất thứ trong tuần lên lcd
    
    if (!digitalRead(Toggle_Up)){
      while (!digitalRead(Toggle_Up));
      if (dow_target == 7)
        dow_target = 1;
      else
        dow_target++;
      lcd.clear();
    }
    
    rtc.setDOW(dow_target); // set lại thứ trong tuần cho ds3231 với dow_target = 1 là thứ 2 còn dow_target = 7 là CN

    if (!digitalRead(Set_date)){
      while (!digitalRead(Set_date));
      check_inter_0 = false;
      check_inter_1 = false;
      set_date_button(); // Set thứ r tới set ngày
      Test_case++;
    }
  }
}

void set_time_button(){
  Time Return_time;
  Return_time = rtc.getTime();
  
  int Test_case = 0;
  int ArrayOfValue[3] = {Return_time.hour, Return_time.min, Return_time.sec}; // Lấy thời gian giờ phút giây vào mảng

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Time:");
  lcd.setCursor(4, 1);
  lcd.print(rtc.getTimeStr());

	while (Test_case < 3){
    if (!digitalRead(Set_time)) {
      while (!digitalRead(Set_time)){ // Để lúc mà nhấn giữ cái nút thời gian vẫn chạy chứ k p đứng yên
        lcd.setCursor(4, 1);
        lcd.print(rtc.getTimeStr());

        Return_time = rtc.getTime();
        ArrayOfValue[0] = Return_time.hour;
        ArrayOfValue[1] = Return_time.min; 
        ArrayOfValue[2] = Return_time.sec;
      }
      Test_case++;
    }
    
    lcd.setCursor(4, 1);
    lcd.print(rtc.getTimeStr());
    delay (500);
    
    Return_time = rtc.getTime(); // Cập nhật lại thời gian
    ArrayOfValue[0] = Return_time.hour;
    ArrayOfValue[1] = Return_time.min; 
    ArrayOfValue[2] = Return_time.sec;
    
    switch (Test_case){ //Làm nhấp nháy như trên
      case 0:{
        lcd.setCursor(4,1);
        lcd.print("  ");
        delay(500);
        break;
      }
  
      case 1:{
        lcd.setCursor(7,1);
        lcd.print("  ");
        delay(500);
        break;
      }
  
      default:{
        lcd.setCursor(10,1);
        lcd.print("  ");
        delay(500);
        break;
      }
    }

    lcd.setCursor(4, 1);
    lcd.print(rtc.getTimeStr());
    
    if (!digitalRead(Set_time)) { //Pooling
      while (!digitalRead(Set_time)){
        lcd.setCursor(4, 1);
        lcd.print(rtc.getTimeStr());

        Return_time = rtc.getTime();
        ArrayOfValue[0] = Return_time.hour;
        ArrayOfValue[1] = Return_time.min; 
        ArrayOfValue[2] = Return_time.sec;
      }
      Test_case++;
    }
    
    
    
    if (check_inter_0) { // Nếu nhấn tăng giá trị
      while (!digitalRead(Toggle_Up)){
        lcd.setCursor(4, 1); // Cũng làm thời gian vẫn chạy khi nhấn giữ nút
        lcd.print(rtc.getTimeStr());

        Return_time = rtc.getTime();
        ArrayOfValue[0] = Return_time.hour;
        ArrayOfValue[1] = Return_time.min; 
        ArrayOfValue[2] = Return_time.sec;
      }
      
      if (Test_case == 0){ // Set giờ
        if (ArrayOfValue[Test_case] == 23)
          ArrayOfValue[Test_case] = 0;
        else 
          ArrayOfValue[Test_case]++;
      }

      else if (Test_case == 1){ // Set phút
        if (ArrayOfValue[Test_case] == 59)
          ArrayOfValue[Test_case] = 0;
        else 
          ArrayOfValue[Test_case]++;
      }
      
      else //Set giây
        ArrayOfValue[Test_case] = 0;
        
      rtc.setTime(ArrayOfValue[0], ArrayOfValue[1], ArrayOfValue[2]); // Cập nhật lại giá trị thời gian cho ds3231
      check_inter_0 = false;
    }
    
    Return_time = rtc.getTime(); //Cập nhật lại thời gian từ ds3231
    ArrayOfValue[0] = Return_time.hour;
    ArrayOfValue[1] = Return_time.min; 
    ArrayOfValue[2] = Return_time.sec;
    
    if (!digitalRead(Set_time)) { //pooling
      while (!digitalRead(Set_time)){
        lcd.setCursor(4, 1);
        lcd.print(rtc.getTimeStr());

        Return_time = rtc.getTime();
        ArrayOfValue[0] = Return_time.hour;
        ArrayOfValue[1] = Return_time.min; 
        ArrayOfValue[2] = Return_time.sec;
      }
      Test_case++;
    }
	}
 
  lcd.clear();
  lcd.print("Done!");
  delay(500);
  lcd.clear();
}



void time_display(){
  lcd.setCursor(0, 0);
  lcd.print(rtc.getDateStr()); //Xuất ngày tháng năm
  
  lcd.setCursor(13, 0);
  lcd.print(day_of_week_display()); //Xuất thứ rút gọn

  lcd.setCursor(0, 1);
  lcd.print(rtc.getTimeStr()); //Xuất thời gian
  
  lcd.setCursor(10, 1); 
  lcd.print(rtc.getTemp()); //Xuất nhiệt ođọ
  lcd.setCursor(14, 1);
  lcd.print((char)223);
  lcd.print("C");
  delay(1000);
}

void Init_button() {               
    pinMode(Toggle_Up, INPUT_PULLUP);
    pinMode(Set_date, INPUT_PULLUP);
    pinMode(Set_time, INPUT_PULLUP);
}

void Init_lcd(){
	lcd.init();               
	lcd.backlight();          
	lcd.clear();

  lcd.setCursor(3,0);
  lcd.print("CLOCK BOX!");
  lcd.setCursor(5,1);
  lcd.print("HEL_LO");
  delay(1000);
  lcd.clear();
}

void Init_rtc(){
	rtc.begin(); 

	//The following lines can be uncommented to set the date and time
  //rtc.setDOW(3);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
}

void setup() { 
	Serial.begin(9600);
 
 	Init_rtc();
	Init_lcd();
  Init_button();

  attachInterrupt(0, Toggle_Up_Value, FALLING); //Int_0 Kích cạnh xuống
  attachInterrupt(1, Toggle_Down_Value, FALLING); //Int_1 Kích cạnh xuống
}

void loop() { 
  if (!digitalRead(Set_time)){
    while (!digitalRead(Set_time));
    check_inter_0 = false;
    set_time_button();
    lcd.clear();
  }

  
  if (!digitalRead(Set_date)){
    while (!digitalRead(Set_date));
    check_inter_0 = false;
    check_inter_1 = false;
    set_dow_button();
    lcd.clear();
  }

  time_display();
}
