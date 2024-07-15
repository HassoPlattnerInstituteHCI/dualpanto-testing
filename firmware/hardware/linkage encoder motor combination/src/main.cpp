#include "Arduino.h"
#include <Encoder.h>
#include <SPI.h>
#include <vector>
#include <deque>

// encoder
static const uint32_t c_hspiSsPin1 = 15;
static const uint32_t c_hspiSsPin2 = 5;
static const uint16_t c_nop = 0x0;
static const uint16_t c_clearError = 0x4001;
static const uint16_t c_readAngle = 0xFFFF; //0xFFFF
static const uint16_t c_dataMask = 0x3FFF;

static const uint16_t endeffectorEncoderPin[2] = {35, 36};
static const uint16_t endeffectorEncoderPin2[2] = {34, 39};
Encoder* endeffectorEncoder[2];

SPIClass m_spi(HSPI);
SPISettings m_settings(10000000, SPI_MSBFIRST, SPI_MODE1);

uint16_t buf = 0;

// motors

static const uint16_t c_ledcFrequency = 20000;
static const uint16_t c_ledcResolution = 12;
static const uint16_t PWM_MAX = 4095;

// PWM pins for the main linkage actuators
//upper left, upper right, lower left, lower right
static const uint16_t pwmPinForwards[4] = {16, 17, 22, 18};
static const uint16_t pwmPinBackwards[4] = {4, 2, 21, 19};

//Digital signal pins for endeffector motors
static const uint16_t endeffectorDirectionPin[2] = {32, 26};
//PWM pins for the endeffector motors
static const uint16_t endeffectorPWMPin[2] = {25, 23};

// #####

//int incomingByte = 0;    // for incoming serial data
int32_t encoders[6] = {0,0,0,0,0,0};
int32_t encoder_zero[6];

void setup_encoders(){
  pinMode(13, OUTPUT);
  pinMode(c_hspiSsPin1, OUTPUT);
  pinMode(c_hspiSsPin2, OUTPUT);

  for (int i = 0; i < 2; i++){
      pinMode(endeffectorEncoderPin[i], INPUT);
      pinMode(endeffectorEncoderPin2[i], INPUT);
      endeffectorEncoder[i] = new Encoder(
          endeffectorEncoderPin[i], endeffectorEncoderPin2[i]);
  }

  //m_values.resize(4, 0);

  m_spi.begin();
}

int32_t overflow_correction[6] = {0,0,0,0,0,0};
uint16_t new_encoder_pos[6];
uint16_t last_encoder_pos[6] = {0,0,0,0,0,0};
void loop_encoders(){
  for (int channel = 0; channel < 2; channel++){
    m_spi.beginTransaction(m_settings);
    digitalWrite(13, HIGH);
    digitalWrite(c_hspiSsPin1, HIGH);
    digitalWrite(c_hspiSsPin2, HIGH);
    if (channel == 0) {digitalWrite(c_hspiSsPin2, LOW);}
    else if(channel == 1) {digitalWrite(c_hspiSsPin1, LOW);}
    for(auto i = 0; i < 2; ++i)
    {
        buf = m_spi.transfer16(c_readAngle);
    }
    digitalWrite(c_hspiSsPin1, HIGH);
    digitalWrite(c_hspiSsPin2, HIGH);

    delayMicroseconds(1);

    if (channel == 0){
        //Serial.printf("\ndptest");
    }

    if(channel == 0) digitalWrite(c_hspiSsPin2, LOW);
    else if(channel == 1) digitalWrite(c_hspiSsPin1, LOW);
    for(auto i = 0; i < 2; ++i){
        buf = m_spi.transfer16(c_nop);
        if (channel == 0){
          new_encoder_pos[(!i) + channel*2] = buf & c_dataMask;
          //Serial.printf("%d,", encoders[(!i) + channel*2]);
        } else {
          new_encoder_pos[i + channel*2] = buf & c_dataMask;
          //Serial.printf("%d,", encoders[i + channel*2]);
        }
    }
    digitalWrite(c_hspiSsPin1, HIGH);
    digitalWrite(c_hspiSsPin2, HIGH);

    m_spi.endTransaction();

    if (channel == 1){
        for (int i = 0; i < 2; i++){
            //Serial.printf("%d,", endeffectorEncoder[i]->read());
            new_encoder_pos[4 + i] = endeffectorEncoder[i]->read();
        }
    }
  }

  for (int i = 0; i < 6; i++){
    if (i == 1 || i == 2) { new_encoder_pos[i] = (uint16_t)(abs(16383 - (int32_t)new_encoder_pos[i]));}

    if (i < 4){
    if (abs(last_encoder_pos[i] - new_encoder_pos[i]) > 15000){
      if (new_encoder_pos[i] < last_encoder_pos[i]){
        overflow_correction[i] += 16383;
      }
      if (new_encoder_pos[i] > last_encoder_pos[i]){
        overflow_correction[i] -= 16383;
      }
    }
    encoders[i] = (int32_t)(new_encoder_pos[i]) + overflow_correction[i] - encoder_zero[i];
    if (encoder_zero[i] != 0) {
      if (encoders[i] > 6000){overflow_correction[i] -= 16383;}
      if (encoders[i] < -5000){overflow_correction[i] += 16383;}
    }
    encoders[i] = (int32_t)(new_encoder_pos[i]) + overflow_correction[i] - encoder_zero[i];
    
    } else {
      encoders[i] = abs(new_encoder_pos[i] % (136 * 2));
    }
    last_encoder_pos[i] = new_encoder_pos[i];
  }
}

