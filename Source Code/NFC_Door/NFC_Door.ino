/*
    Follow me:
    github.com/BRUNOO1545
*/

//libs
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>

//D10:pin of tag reader SDA. D9:pin of tag reader RST
RFID rfid(10, 9);

unsigned char status;
unsigned char str[MAX_LEN];

String accessGranted [1] = {"210363986"}; // String accessGranted [amount of NFC devices] = {"device id", "other device", ...};
int accessGrantedSize = 1; //Max amount of NFC devices

//servo data
Servo lockServo;
int lockPos = 20;
int unlockPos = 100;
boolean locked = true;

//Debug
boolean debug_mode = false;
String debug_text = "[DEBUG] ";

//lines
String LineSpace = "------------------------------------------------------------------------------------------------------";
String LineSpace2 = "-----------------------------------------";

int redLEDPin = 5;
int greenLEDPin = 6;

void setup()
  {
    Serial.begin(9600);
    SPI.begin();
    
    //debug
    if (debug_mode == true)
      {
          Serial.println(LineSpace);
          Serial.println("ALERT: Debug Mode Activated");
          Serial.println(LineSpace);
          Serial.println("");
          Serial.println("");
      }
    
    //init
    Serial.println(LineSpace);
    Serial.println("Starting RFID...");
    rfid.init();
    
    //Init LED sequence
    Serial.println(LineSpace);
    Serial.println("Starting LED sequence...");
    
    pinMode(redLEDPin, OUTPUT);
    pinMode(greenLEDPin, OUTPUT);
    digitalWrite(redLEDPin, HIGH);
    delay(200);
    digitalWrite(greenLEDPin, HIGH);
    delay(200);
    digitalWrite(redLEDPin, LOW);
    delay(200);
    digitalWrite(greenLEDPin, LOW);

    //servo
    Serial.println(LineSpace);
    Serial.println("Starting door locking.\nPlease wait...");
    lockServo.attach(3);
    lockServo.write(lockPos);
    
    Serial.println(LineSpace);
    Serial.println("");
  }


void loop()
  {
    if (rfid.findCard(PICC_REQIDL, str) == MI_OK)
      {
        Serial.println(LineSpace2 + " NFC founded " + LineSpace2);
        
        String temp = "";
        
        if (rfid.anticoll(str) == MI_OK)
          {
            for (int i = 0; i < 4; i++)
              {
                temp = temp + (0x0F & (str[i] >> 4)); 
                temp = temp + (0x0F & str[i]); 
              }
            
            //debug mode
            if (debug_mode == true)
              {
                Serial.println(debug_text + "ID: " + temp);
              }
            
            checkAccess(temp);
          }
        
        rfid.selectTag(str);
      }
    
    rfid.halt();
  }


void checkAccess (String temp)
  {
    boolean granted = false;

    //NFC found
    for (int i=0; i <= (accessGrantedSize-1); i++)
      {
        if(accessGranted[i] == temp)
          {
            Serial.println("Access: Granted");
            granted = true;
            
            if (locked == true)
                {
                    Serial.println("Door state: Unlocked");

                    //debug mode
                    if (debug_mode == true)
                      {
                        Serial.println(debug_text + "Starting servo.");
                        Serial.println(debug_text + "Starting LED sequence.");
                      }
                    
                    lockServo.write(unlockPos);
                    locked = false;
                }
             else if (locked == false)
                {
                    Serial.println("Door state: Locked");

                    //debug mode
                    if (debug_mode == true)
                      {
                        Serial.println(debug_text + "Starting LED sequence.");
                      }
                    
                    lockServo.write(lockPos);
                    locked = true;
                }
            
            //Green LED sequence
            digitalWrite(greenLEDPin, HIGH);
            delay(200);
            digitalWrite(greenLEDPin, LOW);
            delay(200);
            digitalWrite(greenLEDPin, HIGH);
            delay(200);
            digitalWrite(greenLEDPin, LOW);
            delay(200);

            //lines
            Serial.println(LineSpace);
            Serial.println("");
            Serial.println("");
          }
      }

    //NFC not found
    if (granted == false)
      {
        Serial.println("Access: Denied");

        //debug mode
        if (debug_mode == true)
          {
            Serial.println(debug_text + "Starting LED sequence.");
          }
        
        //Red LED sequence
        digitalWrite(redLEDPin, HIGH);
        delay(200);
        digitalWrite(redLEDPin, LOW);
        delay(200);
        digitalWrite(redLEDPin, HIGH);
        delay(200);
        digitalWrite(redLEDPin, LOW);
        delay(200);
      }
  }
