/*
v.0.0 - original clock - https://www.hackster.io/mircemk/esp8266-animated-clock-on-8x8-led-matrices-4867ae
v.1.0 - extract data by Nicu FLORICA (niq_ro)
v.1.a - added data on Serial Monitor, extract as in https://github.com/DIYDave/HTTP-DateTime library
v.1.b - added data on display
v.1.c - test real DS18B20
v.1.c1  small updates
v.1.d - test 12-hour format
v.1.e - update info as in DIYDave library for extract date
v.1.f - update info at every 10 minutes (0,10,20,30,40,50) + temperature stay a little
v.1.g - added name of the day and month (romainan and english language) 
*/

#include "Arduino.h"
#include <ESP8266WiFi.h>
//#include <HttpDateTime.h>            // https://github.com/DIYDave

#include <OneWire.h>
#include <DallasTemperature.h>
#define SENSOR_RESOLUTION 10
const int oneWireBus = 2;  // D4
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

char result[4];

WiFiClient client;

String date;

#define NUM_MAX 4
#define LINE_WIDTH 16
#define ROTATE  90

// for NodeMCU 1.0
#define DIN_PIN 15  // D8
#define CS_PIN  13  // D7
#define CLK_PIN 12  // D6

#include "max7219.h"
#include "fonts.h"

// =======================================================================
// CHANGE YOUR CONFIG HERE:
// =======================================================================
const char* ssid     = "niq_ro";        // SSID WiFi nam
const char* password = "fewbeers1,2,3";         // WiFi password

float utcOffset = 2;                        // Timezone offset (in hour)
byte format1224 = 12;                       // 12/24,  12 -hour format or 24-hour format
// =======================================================================

int realDS = 1;  // no DS18B20 sensor -> 0, real sensor = 1
float temperatureC;

#define MAX_DIGITS 16
byte dig[MAX_DIGITS]={0};
byte digold[MAX_DIGITS]={0};
byte digtrans[MAX_DIGITS]={0};
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
int dx=0;
int dy=0;
byte del=0;
int h,m,s;

int ziua,zi,luna,luna2,an;
String luna1,ziua1;
long localEpoc = 0;
long localMillisAtUpdate = 0;

byte ampm = 0;                // 0 = AM, 1 = PM
int h1, h0; // hour to print
int h7, h3;

byte preluare = 0;
int lenghdrc;
int lenghdrc1;


