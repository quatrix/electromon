// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"             // Include Emon Library
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial esp8266_serial(2,3);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
EnergyMonitor emon1;             // Create an instance

#define WIFI_SSID "evilnet"
#define WIFI_PASS "evil@me4ever"

void get(String dst_ip) {
    String cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += dst_ip;
    cmd += "\",80";
    esp8266_serial.println(cmd);

    Serial.println(cmd);

    if(esp8266_serial.find("Error"))
         return;

    cmd = "GET / HTTP/1.0\r\n\r\n";
    esp8266_serial.print("AT+CIPSEND=");
    esp8266_serial.println(cmd.length());

    if(esp8266_serial.find(">")) {
        Serial.print(">");
    } else {
        esp8266_serial.println("AT+CIPCLOSE");
        Serial.println("connect timeout");
        delay(1000);
        return;
    }

    esp8266_serial.print(cmd);
    delay(2000);
    while (esp8266_serial.available()) {
        char c = esp8266_serial.read();
        Serial.write(c);
        delay(10);
        if(c=='\r') Serial.print('\n');
    }

    Serial.println("====");
    delay(1000);
}

boolean connectWIFI() {
    esp8266_serial.println("AT+CWMODE=1");

    String cmd="AT+CWJAP=\"";
    cmd+=WIFI_SSID;
    cmd+="\",\"";
    cmd+=WIFI_PASS;
    cmd+="\"";

    esp8266_serial.println(cmd);
    Serial.println(cmd);
    delay(5000);

    if (!esp8266_serial.find("OK"))
        return false;

    return esp8266_serial.find("GOT IP");
}

void setup()
{  
    lcd.begin(20,4);   // initialize the lcd for 16 chars 2 lines, turn on backlight

    Serial.begin(9600);
    esp8266_serial.begin(115200);
    esp8266_serial.setTimeout(1000);

/*
    while(!Serial);
    while(!esp8266_serial);

    while(esp8266_serial.available()>0)
        esp8266_serial.read();

    delay(1000);

    esp8266_serial.println("AT+RST");
    Serial.println("resetting esp");
    esp8266_serial.flush();

    if (esp8266_serial.find("ready")) {
        Serial.println("esp ready");
    } else {
        Serial.println("esp not ready");
        while(1);
    }

    delay(1000);

    if (!connectWIFI()) {
        Serial.println("can't connect wifi");
        while(1);
    }

    Serial.println("connected to wifi");

*/


    //emon1.voltage(2, 94, 0.5);  // Voltage: input pin, calibration, phase_shift
    emon1.voltage(2, 219, 1.7);  // Voltage: input pin, calibration, phase_shift
    emon1.current(1, 16.15);       // Current: input pin, calibration.

}


void writeToLCD(String msg, int line) {
    lcd.setCursor(0, line); //Start at character 4 on line 0
    lcd.print(msg);
}


void loop() {
    get("207.210.101.146");
    delay(1000);
/*
    if(esp8266_serial.available()) {  
        while(esp8266_serial.available()) {  
            char c = esp8266_serial.read(); // read the next character.  
            Serial.write(c);  
        }   
    }  
    if(Serial.available())  {  
        // the following delay is required because otherwise the arduino will read the first letter of the command but not the rest  
        // In other words without the delay if you use AT+RST, for example, the Arduino will read the letter A send it, then read the rest and send it  
        // but we want to send everything at the same time.  
        delay(1000);   
        String command="";  

        while(Serial.available()) {  
            command+=(char)Serial.read();  
        }  

        esp8266_serial.println(command); // send the read character to the esp8266_serial  
    }  

*/
/*
    emon1.calcVI(100,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
    //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

    float realPower       = emon1.realPower;        //extract Real Power into variable
    float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
    float powerFactor     = emon1.powerFactor;      //extract Power Factor into Variable
    float Vrms             = emon1.Vrms;             //extract Vrms into Variable
    float Irms            = emon1.Irms;             //extract Irms into Variable

    lcd.clear();
    writeToLCD("Power(R): " + String(realPower), 0);
    writeToLCD("Power(A): " + String(apparentPower), 1);
    writeToLCD("V(rms):   " + String(Vrms), 2);
    writeToLCD("I(rms):   " + String(Irms), 3);

*/
}
