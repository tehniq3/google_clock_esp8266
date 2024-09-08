# google_clock_esp8266
based on https://www.hackster.io/mircemk/esp8266-animated-clock-on-8x8-led-matrices-4867ae and used date from https://github.com/DIYDave/HTTP-DateTime library

my articles:
- https://nicuflorica.blogspot.com/2024/01/ceas-google-cu-data-si-termometru.html
- https://nicuflorica.blogspot.com/2024/02/ceas-google-cu-data-si-termometru-2.html
- https://nicuflorica.blogspot.com/2024/02/ceas-google-cu-data-si-termometru-3.html
- https://nicuflorica.blogspot.com/2024/02/ceas-google-cu-data-si-termometru-4.html
- https://nicuflorica.blogspot.com/2024/02/ceas-google-cu-data-si-termometru-5.html
- https://nicuflorica.blogspot.com/2024/02/ceas-google-cu-data-si-termometru-6.html
- https://nicuflorica.blogspot.com/2024/09/ceas-google-cu-data-si-termometru-cu-2.html

![schematic](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEiUbbcfAkt_eu_Qcr3FFZyJTzQFFvn9oyg6h2Wj00ItHGPsTrbgbiatz2Ambc7Rm_9ei00_GP8AF35JAPAOYSjBiyd-3ZeXwU2j-t-ZhEamPxpnsVwgCRPKHQ5vAoRuI_owFu3ONDxgRvE37yPSaRu0MPr-z96m33-IUQvIKK1dGG7THcYJTSdItHl-1Mq9/s320/google_clock_ds18b20_4xmax7219_sch.png)
![real](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEhhwm1h5fWPwpqZsKe_kxef5aCpKUEG32updz8K_1n3SFZoryvw28Sml7uMRIQ-vVQsI7w7XQeiGeQHGKQ2ZzMp3Ps78Na316ZsVmSAWppgyugWxO_KP_vU168LqfmIhyphenhyphen_GfRl29LluR8OOkH-jDzfUIIvBZL9jG75ezRXBXbw8E14rHmPNUjaKItodSp-2/w200-h150/IMG_20240121_192639.jpg)

history:
- v.0.0 - original clock - https://www.hackster.io/mircemk/esp8266-animated-clock-on-8x8-led-matrices-4867ae
- v.1.0 - extract data by Nicu FLORICA (niq_ro)
- v.1.a - added data on Serial Monitor, extract as in https://github.com/DIYDave/HTTP-DateTime library
- v.1.b - added data on display
- v.1.c - test real DS18B20
- v.1.c1  small updates
- v.1.d - test 12-hour format
- v.1.e - update info as in DIYDave library for extract date
- v.1.f - update info at every 10 minutes (0,10,20,30,40,50) + temperature stay a little
- v.1.g - added name of the day and month (romainan and english language)
- v.1.h - added automatic DST (summer/winter time) using info from https://www.timeanddate.com/time/change/germany and in special from https://www.instructables.com/WiFi-NodeMCU-ESP8266-Google-Clock/
- v.1.i - added 2nd sensor (DS18B20) as at TwoPin_DS18B20.ino
- v.1.j - added WiFiManager with few custom parameters, as I found at https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
- v.1.k - added automatic adjusted for brightness as at https://nicuflorica.blogspot.com/2020/04/alta-versiune-de-ceas-animat-cu-date.html
- v.1.l - changed exterior sensor to DHT22 (insted DS18B20) - // https://www.arduino.cc/reference/en/libraries/dht-sensor-library-for-espx/