//Week Days (english)
String weekDays0[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names (english)
String months0[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
//Week Days (romanian)
String weekDays1[7]={"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata"};
//Month names (romanian)
String months1[12]={"Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie", "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"};





void setup() 
{
  Serial.begin(115200);
  Serial.println("=");
  Serial.println("=======================================================================");
  sensors.begin();
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);
  sendCmdAll(CMD_INTENSITY,0); // Dot Matrix Parlaklık Ayarı 0-15
 
  WiFi.begin(ssid, password);
  printStringWithShift("Google clock.......v.1.g......",38);
  while (WiFi.status() != WL_CONNECTED) 
    {
    delay(250);
    }
}
// =======================================================================


// =======================================================================
void loop()
{  
  /*
  if(updCnt<=0) 
    { // every 10 scrolls, ~450s=7.5m
    updCnt = 10;
    getTime();
    clkTime = millis();
  }
  */
    if ((m%10 == 0) and (preluare == 0))
   {
    getTime();
    clkTime = millis();
    preluare = 1;
    Serial.println("->");
   }
  if ((m%10 >= 9) and (preluare == 1))
   {
    preluare = 0;
    Serial.println("<-");
   } 
   
  if(millis()-clkTime > 30000 && !del && dots) 
  {
    if (realDS == 1)
    {
    sensors.requestTemperatures(); 
    temperatureC = sensors.getTempCByIndex(0);
    Serial.print("DS18B20 -> ");
    Serial.print(temperatureC);
    Serial.println("*C");
    }
    else
    temperatureC = (float)(random(-200,300)/10.);   
    dtostrf(temperatureC, 4, 1, result);
    String derece;
    if (format1224 == 12)
    {
    if (ampm == 0) 
    derece += "AM /";
    else
    if (ampm == 1)
    derece += "PM /";
    }
    //derece += " temperature = ";
     if (m%2 == 0) 
    {
      derece += " temperature = ";
    }
    else
    {
      derece += " temperatura = ";
    }
    
    if (temperatureC > 0) derece += " ";
    derece += result;
    derece += "*C ";
    lenghdrc = derece.length();
    char Buf[lenghdrc]; //    char Buf[8];
    derece.toCharArray(Buf,lenghdrc); // derece.toCharArray(Buf,8);   
    printStringWithShift(Buf,50); 
    delay(3000);
    String derece1;
    derece1 += " / ";
    if (m%2 == 0) 
    {
      derece1 += weekDays0[ziua%7];
    }
    else
    {
      derece1 += weekDays1[ziua%7];
    }
    derece1 += ", "; 
    derece1 += zi/10;
    derece1 += zi%10;
    derece1 += "-";
    /*
    derece1 += luna/10;
    derece1 += luna%10;
    */
    if (m%2 == 0)
    {
      derece1 += months0[luna-1];
    }
    else
    {
      derece1 += months1[luna-1];
    }   
    derece1 += "-"; 
    derece1 += an;
    derece1 += "             ";
    lenghdrc1 = derece1.length();
    char Buf1[lenghdrc1]; //    char Buf[8];
    derece1.toCharArray(Buf1,lenghdrc1); // derece.toCharArray(Buf,8);   
    printStringWithShift(Buf1,50);

    delay(200);
  //  updCnt--;
    clkTime = millis();
}

  if(millis()-dotTime > 500) {
    dotTime = millis();
    dots = !dots;
  }
  updateTime();

  if (format1224 == 24)
  {
   h1 = h;
  }
  if (format1224 == 12)
  {
  if (h/12 == 1) 
    ampm = 1;
  else
    ampm = 0;
  h1 = h%12;
  if (h1 == 0) h1 = 12;   
  }
  
  showAnimClock();
 // showSimpleClock();
    
  // Adjusting LED intensity.
  // 12am to 6am, lowest intensity 0
  //if ( (h == 0) || ((h >= 1) && (h <= 6)) ) sendCmdAll(CMD_INTENSITY, 0);
  //6pm to 11pm, intensity = 2
  //else if ( (h >=7) && (h <= 23) ) sendCmdAll(CMD_INTENSITY, 1);
  // max brightness during bright daylight
  //else sendCmdAll(CMD_INTENSITY, 2);
 
delay(100); 
}  // end main loop

// =======================================================================

void showAnimClock()
{
  
  byte digPos[6]={1,8,17,24,34,41};
  int digHt = 12;
  int num = 6; 
  int i;

  if(del==0) {
    del = digHt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    dig[0] = h1/10; 
    dig[1] = h1%10;
    dig[2] = m/10;
    dig[3] = m%10;
    dig[4] = s/10;
    dig[5] = s%10;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : digHt;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digPos[i], dig6x8);
    } else {
      dy = digHt-digtrans[i];
      showDigit(digold[i], digPos[i], dig6x8);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig6x8);
      digtrans[i]--;
    }
  }
  dy=0;
  setCol(15,dots ? B00100100 : 0);
  setCol(32,dots ? B00100100 : 0);
  if (format1224 == 12)
  {
  if (ampm == 1)
   setCol(0, ampm ? B00000011 : 0);
  if (h1/10 == 0)
  {
   setCol(1, 0);
   setCol(2, 0);
   setCol(3, 0);
   setCol(4, 0);
   setCol(5, 0);
   setCol(6, 0);
  }
  }
  refreshAll();
 delay(30);
}

// =======================================================================

void showDigit(char ch, int col, const uint8_t *data)
{
  if(dy<-8 | dy>8) return;
  int len = pgm_read_byte(data);
  int w = pgm_read_byte(data + 1 + ch * len);
  col += dx;
  for (int i = 0; i < w; i++)
    if(col+i>=0 && col+i<8*NUM_MAX) {
      byte v = pgm_read_byte(data + 1 + ch * len + 1 + i);
      if(!dy) scr[col + i] = v; else scr[col + i] |= dy>0 ? v>>dy : v<<-dy;
    }
}