void send_encoders(){
  // for (int i = 0; i < 6; i++){
  //   Serial.printf("%d,", encoders[i] - encoder_zero[i]);
  // }
  // Serial.print(" -- ");
  for (int i = 0; i < 6; i++){
    Serial.printf("%d,", encoders[i]);
  }
  Serial.println("");
}

void setup_motors(){
   for (int i = 0; i < 4; i++){
    pinMode(pwmPinForwards[i], OUTPUT);
    ledcSetup(i, c_ledcFrequency, c_ledcResolution);
    ledcAttachPin(pwmPinForwards[i], i);
  }

  for (int i = 0; i < 4; i++){
    pinMode(pwmPinBackwards[i], OUTPUT);
    ledcSetup(i + 4, c_ledcFrequency, c_ledcResolution);
    ledcAttachPin(pwmPinBackwards[i], i + 4);
  }

  for (int i = 0; i < 2; i++){
    pinMode(endeffectorDirectionPin[i], OUTPUT);
    pinMode(endeffectorPWMPin[i], OUTPUT);
    ledcSetup(i + 8, c_ledcFrequency, c_ledcResolution);
    ledcAttachPin(endeffectorPWMPin[i], i + 8);
  }
}

void reset_motors(){
  for (int i = 0; i < 8; i++){
    ledcWrite(i, 0.175*PWM_MAX);
    delay(10);
    ledcWrite(i, 0);
  }

  for (int i = 0; i < 2; i++){
    digitalWrite(endeffectorDirectionPin[i], 1);
    ledcWrite(i + 8, 0.175*PWM_MAX);
    delay(10);
    digitalWrite(endeffectorDirectionPin[i], 0);
    ledcWrite(i+8, 0);
  }
}

void move_to_end(uint16_t pid){
  loop_encoders();
  int last_pos = encoders[pid % 4] - 4;
  while ((abs(last_pos - encoders[pid % 4]) > 3)){ 
    int new_speed = (100 + abs(last_pos - encoders[pid % 4])) / 100;
    ledcWrite(pid, (1 / new_speed) * 0.3*PWM_MAX);
    delay(10);
    ledcWrite(pid, 0);
    //delay(10);
    last_pos = encoders[pid % 4];
    loop_encoders();
  }
}

