// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"             // Include Emon Library
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial esp8266_serial(2,3);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
EnergyMonitor emon1;             // Create an instance

String genStatsRequest(float realPower, float apparentPower, float powerFactor, float Vrms, float Irms) {
    String cmd = String("GET /input?json={");

    cmd += "\"realPower\":";
    cmd += realPower;
    
    cmd += ",\"apparentPower\":";
    cmd += apparentPower;

    cmd += ",\"powerFactor\":";
    cmd += powerFactor;

    cmd += ",\"Vrms\":";
    cmd += Vrms;

    cmd += ",\"Irms\":";
    cmd += Irms;

    cmd += "}";
    cmd += " HTTP/1.0\r\n";
    cmd += "Host: emoncms.org\r\n";
    cmd += "User-Agent: Arduino-ethernet\r\n";
    cmd += "Connection: close\r\n";
    cmd += "\r\n";

    return cmd;
}

boolean send(String dst_ip, String request) {
    String cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += dst_ip;
    cmd += "\",55667";
    esp8266_serial.println(cmd);

    if(esp8266_serial.find("Error"))
         return false;

    esp8266_serial.print("AT+CIPSEND=");
    esp8266_serial.println(request.length());
    delay(500);
    Serial.println(request);
    esp8266_serial.print(request);
}

void setup() {  
    lcd.begin(20,4);   // initialize the lcd for 16 chars 2 lines, turn on backlight

    Serial.begin(9600);
    esp8266_serial.begin(115200);
    //esp8266_serial.setTimeout(1000);

    emon1.voltage(2, 219, 1.7);  // Voltage: input pin, calibration, phase_shift
    emon1.current(1, 16.15);       // Current: input pin, calibration.
}


void writeToLCD(String msg, int line) {
    lcd.setCursor(0, line); //Start at character 4 on line 0
    lcd.print(msg);
}


void serialSpy() {
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
        Serial.println("sending: " + command);

        esp8266_serial.println(command); // send the read character to the esp8266_serial  
    }  
}

int counts = 0;

void loop() {
    //serialSpy();
    emon1.calcVI(100,2000);         // Calculate all. No.of half wavelengths (crossings), time-out

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

    send("108.161.129.205",genStatsRequest(realPower, apparentPower, powerFactor, Vrms, Irms));
}

