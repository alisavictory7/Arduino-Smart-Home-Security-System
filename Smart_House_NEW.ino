#include <gprs.h>
#include <SoftwareSerial.h>
#define TIMEOUT 5000
GPRS gprs;

const int buttonPin = 2;
const int ledPin =  13;
const int pinSpeaker = 10;
const int sensor = 4;
const int ldrPin = A0;
const int ldrPin2 = A1;
const int pp = 6;

int PIRstate = 0;       // infrared sensor     
int reedSwitchState;    // reed switch / magnetic door lock
int ppState;            // pressure pad
int ldrState;           // 1st LDR
int ldr2State;          // 2nd LDR
int flag;               // going to be changed later accordingly

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pinSpeaker, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(sensor, INPUT_PULLUP); 
  pinMode(ldrPin, INPUT);
  pinMode(ldrPin2, INPUT);
  pinMode(pp, INPUT);
  digitalWrite(pp, HIGH);
  flag = 0;     // house is safe

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting SIM800 Auto Read SMS");
  
  gprs.preInit();
  delay(1000);
  while (0 != gprs.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }
  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  
  Serial.println("Init success");
}
void loop() {
  PIRstate = digitalRead(buttonPin);
  reedSwitchState = digitalRead(sensor);
  ppState = digitalRead(pp);

  ldrState = analogRead(ldrPin);
  ldr2State = analogRead(ldrPin2);

  if(ppState == LOW) {
    flag = 1;
    digitalWrite(ledPin, HIGH);
    digitalWrite(pinSpeaker, HIGH);

    Serial.println("SMS HOME SECURITY, WARNING!!!");
    Serial.println("Pressure Pad: Detected applied pressure");

    gprs.sendSMS("089637070357","Pressure Pad: Detected applied pressure");
    delay (200);
  }
  if(PIRstate == HIGH) {
    flag = 2;
    digitalWrite(ledPin, HIGH);
    digitalWrite(pinSpeaker, HIGH);

    Serial.println("SMS HOME SECURITY, WARNING!!!");
    Serial.println("Infrared Sensor: Movement Detected in Living Room!");
    
    gprs.sendSMS("089637070357","Infrared Sensor: Movement Detected In Living Room!");
    delay (200);
  }
  if(reedSwitchState == HIGH) {
    flag = 3;
    digitalWrite(ledPin, HIGH);
    digitalWrite(pinSpeaker, HIGH);

    Serial.println("SMS HOME SECURITY, WARNING!!!");
    Serial.println("Reed switch: Front door is unlocked!");
    
    gprs.sendSMS("089637070357","Reed Switch: Front door is unlocked!");
    delay (200);
  }
  if(ldrState < 800) {
    flag = 4;
    digitalWrite(ledPin, HIGH);
    digitalWrite(pinSpeaker,HIGH);

    Serial.println("SMS HOME SECURITY, WARNING!!!");
    Serial.println("LDR: Laser 1 Beam Blockage!");
    
    gprs.sendSMS("089637070357","LDR: Laser 1 Beam Blockage!");
    delay (200);
  }
  if(ldr2State < 800) {
    flag = 5;
    digitalWrite(ledPin, HIGH);
    digitalWrite(pinSpeaker,HIGH);

    Serial.println("SMS HOME SECURITY, WARNING!!!");
    Serial.println("LDR: Laser 2 Beam Blockage!");
    
    gprs.sendSMS("089637070357","LDR: Laser 2 Beam Blockage!");
    delay (200);
  }
  else {
    digitalWrite(ledPin, LOW);
    digitalWrite(pinSpeaker, LOW);
    if(flag == 0) {
      Serial.println("House is safe :)");
      delay (200);
    }
    else {
      if(flag==1) {
        Serial.println("Pressure Pad: Detected applied pressure");
      }
      else if(flag==2) {
        Serial.println("Infrared Sensor: Movement Detected in Living Room!");
      }
      else if(flag==3) {
        Serial.println("Reed switch: Front door is unlocked!");
      }
      else if(flag==4) {
        Serial.println("LDR: Laser 1 Beam Blockage!");
      }
      else {
        Serial.println("LDR: Laser 2 Beam Blockage!");
      }
    }
  }
  Serial.println();
}