std::deque<uint16_t> last_positions1;
std::deque<uint16_t> last_positions2;
void both_move_to_end(uint16_t pid1, uint16_t pid2){
  loop_encoders();
  if (last_positions1.size() >= 3){ last_positions1.pop_front(); }
  last_positions1.push_back(encoders[pid1 % 4] - 4);

  if (last_positions2.size() >= 3){ last_positions2.pop_front(); }
  last_positions2.push_back(encoders[pid2 % 4] - 4);

    int last_pos1 = encoders[pid1 % 4] - 4;
    int last_pos2 = encoders[pid1 % 4] - 4;

    uint16_t max1 = *std::max_element(last_positions1.begin(), last_positions1.end());
    uint16_t min1 = *std::min_element(last_positions1.begin(), last_positions1.end());
    uint16_t max2 = *std::max_element(last_positions2.begin(), last_positions2.end());
    uint16_t min2 = *std::min_element(last_positions2.begin(), last_positions2.end());

  while (abs(max1 - min1) > 10 || abs(max2 - min2) > 10){
    int new_speed1 = (100 + abs(last_pos1 - encoders[pid1 % 4])) / 100;
    int new_speed2 = (100 + abs(last_pos2 - encoders[pid2 % 4])) / 100;
    ledcWrite(pid1, (1 / new_speed1) * 0.2*PWM_MAX);
    ledcWrite(pid2, (1 / new_speed2) * 0.2*PWM_MAX);
    delay(10);
    ledcWrite(pid1, 0);
    ledcWrite(pid2, 0);
    //delay(10);

    if (last_positions1.size() >= 5){ last_positions1.pop_front(); }
    last_positions1.push_back(encoders[pid1 % 4] - 4);

    if (last_positions2.size() >= 5){ last_positions2.pop_front(); }
    last_positions2.push_back(encoders[pid2 % 4] - 4);

    last_pos1 = encoders[pid1 % 4];
    last_pos2 = encoders[pid2 % 4];

    max1 = *std::max_element(last_positions1.begin(), last_positions1.end());
    min1 = *std::min_element(last_positions1.begin(), last_positions1.end());
    max2 = *std::max_element(last_positions2.begin(), last_positions2.end());
    min2 = *std::min_element(last_positions2.begin(), last_positions2.end());
    loop_encoders();
  }
}

void move_linkage(uint16_t dir1, uint16_t dir2){

    loop_encoders();
    send_encoders();

    // move out
    ledcWrite(dir1 % 4, 0.2*PWM_MAX);
    ledcWrite(dir2 % 4, 0.2*PWM_MAX);
    delay(100);
    ledcWrite(dir1 % 4, 0);
    ledcWrite(dir2 % 4, 0);
    delay(200);


    // move to end
    move_to_end(dir1);
    ledcWrite(dir1, 0.4*PWM_MAX);
    move_to_end(dir2);
    ledcWrite(dir1, 0);

    loop_encoders();
    send_encoders();

//    ledcWrite(dir2, 0.4*PWM_MAX);
//    ledcWrite(dir1, 0.1*PWM_MAX);
//    delay(100);
//    ledcWrite(dir1, 0.4*PWM_MAX);
//    ledcWrite(dir2, 0.3*PWM_MAX);
//    delay(500);
//    ledcWrite(dir1, 0);
//    ledcWrite(dir2, 0);

    both_move_to_end(dir1 % 4, dir2 % 4);
    both_move_to_end(dir1, dir2);

    // back to start
    loop_encoders();
    send_encoders();

}

void align_motors(){
  reset_motors();

  move_linkage(5, 4);

  //lower handle

  move_linkage(6, 7);
}

void setup(){
  Serial.begin(9600);    // opens serial port, sets data rate to 9600 bps
  setup_encoders();
  loop_encoders();
  setup_motors();
  reset_motors();
  delay(5000);
  align_motors();
}

void loop(){
  delay(1);
}



//std::deque<uint16_t> last_positions1[4];
//void both_move_to_end(uint16_t pid1, uint16_t pid2){
//  loop_encoders();
//  if (last_positions1.size() >= 5){ last_positions1.pop_front(); }
//  last_positions1.push_back(encoders[pid1 % 4] - 4;);
//
//
//  int last_pos1 = encoders[pid1 % 4] - 4;
//  int last_pos2 = encoders[pid1 % 4] - 4;
//
//  while ((abs(last_pos1 - encoders[pid1 % 4]) > 3) || (abs(last_pos2 - encoders[pid2 % 4]) > 3)){
//    int new_speed1 = (100 + abs(last_pos1 - encoders[pid1 % 4])) / 100;
//    int new_speed2 = (100 + abs(last_pos2 - encoders[pid2 % 4])) / 100;
//    ledcWrite(pid1, (1 / new_speed1) * 0.2*PWM_MAX);
//    ledcWrite(pid2, (1 / new_speed2) * 0.2*PWM_MAX);
//    delay(10);
//    ledcWrite(pid1, 0);
//    ledcWrite(pid2, 0);
//    //delay(10);
//    last_pos1 = encoders[pid1 % 4];
//    last_pos2 = encoders[pid2 % 4];
//    loop_encoders();
//  }
//}