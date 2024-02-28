#include <SolarCalculator.h>
#include <LiquidCrystal.h>
#include <virtuabotixRTC.h>
#include <LowPower.h>

int czytest = 0; //zmiana na debug z wyswietlania

int ustawienie_sekunda = 20; //zmiana sekund na RTC
int ustawienie_minuta = 26; //zmiana minut na RTC
int ustawienie_godzina = 18; //zmiana godziny na RTC
int ustawienie_dzientygodnia = 3; //zmiana dnia tygodnia na RTC
int ustawienie_dzienmiesiaca = 17; //zmiana daty na RTC
int ustawienie_miesiac = 1; //zmiana miesiaca na RTC
int ustawienie_rok = 2024; //zmiana roku na RTC

int poprawa_godziny = 0; //zmiana na 1 gdy trzeba poprawic date na RTC

double latitude = 51.1075; //zmiana szerokosci geograficznej
double longitude = 17.0625; //zmiana dlugosci geograficznej
int time_zone = +2; //zmiana strefy czasowej (nie uwzglednia czasu letniego/zimowego)

float jasnosc = 126;  //max jasnosc wyswietlacza
float ciemnosc = 10;  //min jasnosc wysweitlacza
int czy_godzina_zachodu = 0; //wewnetrzna zmienna, czy jest godzina zachodu

int zegarek_czy_muzyka = 0; //zmiana wyswietlania zegara
String is_paused = "True"; //wewnetrzna zmienna
String x = ""; //wewnetrzna zmienna
String song_name = ""; //wewnetrzna zmienna
String old_song_name = "a"; //wewnetrzna zmienna
String incoming_song_name = ""; //wewnetrzna zmienna
String song_length = ""; //wewnetrzna zmienna
int ktora_czesc = 0; //wewnetrzna zmienna

int korekta = 0; //wewnetrzna zmienna
int spanko = 0; //wewnetrzna zmienna
/*
 * The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
 * CONNECTIONS:
 * DS1302 CLK/SCLK --> 7
 * DS1302 DAT/IO --> 8
 * DS1302 RST/CE --> 9
 * DS1302 VCC --> 3.3v - 5v
 * DS1302 GND --> GND

https://docs.arduino.cc/learn/electronics/lcd-displays
*/

byte pause_symbol[8] = {
  0b00000,
  0b00000,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b00000,
  0b00000,
};

byte solaireL[8] = {
  0b10001,
  0b11001,
  0b11001,
  0b01101,
  0b01101,
  0b01101,
  0b00111,
  0b00111,
};

byte solaireF[8] = {
  0b11111,
  0b00000,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b00000,
  0b11111,
};

byte solaireR[8] = {
  0b10001,
  0b10011,
  0b10011,
  0b10110,
  0b10110,
  0b10110,
  0b11100,
  0b11100,
};

const int rs = 6, en = 12, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

virtuabotixRTC myRTC(7, 8, 9);

