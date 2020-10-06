#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

char auth[] = "sQiF_9zJhEA1m209F2fkonJeiSVM_Yn3"; // nodeMCU3>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "AndroidAPchy";
char pass[] = "chan0812";

//SimpleTimer timer;

int echoPin1 = 14; //D5, 초음파 센서의 핀번호 설정
int trigPin1 = 12; //D6, 초음파 센서의 핀번호 설정

int echoPin2 = 13; //D7, 초음파 센서의 핀번호 설정
int trigPin2 = 15; //D8, 초음파 센서의 핀번호 설정

int num = 0;      //사람수 카운트
int temp1 = 0;    //1번 센서 활성화 카운트
int temp2 = 0;    //2번 센서 활성화 카운트
unsigned long rec_sen1 = 0;
unsigned long rec_sen2 = 0;

int jud = 3; // 순서 판단 용

int limitnum = 2; //방안에는 이 이상 넘을 수 없다. 

int who = 15; // Threshold


void setup() {
  // put your setup code here, to run once:
pinMode(trigPin1, OUTPUT); // trig를 출력모드로 설정
pinMode(echoPin1, INPUT); //echo를 입력모드로 설정
pinMode(trigPin2, OUTPUT); // trig를 출력모드로 설정
pinMode(echoPin2, INPUT); //echo를 입력모드로 설정



Serial.begin(9600);
Blynk.begin(auth, ssid, pass);

//timer.setInterval(10000, Reset);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentmillis = millis();
  
  Blynk.run();
  float duration1, distance1; //초음파센서의 값
  float duration2, distance2; //초음파센서의 값

   //문과 사람의 거리


  
  //########## 초음파 센서 1 ################
  digitalWrite(trigPin1, HIGH);// 초음파를 보낸다. 다 보내면 echo가 HIGH 상태로 대기하게 된다.
  delay(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH); // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance1 = ((float)(340 * duration1) / 10000) / 2;  // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  //Serial.print((int)distance1);
  //Serial.println("cm");// 수정한 값을 출력

  //########### 초음파 센서 2 ###############
  digitalWrite(trigPin2, HIGH);// 초음파를 보낸다. 다 보내면 echo가 HIGH 상태로 대기하게 된다.
  delay(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH); // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance2 = ((float)(340 * duration2) / 10000) / 2;  // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  //Serial.print((int)distance2);
  //Serial.println("cm");// 수정한 값을 출력

  //## 센서 활성화
  if((int)distance1 < who){
     temp1++;
     rec_sen1 = currentmillis;
     Serial.println("sen1 activated!");
     jud = 1; // sen1 이 마지막
     //timer.run();
  }
  if((int)distance2 < who){
    
      temp2++;
      rec_sen2 = currentmillis;
      Serial.println("sen2 activated!");
      jud=2;  //sen2 이 마지막
      //timer.run();
    
  }

  //unsigned long gap = rec_sen2 - rec_sen1;      //gap>0 이면, sen1이 먼저 활성화 되었다.
  //temp1>temp2 이면 sensor1이 먼저 활성화 되었음.
Serial.print("jud : ");
Serial.println(jud);
//Serial.println(rec_sen1);
//Serial.println(rec_sen2);
long gap = rec_sen2-rec_sen1;
//Serial.println(gap);
Serial.print("temp : ");
Serial.println(temp1);
Serial.print("temp2 : ");
Serial.println(temp2);

  //if(gap>0&& (rec_sen1 > 0 && rec_sen2 > 0)){
  if(jud == 1 && temp2 > 1){
    num--;
    jud=0;
    Serial.println("the main comming out");
    temp1 = 0 ;
     temp2 = 0 ;
    
  }
  //if(gap>0&& (rec_sen1 > 0 && rec_sen2 > 0)){
  if(jud == 2 && temp1 > 3){
    num++;
    jud=0;
    Serial.println("the main comming in");
    temp1 = 0 ;
    temp2 = 0 ;
    
  }
  //if(((int)distance1>=who) && ((int)distance2 >= who)){
  if((jud==2 && temp1>3) || (jud == 1 && temp2 > 0)){
    temp1=0;
    temp2=0;
  }
  Serial.print("num : ");
  Serial.println(num);


  

  if(num>=limitnum){
   
      Blynk.notify("Test!!");
      Serial.println("Something is Happen!");
      delay(10000);
      num=0;
      
  
  }

  delay(400);
}



void Reset(){
rec_sen1 =0;
rec_sen2 =0;
temp1=0;
temp2=0;
}