// =======================================================================

void setCol(int col, byte v)
{
  if(dy<-8 | dy>8) return;
  col += dx;
  if(col>=0 && col<8*NUM_MAX)
    if(!dy) scr[col] = v; else scr[col] |= dy>0 ? v>>dy : v<<-dy;
}

// =======================================================================

int showChar(char ch, const uint8_t *data)
{
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8 + i] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  scr[NUM_MAX*8 + i] = 0;
  return w;
}

// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay) {
  
  if (c < ' ' || c > '~'+25) return;
  c -= 32;
  int w = showChar(c, font);
  for (int i=0; i<w+1; i++) {
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}

// =======================================================================


void printStringWithShift(const char* s, int shiftDelay){
  while (*s) {
    printCharWithShift(*s, shiftDelay);
    s++;
  }
//delay(2000);
}
// =======================================================================



void getTime()
{
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
    Serial.println("connection to google failed");
    return;
  }

  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println(".");
    repeatCounter++;
    
  }

  String line;
  client.setNoDelay(false);
  while(client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    //Serial.println(line);
    if (line.startsWith("DATE: ")) {
      //date = "     "+line.substring(6, 17);
      date = ""+line.substring(6, 22); //17);
      ziua1 = line.substring(6, 9);
      luna1 = line.substring(14, 17);
      luna2 = line.substring(14, 17).toInt();
      zi = line.substring(11, 13).toInt();
      an = line.substring(18, 22).toInt();
      h0 = line.substring(23, 25).toInt();
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt();
      zitozi();
      lunatoluna();
      localMillisAtUpdate = millis();
      localEpoc = (h0 * 60 * 60 + m * 60 + s);
      Serial.print(date);
      Serial.print(" -> ");
      Serial.print(ziua1);
      Serial.print(" (");  
      Serial.print(ziua);
      Serial.print("), ");      
      Serial.print(zi/10);
      Serial.print(zi%10);
      Serial.print("-");
      Serial.print(luna1);
      Serial.print(" (");  
      Serial.print(luna);
      Serial.print(") - ");  
      Serial.print(an);      
      Serial.print(", ");
      Serial.print(h0/10);
      Serial.print(h0%10);
      Serial.print(":");
      Serial.print(m/10);
      Serial.print(m%10);
      Serial.print(":");
      Serial.print(s/10);
      Serial.println(s%10);    
    }
  }
  client.stop();

  long epoch1 = fmod(round(localEpoc + 3600 * utcOffset + 86400L), 86400L);
  h3 = ((epoch1  % 86400L) / 3600) % 24;
  Serial.print(h3); 
  Serial.print(" -> ");
  Serial.println(h0); 

// In case of positive timezone offset 
if (utcOffset > 0)
{  
 //   if (h3  > 23)
    if (h3 < h0) 
    {
    zi++;
    ziua++;
    if (ziua > 7) ziua = 1;
    }
  
    if (zi == 29)
      if (luna == 2 && an % 4 !=0){   // Kein Schaltjahr = 28 Tage
        luna = 3;
        zi = 1;
      }          
    if (zi == 30)
      if (luna == 2 && an % 4 ==0){   // Schaltjahr = 29 Tage
        luna = 3;
        zi = 1;
      } 
     if (zi == 31)
      if (luna == 4 || luna == 6 || luna == 9 || luna == 11){   // 30 tage
        luna++;
        zi = 1;     
      }
     if (zi == 32)
      if (luna == 1 || luna == 3 || luna == 5 || luna == 7 || luna == 8 || luna == 10){   // 31 tage
        luna++;
        zi = 1;   
      }else 
      {           
        if (luna == 12){
          luna = 1;
          zi = 1; 
          an++;
        }                 
      }
  }

