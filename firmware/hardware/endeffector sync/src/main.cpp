#include "Arduino.h"
#include <Encoder.h>
#include <SPI.h>
#include <vector>

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
    
    if (i >= 4) {
      encoders[i] = abs(new_encoder_pos[i] % (136 * 2));
    }
  }
}

void send_encoders(){
  // for (int i = 0; i < 6; i++){
  //   Serial.printf("%d,", encoders[i] - encoder_zero[i]);
  // }
  // Serial.print(" -- ");
  for (int i = 0; i < 6; i++){
    Serial.printf("[%d, %d]", encoders[i], overflow_correction[i]);
  }
  Serial.println("");
}

void setup_motors(){

  for (int i = 0; i < 2; i++){
    pinMode(endeffectorDirectionPin[i], OUTPUT);
    pinMode(endeffectorPWMPin[i], OUTPUT);
    ledcSetup(i + 8, c_ledcFrequency, c_ledcResolution);
    ledcAttachPin(endeffectorPWMPin[i], i + 8);
  }
}

void reset_motors(){
  for (int i = 0; i < 2; i++){
    digitalWrite(endeffectorDirectionPin[i], 1);
    ledcWrite(i + 8, 0.175*PWM_MAX);
    delay(10);
    digitalWrite(endeffectorDirectionPin[i], 0);
    ledcWrite(i+8, 0);
  }
}


void sync_endeffectors(){
    //reset_motors();


    loop_encoders();

    if (encoders[4] - encoders[5] > 3) {
        digitalWrite(endeffectorDirectionPin[1], 0);
        ledcWrite(8, 0.175*PWM_MAX);
        digitalWrite(endeffectorDirectionPin[0], 1);
        ledcWrite(9, 0.175*PWM_MAX);
    }
    else if (encoders[4] - encoders[5] < -3) {
        digitalWrite(endeffectorDirectionPin[1], 1);
        ledcWrite(8, 0.175*PWM_MAX);
        digitalWrite(endeffectorDirectionPin[0], 0);
        ledcWrite(9, 0.175*PWM_MAX);

    }
    else {
        ledcWrite(8, 0);
        ledcWrite(9, 0);
    }
}

void setup(){
  Serial.begin(9600);    // opens serial port, sets data rate to 9600 bps
  setup_encoders();
  loop_encoders();
  setup_motors();
  reset_motors();
  delay(5000);
  //align_motors();
  loop_encoders();
}

void loop(){  
  //delay(2);
  //loop_motors();
  for (int i = 0; i < 1000; i++){
    sync_endeffectors();
  }
  loop_encoders();
  send_encoders();
}
