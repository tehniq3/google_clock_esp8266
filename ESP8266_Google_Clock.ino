// https://www.instructables.com/WiFi-NodeMCU-ESP8266-Google-Clock/

//V.3 - Data converted and daylight savings time

//V.4 - localized data, days... (use yours....)

//V.6 - thermometer and hygrometer

//V.7 - animated clock corrected + seconds +1,5

//V.8 - tuning thermometer and hygrometer

//v.9 - DST corrected / compiler error corrected (long)round

//v.10 - Date change at 00:00 corrected for GMT>0

//v.11 - automitc panel brightness (+3.3V ---|==10k==|---A0---|GND

// review: anthias64@gmail.com

// kudos original maker code can't find its name...

#include "Arduino.h"

#include

#include //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

WiFiClient client;

#define NUM_MAX 6

// for NodeMCU 1.0

#define DIN_PIN 13 // D7

#define CS_PIN 0 // D3

#define CLK_PIN 14 // D5

#include "max7219.h"

#include "fonts.h"

#include "DHT.h" // sensor DHT

// DHT sensor

#define DHTPIN 12 // what pin we're connected to // GPIO 12 = D6

// Uncomment whatever type you're using!

//#define DHTTYPE DHT11 // DHT 11

#define DHTTYPE DHT22 // DHT 22 (AM2302)

//#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE, 11); // for ESP8266

//DHT

float temperature = 0;

int temp = 0;

byte minus = 0;

int umidity = 0;

int poz, poz2;

int mult = 0;

float temp_offset = -1.0; // taratura termometro

float rh_offset = +3; // taratura igrometro

char TempLabel[] = " Temperatura: "; // Fixxed text

char UmiLabel[] = " Umidita': "; // Fixxed text

//modifica test ip

const char ssid[] = "xxxxxxxxxx"; // your network SSID (name)

const char password[] = "xxxxxxxxxxxxxxxxxxxxx"; // your network password

// Localized Months and DoWeek

String M_arr[13] = {" ", "Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"};

String Dow_arr[8] = {" ", "Lunedi", "Martedi", "Mercoledi", "Giovedi", "Venerdi", "Sabato", "Domenica"};

void setup()

{

Serial.begin(115200);

initMAX7219(); //

sendCmdAll(CMD_SHUTDOWN,1); //

sendCmdAll(CMD_INTENSITY,1); // brightness from 1 to 5

Serial.print("Connecting to ");

Serial.println(ssid);

WiFi.begin(ssid, password);

printStringWithShift("Connecting",30); //

delay (1000);

while (WiFi.status() != WL_CONNECTED) {

delay(500);

Serial.print(".");

}

Serial.print("IP number assigned by DHCP is ");

Serial.println(WiFi.localIP());

printStringWithShift((String(" IP:")+WiFi.localIP().toString()).c_str(), 30);//

delay(1000);//

// DHT

dht.begin();

temperature = dht.readTemperature() + temp_offset;

umidity = dht.readHumidity() + rh_offset;

Serial.print(temperature);

Serial.println(" gr.Celsius");

Serial.print(umidity);

Serial.println(" %RH");

if (temperature < 0)

{

temp = -10*temperature;

minus = 1;

}

else

{

minus = 0;

temp = 10*temperature;

}

}

// =============================DEFINE VARS==============================

#define MAX_DIGITS 20

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

float utcOffset = +1; // ==> your timezone

long localEpoc = 0;

long localMillisAtUpdate = 0;

int day, month, year, dayOfWeek;

int monthnum = 0;

int downum = 0;

int summerTime = 0;

String date;

String dayloc;

String monthloc;

// =======================================================================

void loop()

