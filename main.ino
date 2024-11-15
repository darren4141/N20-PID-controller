const int fwdPin1 = 2;
const int bwdPin1 = 4;
const int enablePin1 = 15;
const int encA1 = 16;
const int encB1 = 17;

const int fwdPin2 = 18;
const int bwdPin2 = 19;
const int enablePin2 = 5;
const int encA2 = 22;
const int encB2 = 23;

int encoderCount[] = {0, 0};
int encoderCountPrevious[] = {0, 0};
int speedPrevious = 0;

float eI[] = {0, 0};
float ePPrevious[] = {0, 0};
float previousTime[] = {0, 0};

float previousPwr = 0;
int cycle = 0;
float prevPID1calculate = 0;
float prevPID2calculate = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(fwdPin1, OUTPUT);
  pinMode(bwdPin1, OUTPUT);

  pinMode(encA1, INPUT);
  pinMode(encB1, INPUT);

  attachInterrupt(digitalPinToInterrupt(encA1), tickEncoder1, RISING);

  pinMode(fwdPin2, OUTPUT);
  pinMode(bwdPin2, OUTPUT);

  pinMode(encA2, INPUT);
  pinMode(encB2, INPUT);

  attachInterrupt(digitalPinToInterrupt(encA2), tickEncoder2, RISING);
  
  Serial.begin(9600);

}

void loop() {
  cycle++;
  // put your main code here, to run repeatedly:
//  digitalWrite(fwdPin1, HIGH);
//  digitalWrite(bwdPin1, LOW);
//
//  digitalWrite(fwdPin2, LOW);
//  digitalWrite(bwdPin2, HIGH);
//
//  analogWrite(enablePin1, 255);
//  analogWrite(enablePin2, 255);


  // float pwr1 = calculatePositionPID(0, 1000, 0.7, 0.08, 0.1);
  // float pwr1 = calculateSpeedPID(0, 200000000, 1, 0, 0);
  float pwr1;

  if(cycle % 20 == 0){
    pwr1 = calculateSpeedPID(0, 200, 0.1, 0, 0);
    prevPID1calculate = pwr1;
  }else{
    pwr1 =  prevPID1calculate;
  }


  if(pwr1 > 0){
    analogWrite(enablePin1, abs(pwr1));
    digitalWrite(fwdPin1, HIGH);
    digitalWrite(bwdPin1, LOW);
  }else{
    analogWrite(enablePin1, abs(pwr1));
    digitalWrite(fwdPin1, LOW);
    digitalWrite(bwdPin1, HIGH);
  }

  // float pwr2 = calculatePID(1, 1000, 0.1, 0, 0);

  float pwr2;

  if(cycle % 20 == 0){
    pwr2 = calculateSpeedPID(0, 200, 0.1, 0, 0);
    prevPID2calculate = pwr2;
  }else{
    pwr2 =  prevPID2calculate;
  }

  if(pwr2 > 0){
    analogWrite(enablePin2, abs(pwr2));
    digitalWrite(fwdPin2, HIGH);
    digitalWrite(bwdPin2, LOW);
  }else{
    analogWrite(enablePin2, abs(pwr2));
    digitalWrite(fwdPin2, LOW);
    digitalWrite(bwdPin2, HIGH);
  }
  
  Serial.print(encoderCount[0]);
  Serial.print(", ");
  Serial.println(encoderCount[1]);
}

void tickEncoder1(){
  if(digitalRead(encA1) > digitalRead(encB1)){
    encoderCount[0]++;
  }else{
    encoderCount[0]--;
  }
}

void tickEncoder2(){
  if(digitalRead(encA2) > digitalRead(encB2)){
    encoderCount[1]++;
  }else{
    encoderCount[1]--;
  }
}

float calculatePositionPID(int motorNum, int target, float kP, float kI, float kD){
  long currentTime = micros();
  float dT = ((float)(currentTime - previousTime[motorNum])) / 1.0e6;

  int eP = target - encoderCount[motorNum];
  eI[motorNum] = eI[motorNum] + (eP * dT);
  float eD = (eP - ePPrevious[motorNum])/dT;

  previousTime[motorNum] = currentTime;
  ePPrevious[motorNum] = eP;

  Serial.print("eP: ");
  Serial.print(eP);
  Serial.print(", eI: ");
  Serial.print(eI[motorNum]);
  Serial.print(", eD: ");
  Serial.print(eD);
  Serial.print(" | ");


  float pwr = (kP * eP) + (kI * eI[motorNum]) + (kD * eD);

  if(abs(pwr) > 255){
    if(pwr > 0){
      pwr = 255;
    }else{
      pwr = -255;
    }
  }

  return pwr;
}

float calculateSpeedPID(int motorNum, int target, float kP, float kI, float kD){
  long currentTime = micros();
  float dT = ((float)(currentTime - previousTime[motorNum])) / 1.0e6;

  int speed = (encoderCount[motorNum] - encoderCountPrevious[motorNum])/dT;

  if(speed == 0){//speed - speedPrevious > 100
    speed = speedPrevious;
  }
  encoderCountPrevious[motorNum] = encoderCount[motorNum];

  int eP = target - speed;
  eI[motorNum] = eI[motorNum] + (eP * dT);
  float eD = (eP - ePPrevious[motorNum])/dT;

  previousTime[motorNum] = currentTime;
  ePPrevious[motorNum] = eP;
  speedPrevious = speed;
/*

SPEED RPM:

P: RPM
I: integral of RPM
D: Change in RPM

*/

  // Serial.print("eP: ");
  // Serial.print(eP);
  // Serial.print(", eI: ");
  // Serial.print(eI[motorNum]);
  // Serial.print(", eD: ");
  // Serial.print(eD);
  Serial.print(" | SPEED: ");
  Serial.print(speed);
  Serial.print(" | ");


  float pwr = (kP * eP) + (kI * eI[motorNum]) + (kD * eD);

  if(abs(pwr) > 255){
    if(pwr > 0){
      pwr = 255;
    }else{
      pwr = -255;
    }
  }

  previousPwr = previousPwr + pwr;

  return previousPwr;
}