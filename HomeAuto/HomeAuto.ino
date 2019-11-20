#include<SoftwareSerial.h>
SoftwareSerial mySerial(3,2);     //(Rx,Tx)
String BT;
int intBT;
#define kBulb 8
#define fan 9
#define roomBulb 10
#define oBulb 11
#define Temp  A0
#define PIR  A1
#define LDR  A2

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(kBulb,OUTPUT);
  pinMode(fan,OUTPUT);
  pinMode(roomBulb,OUTPUT);
  pinMode(oBulb,OUTPUT);
  pinMode(Temp,INPUT);
  pinMode(PIR,INPUT);
  pinMode(LDR,INPUT);
  mySerial.print("Connected");
}
int fstate=0;
int bulbstate=0;
int tsense=0;
int kstate = 0;
int ostate = 0;
//             FAN
//             FAN
void FAN(String received){                                    //received values between 97-105
  if(received.equalsIgnoreCase("fON")){
    analogWrite(fan,100);
    fstate = 1;
  }if(received.equalsIgnoreCase("fOFF")){
    analogWrite(fan,0);
    fstate = 0;
  }
  //BT="";
}
//             KBULB
//             KBULB
void KBULB(){                                   //received values between 106-107
  int pirValue = digitalRead(PIR);  // read input value
  if (pirValue == 1) {
    digitalWrite(kBulb, HIGH);
    //Serial.println("Motion detected!");
  } 
  else if(pirValue == 0){
    digitalWrite(kBulb, LOW);
    //Serial.println("Motion ended!");
  }
}
//             OBULB
//             OBULB
void OBULB(){                                   //received values between 121-122
  int ldr = analogRead(LDR);
  float ldr8bit = ldr/4.0;
  if(ldr8bit>135){
    analogWrite(oBulb,0);
  }
  else if(ldr8bit>100 && ldr8bit<=120){
    analogWrite(oBulb,125);
  }
  else if(ldr8bit>50 && ldr8bit<=100){
   analogWrite(oBulb,180);
  }
  else if(ldr8bit<80){
    analogWrite(oBulb,255);
  }
}
//             RoomBULB
//             RoomBULB

void TempSense(){                 //Function for Temperature calc. & speed regulation
  float TempValue = analogRead(Temp);
  float cel = (TempValue/1024.0)*500.0;
  //Serial.print(cel);Serial.print(" C");
  if(cel<=12){
    analogWrite(fan,0);
  }
  else if(cel>12 && cel<=15){
    analogWrite(fan,51);
  }
  else if(cel>15 && cel<=22){
    analogWrite(fan,102);
  }
  else if(cel>22 && cel<=29){
    analogWrite(fan,153);
  }
  else if(cel>29 && cel<=40){
    analogWrite(fan,204);
  }
  else{
    analogWrite(fan,255);
  }
}
void loop() {
  if(mySerial.available()){
    while(mySerial.available()){                
        delay(3);
        char inChar = (char)mySerial.read();
        BT += inChar;
    }
    intBT = BT.toInt();
    Serial.println(BT);
  }
  if(BT.equalsIgnoreCase("fON")||BT.equalsIgnoreCase("fOFF")){
    FAN(BT);
  }
  if(BT.equalsIgnoreCase("rON")){
    bulbstate=1;
  }else if(BT.equalsIgnoreCase("rOFF")){
    bulbstate=0;
  }
  if(BT.equalsIgnoreCase("kON")){
    kstate=1;
  }else  if(BT.equalsIgnoreCase("kOFF")){
    kstate=0;
  }
  if(BT.equalsIgnoreCase("oON")){
    ostate=1;
  }else if(BT.equalsIgnoreCase("oOFF")){
    ostate=0;
  }
  if(intBT>255 && intBT<=510){
    if(fstate == 1 && tsense==0){
      analogWrite(fan,intBT-255);
    }
  }
  if(BT.equalsIgnoreCase("autoSense")){
    if(tsense==0){
      tsense=1;
    }else if(tsense==1){
      tsense=0;
    }
  }
  if(tsense==1){
    if(fstate==1){
      TempSense();
    }
  }
  switch(ostate){
      case 1: OBULB();break;
      case 0: analogWrite(oBulb,0);break;
  }
  switch(kstate){
      case 1: KBULB();break;
      case 0: digitalWrite(kBulb,LOW);break;
  }
  switch(bulbstate){
    case 1: //else{
      digitalWrite(roomBulb,HIGH);
    //}
    case 0: digitalWrite(roomBulb,LOW);
  }
  if(intBT<=255){
    if(bulbstate == 1){
      analogWrite(roomBulb,intBT);
    }
  }
  BT="";
}