{

if(updCnt<=0) { // every 10 scrolls, ~450s=7.5m

updCnt = 60;

Serial.println("Getting data ...");

// clr();

printStringWithShift(" Get Time...",30);

getTime();

Serial.println("Data loaded");

clkTime = millis();

}

contrast(); // //autobrightness

//print data

if(millis()-clkTime > 30000 && !del && dots) { // clock for 30s, then scrolls for about 30s

printStringWithShift(" ",40); //Space before

printStringWithShift(date.c_str(),40); //data1

// Reading temperature or humidity

int umidity = dht.readHumidity()+ rh_offset;

float temperature = dht.readTemperature()+ temp_offset;

//umidity = umidity + rh_offset;

//temperature = temperature + temp_offset;

Serial.println("Temp e Umid corretti");

Serial.println(temperature);

Serial.println(umidity);

int t2 = 10*temperature;

// print temperature

// http://www.arduino-hacks.com/converting-integer-t...

char c[3], d[2];

String str1, str2;

int t2z = t2/10;

int t2u = t2 - t2z*10;

str1=String(t2z);

str1.toCharArray(c,3);

str2=String(t2u);

str2.toCharArray(d,2);

printStringWithShift(TempLabel ,40); // Send scrolling Text

printStringWithShift("+" ,40);

printStringWithShift(c ,40);

printStringWithShift("," ,40);

printStringWithShift(d ,40);

printStringWithShift(" C^" ,40);

//delay(2000);

// print umidity

// http://www.arduino-hacks.com/converting-integer-t...

char b[3];

String str;

str=String(umidity);

str.toCharArray(b,3);

printStringWithShift(UmiLabel ,40); // Send scrolling Text

printStringWithShift(b ,40);

printStringWithShift(" %" ,40);

//delay(2000);

printStringWithShift(" ",40); //Space after

delay(200);

updCnt--;

clkTime = millis();

}

if(millis()-dotTime > 500) {

dotTime = millis();

dots = !dots;

}

updateTime(); // get time

// print clock

showAnimClock();

//showSimpleClock();

}

// =======================================================================

void showSimpleClock()

{

dx=dy=0;

clr();

showDigit(h/10, 0, dig6x8);

showDigit(h%10, 8, dig6x8);

showDigit(m/10, 17, dig6x8);

showDigit(m%10, 25, dig6x8);

showDigit(s/10, 34, dig6x8);

showDigit(s%10, 42, dig6x8);

setCol(15,dots ? B00100100 : 0);

setCol(32,dots ? B00100100 : 0);

refreshAll();

}

// =======================================================================

void showAnimClock()

