#include <SoftwareSerial.h>
#include <SimpleTimer.h>

SoftwareSerial btSerial(2,3); // Rx, Tx for arduino, Tx, Rx for BT


SimpleTimer timer;

int echoPin1 = 8; //D5, 초음파 센서의 핀번호 설정
int trigPin1 = 9; //D6, 초음파 센서의 핀번호 설정

int echoPin2 = 10; //D7, 초음파 센서의 핀번호 설정
int trigPin2 = 11; //D8, 초음파 센서의 핀번호 설정

int buzzer = 4;    //D3, 부저
int LEDR = 5;      //D1
int LEDG = 6;       //D2

int num = 0;      //사람수 카운트
unsigned long rec_sen1 = 0; //센서 활성화 시간 기록
unsigned long rec_sen2 = 0;

int jud1 = 0; // 순서 판단 용
int jud2 = 0; 

int limitnum = 2; //방안에는 이 이상 넘을 수 없다. 

int who = 15; // Threshold

int counts = 0;

//######## reset 함수 ###########
void Reset(){
jud1 = 0;
jud2 = 0;
rec_sen1 =0;
rec_sen2 =0;
digitalWrite(LEDG, LOW);
Serial.println("Reset!");
}

void Send(){
  btSerial.write(num);
}

//###############################################
//################ setup ########################
void setup() {
  // put your setup code here, to run once:
pinMode(trigPin1, OUTPUT); // trig를 출력모드로 설정
pinMode(echoPin1, INPUT); //echo를 입력모드로 설정
pinMode(trigPin2, OUTPUT); // trig를 출력모드로 설정
pinMode(echoPin2, INPUT); //echo를 입력모드로 설정

pinMode(LEDR, OUTPUT);
pinMode(LEDG, OUTPUT);


Serial.begin(9600);
btSerial.begin(9600);

//timer.setInterval(1000L, Send);


}
//###################################
//########### loop 문 ###############
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentmillis = millis();
  timer.run();

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


  //########### 센서 활성화 ##########  
  if((int)distance1 < who){
     rec_sen1 = currentmillis;
     Serial.println("sen1 activated!");
     jud1 = 1; // sen1 이 마지막
     counts=0;
  }
  
  //Serial.println(currentmillis);
  //Serial.println(rec_sen1);

  // millis를 통해 일정 시간이 지나면 꺼짐. 
  if(currentmillis - rec_sen1 >= 3000 && currentmillis - rec_sen1 < 3600){
    Reset();
  }
 
  if((int)distance2 < who){  
      rec_sen2 = currentmillis;
      Serial.println("sen2 activated!");
      jud2=1;  //sen2 이 마지막 
      counts=0; 
  }
  if(currentmillis - rec_sen2 >= 3000 && currentmillis - rec_sen2 < 3600){
    Reset();
  }

  //unsigned long gap = rec_sen2 - rec_sen1;      //gap>0 이면, sen1이 먼저 활성화 되었다.
  //temp1>temp2 이면 sensor1이 먼저 활성화 되었음.
  
Serial.print("jud1 : ");
Serial.print(jud1);
Serial.print("     jud2 : ");
Serial.println(jud2);

//Serial.println(rec_sen1);
//Serial.println(rec_sen2);

long gap = rec_sen2-rec_sen1; 
  
  //################ 사람 수 판단 ##################
  if((jud1 == 1 && jud2 == 1) && (gap)< -300){
    num--;   
    Reset();                        // 지나가는 도중 중복 카운트를 피하기 위해
    digitalWrite(LEDG, HIGH);
    tone(buzzer, 800, 500);
    timer.setTimeout(300, Reset);  // 사람이 지나 간 후 jud 변수 완벽한 초기화를 위해
    Serial.println("the main comming out");
  }

  if((jud1 == 1 && jud2 == 1) && (gap)> 300){
    num++;
    Reset();
    digitalWrite(LEDG, HIGH);
    tone(buzzer, 800, 500);
    timer.setTimeout(300, Reset);
    Serial.println("the main comming in");
  }
  
  Serial.print("num : ");
  Serial.println(num);


  //############## 판단조건 ###############
  if(num>limitnum){
      
      Serial.println("Something is Happen!");
      digitalWrite(LEDR, HIGH);
      tone(buzzer, 800, 3000);
      delay(10000);
      digitalWrite(LEDR, LOW);
            
  }
  

  if(btSerial.available()&& (counts != 1)){
    counts = 1;
    limitnum=btSerial.read();
    btSerial.write(num);
    Serial.println("limit number is changed");
    Serial.println(limitnum);
  }

  delay(100);
}