void setup() {
  
  myRTC.updateTime();

  lcd.begin(16, 2);

  Serial.begin(9600);

  pinMode(10, OUTPUT);
  pinMode(11, INPUT);
  pinMode(6, OUTPUT);
  pinMode(A5, INPUT);
  pinMode(A4, INPUT);

  if (poprawa_godziny == 1) { myRTC.setDS1302Time(ustawienie_sekunda, ustawienie_minuta, ustawienie_godzina, ustawienie_dzientygodnia, ustawienie_dzienmiesiaca, ustawienie_miesiac, ustawienie_rok); }

  lcd.createChar(0, solaireL);
  lcd.createChar(1, solaireF);
  lcd.createChar(2, solaireR);
  lcd.createChar(3, pause_symbol);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void (*resetFunc)(void) = 0;

void loop() 
{
  if (digitalRead(11) == 1)
  {
    switch(zegarek_czy_muzyka)
    {
      case 0:
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("ZMIANA NA");
        lcd.setCursor(5, 1);
        lcd.print("MUZYKE");
        zegarek_czy_muzyka = 1;
        delay(1000);
        lcd.clear();
        break;
    
      case 1:
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("ZMIANA NA");
        lcd.setCursor(5, 1);
        lcd.print("ZEGAR");
        zegarek_czy_muzyka = 0;
        delay(1000);
        lcd.clear();
        break;
    }
  }
  // Date
  int year = myRTC.year;
  int month = myRTC.month;
  int day = myRTC.dayofmonth;

  double transit, sunrise, sunset;

  // Calculate the times of sunrise, transit, and sunset, in hours (UTC)
  calcSunriseSunset(year, month, day, latitude, longitude, transit, sunrise, sunset);

  int m_sr = int(round(sunrise * 60));
  int hr_sr = (m_sr / 60) % 24;
  int mn_sr = m_sr % 60;

  int m_ss = int(round(sunset * 60));
  int hr_ss = ((m_ss / 60) % 24)-1;
  int mn_ss = m_ss % 60;

  if (myRTC.hours == hr_ss + time_zone) {
    czy_godzina_zachodu = 1;
    if (myRTC.minutes < mn_ss) { analogWrite(10, jasnosc); }
    if (myRTC.minutes > mn_ss) { analogWrite(10, ciemnosc); }
    if (myRTC.minutes == mn_ss) { analogWrite(10, (jasnosc + (ciemnosc - jasnosc) * myRTC.seconds / 59)); }
  } else {
    czy_godzina_zachodu = 0;
  }
  if (myRTC.hours > hr_ss + time_zone || myRTC.hours < hr_sr + time_zone && czy_godzina_zachodu == 0) { analogWrite(10, ciemnosc); }
  if (myRTC.hours >= hr_sr + time_zone && myRTC.hours < hr_ss + time_zone && czy_godzina_zachodu == 0) { analogWrite(10, jasnosc); }

  String dzientygodnia = "";

  switch (myRTC.dayofweek) {
    case 1:
      dzientygodnia += "Poniedzialek ";
      break;

    case 2:
      dzientygodnia += "Wtorek";
      break;

    case 3:
      dzientygodnia += "Sroda";
      break;

    case 4:
      dzientygodnia += "Czwartek";
      break;

    case 5:
      dzientygodnia += "Piatek";
      break;

    case 6:
      dzientygodnia += "Sobota";
      break;

    case 7:
      dzientygodnia += "Niedziela";
      break;
  }

  String data = "";

  myRTC.updateTime();

  if (myRTC.hours < 10) {
    data += "0";
    data += myRTC.hours;
  } else {
    data += myRTC.hours;
  }

  data += ":";

  if (myRTC.minutes < 10) {
    data += "0";
    data += myRTC.minutes;
  } else {
    data += myRTC.minutes;
  }

  data += ":";

  if (myRTC.seconds < 10) {
    data += "0";
    data += myRTC.seconds;
  } else {
    data += myRTC.seconds;
  }

  data += " ";
  data += " ";
  data += " ";

  if (myRTC.dayofmonth < 10) {
    data += "0";
    data += myRTC.dayofmonth;
  } else {
    data += myRTC.dayofmonth;
  }

  data += ".";

  if (myRTC.month < 10) {
    data += "0";
    data += myRTC.month;
  } else {
    data += myRTC.month;
  }

  switch(zegarek_czy_muzyka)
  {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(data);

      lcd.setCursor(0, 1);

      if (myRTC.hours == 21 && myRTC.minutes == 37) {
        if(myRTC.hours == 21 && myRTC.minutes == 37 && myRTC.seconds <= 2)
        {
          lcd.clear();
        }
        lcd.print("PAPAJOWA");
      }

      else
      {
        switch (czytest) {

          case 0:
          
            if (myRTC.hours == 21 && myRTC.minutes == 38 && myRTC.seconds == 10) {lcd.clear();}

            lcd.print(dzientygodnia);

            if (myRTC.dayofweek == 1) {
              lcd.setCursor(13, 1);
            } else {
              lcd.setCursor(12, 1);
            }
            lcd.write(byte(0));
            lcd.write(byte(1));
            lcd.write(byte(2));
            break;

          case 1:
            lcd.setCursor(0, 1);
            lcd.print(hr_ss + time_zone);
            lcd.write(":");
            if (mn_ss == 0) {
              lcd.print("00");
            } else {
              lcd.print(mn_ss);
            }
            lcd.write(" ");
            lcd.print(czy_godzina_zachodu);
            lcd.write(" ");
            lcd.print(jasnosc + (ciemnosc - jasnosc) * myRTC.seconds / 59);
            break;
        }
      }
    break;

    case 1:
    
        x = Serial.readString();
        incoming_song_name = getValue(x,'~',0);
        if(incoming_song_name != old_song_name)
        {
          ktora_czesc = 0;
          song_name = incoming_song_name;
          if (song_name.length() > 16)
          {
            song_name += "    ";
            old_song_name = incoming_song_name;
          }
        else
        old_song_name = incoming_song_name;
        }
        else
        song_length = getValue(x,'~',1);
        is_paused = getValue(x,'~',2);
        if (song_name.length() > 16)
        {
          if (abs(ktora_czesc+16) >= song_name.length())
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(song_name.substring(ktora_czesc,(ktora_czesc+16)));
            lcd.setCursor(song_name.length()-ktora_czesc,0);
            lcd.print(song_name.substring(0,16-song_name.length()-ktora_czesc));
            if (ktora_czesc == song_name.length()){ktora_czesc = 0;}
          }
          else 
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(song_name.substring(ktora_czesc,(ktora_czesc+16)));
          }
        }
        else 
        {
          lcd.clear();
          lcd.setCursor(round((16-song_name.length())/2), 0);
          lcd.print(song_name);
        }
        lcd.setCursor(0, 1);
        lcd.print(song_length);

        if (is_paused == "False")
        {
          lcd.print(" ");
          lcd.write(byte(3));
        }
        if (is_paused == "True")
        {
          lcd.print("  ");
        }
        ktora_czesc++;
    break;
  }
  if (myRTC.hours == 23 && myRTC.minutes == 59 && myRTC.seconds >= 30) 
    {
      korekta = 1;
      czy_godzina_zachodu = 1;
    }

  if (myRTC.hours == 0 && myRTC.minutes == 0 && myRTC.seconds >= 6 && korekta == 1) 
    {
      lcd.clear();
      myRTC.DS1302_write(0x80, 0);
      korekta = 0;
    }

  if (myRTC.hours == 4 && myRTC.minutes == 10 && myRTC.seconds == 10) 
  {
    lcd.noDisplay();

    for (int spanko = 0; spanko <= 10; spanko++) {
      LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    }
  }

  if (myRTC.hours == 4 && myRTC.minutes >= 50 && myRTC.seconds >= 10) 
  {
    lcd.display();
    lcd.clear();
    spanko = spanko * 0;
  }
}