// In case of negative timezone offset 
  if (utcOffset < 0)
  {
    zi--;
    ziua--;
    if (ziua < 1) ziua = 7;
  }
  if (zi < 1){
    luna--;
    if (luna < 1)
    {
      luna = 12;    
      an--;           
    }
    
      if (luna == 1)
      {
        zi = 31;
      }
      if (luna == 2)
      {
        if (an % 4 !=0) zi = 28;   //Kein Schaltjahr
        if (an % 4 ==0) zi = 29;   // Schaltjahr
      }
      if (luna == 3)
      {
        zi = 31;
      }
      if (luna == 4)
      {
        zi = 30;
      }
      if (luna == 5)
      {
        zi = 31;
      }
      if (luna == 6)
      {
        zi = 30;
      }
      if (luna == 7)
      {
        zi = 31;
      }
      if (luna == 8)
      {
        zi = 31;
      }
      if (luna == 9)
      {
        zi = 30;
      }
      if (luna == 10)
      {
        zi = 31;
      }
      if (luna == 11)
      {
        zi = 30;
      }
      if (luna == 12)
      {
        zi = 31;
      }
  }  


      Serial.print("Offfset = ");
      Serial.print(utcOffset);
      Serial.print(" -> ");
      Serial.print(ziua1);
      Serial.print(" (");  
      Serial.print(ziua);
      Serial.print("), ");      
      Serial.print(zi/10);
      Serial.print(zi%10);
      Serial.print("-");
      Serial.print(luna1);
      Serial.print(" (");  
      Serial.print(luna);
      Serial.print(") - ");  
      Serial.print(an);      
      Serial.print(", ");
      Serial.print(h3/10);
      Serial.print(h3%10);
      Serial.print(":");
      Serial.print(m/10);
      Serial.print(m%10);
      Serial.print(":");
      Serial.print(s/10);
      Serial.println(s%10);  
 
}

// =======================================================================

void updateTime()
{
  long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);
  long epoch = fmod(round(curEpoch + 3600 * utcOffset + 86400L), 86400L);
  h = ((epoch  % 86400L) / 3600) % 24;
  m = (epoch % 3600) / 60;
  s = epoch % 60; 
}

// =======================================================================

void zitozi()
{
  if (ziua1 == "MON") ziua = 1;
  else
  if (ziua1 == "TUE") ziua = 2;
  else
  if (ziua1 == "WED") ziua = 3;
  else
  if (ziua1 == "THU") ziua = 4;
  else
  if (ziua1 == "FRI") ziua = 5;
  else
  if (ziua1 == "SAT") ziua = 6;
  else 
  if (ziua1 == "SUN") ziua = 7;
}

void lunatoluna()
{
  if (luna1 == "JAN") luna = 1;
   else
  if (luna1 == "FEB") luna = 2;
   else
  if (luna1 == "MAR") luna = 3;
   else
  if (luna1 == "APR") luna = 4;
   else
  if (luna1 == "MAY") luna = 5;
   else
  if (luna1 == "JUN") luna = 6;
   else
  if (luna1 == "JUL") luna = 7;
   else
  if (luna1 == "AUG") luna = 8;
   else
  if (luna1 == "SEP") luna = 9;
   else
  if (luna1 == "OCT") luna = 10;
   else
  if (luna1 == "NOV") luna = 11; 
   else
  if (luna1 == "DEC") luna = 12;   
}

void showDayMonth()
{ 
  byte digPos[6]={1,8,17,24,34,41};
  int digHt = 12;
  int num = 6; 
  int i;
  if(del==0) {
    del = digHt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    //dig[0] = h/10 ? h/10 : 10;
    dig[0] = ziua/10; 
    dig[1] = ziua%10;
    dig[2] = luna/10;
    dig[3] = luna%10;
 //   dig[4] = s/10;
 //   dig[5] = s%10;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : digHt;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digPos[i], dig6x8);
    } else {
      dy = digHt-digtrans[i];
      showDigit(digold[i], digPos[i], dig6x8);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig6x8);
      digtrans[i]--;
    }
  }
  dy=0;
  setCol(15,dots ? B00100100 : 0);
//  setCol(32,dots ? B00100100 : 0);
  refreshAll();
// delay(30);
}
