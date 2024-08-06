#include <Wire.h>
#include <LiquidCrystal.h>

// Define the LCD and its pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // RS, EN, D4, D5, D6, D7

const int bellTimes[8][2] = {
  {9, 0},   // 9:00 AM
  {10, 0},  // 10:00 AM
  {11, 0},  // 11:00 AM
  {12, 0},  // 12:00 PM
  {12, 20}, // 12:20 PM
  {13, 0},  // 1:00 PM
  {13, 30}, // 1:30 PM
  {14, 15}  // 2:15 PM
};

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};// we are taking the days as 1, 2,3,4,5,6,7


const int nationalholiday[][2]
  {
    {1, 1},    // New Year's Day
    {15, 1},   // Pongal
    {16, 1},   // pongal holidays
    {17, 1},
    {18, 1},  // pongal holidays
    {26, 1},   // Republic Day
    {15, 8},   // Independence Day
    {2, 10},   // Gandhi Jayanti
    {25, 12}, // Crismas
  };


void setup() {
  lcd.begin(16, 2);
  Wire.begin();
  DS1307_Write(0, 12, 30, 1, 1, 7, 24); // Sec, Min, Hour, Day, Date, Month, Year 

  pinMode(12, OUTPUT); // Buzzer pin
  digitalWrite(12, LOW); // Initially low
}

uint8_t decimal_to_bdc(char value) {// To change the value that we are receiving from the DS1307 as a decimal to binary
  uint8_t msb, lsb, hex;
  msb = value / 10;
  lsb = value % 10;
  hex = ((msb << 4) + lsb);
  return hex;
}

void DS1307_Write(char _second, char _minute, char _hour, char _day, char _date, char _month, char _year) {
  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.write(decimal_to_bdc(_second));
  Wire.write(decimal_to_bdc(_minute));
  Wire.write(decimal_to_bdc(_hour));
  Wire.write(decimal_to_bdc(_day));
  Wire.write(decimal_to_bdc(_date));
  Wire.write(decimal_to_bdc(_month));
  Wire.write(decimal_to_bdc(_year));
  Wire.endTransmission();
}

uint8_t DS1307_Read(char addr) { //  address 
  char data;
  Wire.beginTransmission(0x68);
  Wire.write(addr);
  Wire.endTransmission();
  delay(5);

  Wire.requestFrom(0x68, 1); // request data from the DS1307

  if (Wire.available()) { // if communication bus is available
    data = Wire.read(); // the input we are giving is stored in the location data
    data = ((data >> 4) * 10) + (data & 0x0F); // converting the hex to binary by masking it
  }
  return data;
}

void loop() {
  int day = DS1307_Read(0x03); // Read day of the week
  int hour = DS1307_Read(0x02); // Read hour
  int minute = DS1307_Read(0x01); // Read minute
  int date = DS1307_Read(0x04); // Read date
  int month = DS1307_Read(0x05); // Read month
  int i;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(daysOfTheWeek[day - 1]);
  lcd.setCursor(0, 1);
  lcd.print(minute);
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(3, 1);
  lcd.print(hour);

  if (day == 1 ) { // which is Sunday according to our code  
    //lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("HOLIDAY");
    digitalWrite(12, LOW);
    delay(60000); // check for every 1 min
    return;
   
  } 
  else {
    for (int i = 0; i < 8; i++) {
      if (hour == bellTimes[i][0] && minute == bellTimes[i][1]) {
        ringBell();
      }
    }
  }
  delay(1000); // Check every second
}

void leave()
{
int i;  
int day;
int month;
 for (i = 0; i <sizeof(nationalholiday) / sizeof(nationalholiday[1]); i++)
    {
      if (nationalholiday[i][0] == day && nationalholiday[i][1] == month) 
      {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("------------");
    lcd.setCursor(3, 1);
    lcd.print("NATIONAL HOLIDAY");
    digitalWrite(12, LOW);
    delay(60000); // check for every 1 min
    return;
    }
}
};

void ringBell() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Bell is Ringing");

  digitalWrite(12, HIGH);
  delay(10000); // Ring the bell for 5 seconds
  digitalWrite(12, LOW);
}