{

byte digPos[6]={0,8,17,25,34,42};

int digHt = 12;

int num = 6;

int i;

if(del==0) {

del = digHt;

for(i=0; i

dig[0] = h/10 ? h/10 : 10;

dig[1] = h%10;

dig[2] = m/10;

dig[3] = m%10;

dig[4] = s/10;

dig[5] = s%10;

for(i=0; i

} else

del--;

clr();

for(i=0; i

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

refreshAll();

delay(10); //default 30

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

int dualChar = 0;

unsigned char convertPolish(unsigned char _c)

{

unsigned char c = _c;

if(c==196 || c==197 || c==195) {

dualChar = c;

return 0;

}

if(dualChar) {

switch(_c) {

case 133: c = 1+'~'; break; // 'ą'

case 135: c = 2+'~'; break; // 'ć'

case 153: c = 3+'~'; break; // 'ę'

case 130: c = 4+'~'; break; // 'ł'

case 132: c = dualChar==197 ? 5+'~' : 10+'~'; break; // 'ń' and 'Ą'

case 179: c = 6+'~'; break; // 'ó'

case 155: c = 7+'~'; break; // 'ś'

case 186: c = 8+'~'; break; // 'ź'

case 188: c = 9+'~'; break; // 'ż'

//case 132: c = 10+'~'; break; // 'Ą'

case 134: c = 11+'~'; break; // 'Ć'

case 152: c = 12+'~'; break; // 'Ę'

case 129: c = 13+'~'; break; // 'Ł'

case 131: c = 14+'~'; break; // 'Ń'

case 147: c = 15+'~'; break; // 'Ó'

case 154: c = 16+'~'; break; // 'Ś'

case 185: c = 17+'~'; break; // 'Ź'

case 187: c = 18+'~'; break; // 'Ż'

default: break;

}

dualChar = 0;

return c;

}

switch(_c) {

case 185: c = 1+'~'; break;

case 230: c = 2+'~'; break;

case 234: c = 3+'~'; break;

case 179: c = 4+'~'; break;

case 241: c = 5+'~'; break;

case 243: c = 6+'~'; break;

case 156: c = 7+'~'; break;

case 159: c = 8+'~'; break;

case 191: c = 9+'~'; break;

case 165: c = 10+'~'; break;

case 198: c = 11+'~'; break;

case 202: c = 12+'~'; break;

case 163: c = 13+'~'; break;

case 209: c = 14+'~'; break;

case 211: c = 15+'~'; break;

case 140: c = 16+'~'; break;

case 143: c = 17+'~'; break;

case 175: c = 18+'~'; break;

default: break;

}

return c;

}

// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay) {

c = convertPolish(c);

if (c < ' ' || c > '~'+25) return;

c -= 32;

int w = showChar(c, font);

for (int i=0; i

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

}

// =======================================================================

void getTime()

{

WiFiClient client;

if (!client.connect("www.google.com", 80)) {

Serial.println("connection to google failed");

return;

}

client.print(String("GET / HTTP/1.1\r\n\n") +

String("Host: www.google.com\r\n\n") +

String("Connection: close\r\n\n\r\n\n"));

int repeatCounter = 0;

while (!client.available() && repeatCounter < 10) {

delay(500);

//Serial.println(".");

repeatCounter++;

}

String line;

client.setNoDelay(false);

while(client.connected() && client.available()) {

line = client.readStringUntil('\n');

line.toUpperCase();

// Serial.println(line); //test stringa

if (line.startsWith("DATE: ")) {

//date = " "+line.substring(6, 22);

date =line.substring(6, 22);

date.toUpperCase();

Serial.println(date); //check

int day = atoi (date.substring(5,7).c_str()); //day number

int year = 2000 + atoi (date.substring(13,16).c_str()); //year number

monthnum = month2index(date.substring(8,11)); // convert month in mumber

month = monthnum; //check

downum = dow2index(date.substring(0,3)); // convert dow in mumber

dayOfWeek = downum; //check

monthloc = M_arr[month]; // localize month

dayloc = Dow_arr[dayOfWeek]; // localize DayOfWeek

date = dayloc + " " + day + " " + monthloc + " " + year;

// decodeDate(date); //data3

h = line.substring(23, 25).toInt();

m = line.substring(26, 28).toInt();

s = line.substring(29, 31).toInt()+1.5; //correzione ora

// summerTime = checkSummerTime();

// calcolo ora legale

Serial.print("Ora GMT: " );

Serial.print(h); //check

Serial.print(" : " );

Serial.print(m); //check

Serial.print(" : " );

Serial.print(s); //check

Serial.println(""); //check

Serial.print("Data: " );

Serial.println(date); //check

//day = 31;

//year = 2021;

//month = 10;

if(month>3 && month<=10) summerTime=1;

if(month==3 && day>=31-(((5*year/4)+4)%7) ) summerTime=1;

if(month==10 && day>=31-(((5*year/4)+1)%7) ) summerTime=0;

Serial.print("Summertime: ");

Serial.println(summerTime);

unsigned short MoZiff[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; //Monatsziffer

unsigned short Tage_Monat[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

if(h+utcOffset+summerTime>23) {

// if(++day>31) { day=1; month++; }; // needs better patch

// if(++dayOfWeek>7) dayOfWeek=1;

h = h - 24;

dayOfWeek = dayOfWeek +1;

if(dayOfWeek>7) dayOfWeek=1;

day = day + 1;

if (day > Tage_Monat[month - 1]) {

day = 1;

month = month + 1;

if (month > 12) {

month = 1;

year = year + 1;

}

}

}

monthloc = M_arr[month]; // localize month

dayloc = Dow_arr[dayOfWeek]; // localize DayOfWeek

date = dayloc + ", " + day + " " + monthloc + " " + year;

localMillisAtUpdate = millis();

localEpoc = (h * 60 * 60 + m * 60 + s);

}

}

client.stop();

}

// =======================================================================

//convert month string in month number

int month2index (String month)

{

String months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

for (int i = 0; i < 12; i++) {

if (months[i] == month)

return i + 1;

}

return 0;

}

// =======================================================================

//convert dow string in dow number

int dow2index (String dayOfWeek)

{

String dows[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};

for (int i = 0; i < 12; i++) {

if (dows[i] == dayOfWeek)

return i + 1;

}

return 0;

}

// =======================================================================

//check if it is summer time

// int checkSummerTime()

//{

//if(month>3 && month<10) return 1;

//if(month==3 && day>=31-(((5*year/4)+4)%7) ) return 1;

//if(month==10 && day<31-(((5*year/4)+1)%7) ) return 0;

//return 0;

// return 1;

//}

// =======================================================================

void updateTime()

{

long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);

//long epoch = round(curEpoch + 3600 * (utcOffset+summerTime) + 86400L) % 86400L;

long epoch = (long)round(curEpoch + 3600 * (utcOffset+summerTime) + 86400L) % 86400L; // fix compiler error

//long epoch = round(curEpoch + 3600 * (utcOffset+summerTime) + 86400L);

h = ((epoch % 86400L) / 3600) % 24;

m = (epoch % 3600) / 60;

s = epoch % 60;

}

// =======================================================================

void contrast()

{

int lumina = analogRead(A0); //autobrightness (+3.3V ---|==10k==|---A0---|GND (niq_ro)

lumina = map(lumina, 0, 1023, 0, 4); //broghtness 0-4

//Serial.print("brightness = ");

//Serial.print(lumina);

//Serial.println (" / 4");

sendCmdAll(CMD_INTENSITY, lumina);

}

