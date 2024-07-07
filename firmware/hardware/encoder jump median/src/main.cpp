#include <Encoder.h>
#include "Arduino.h"
#include <SPI.h>
#include <vector>
#include <deque>

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

static const uint32_t c_hspiSsPin1 = 15;
static const uint32_t c_hspiSsPin2 = 5;
static const uint16_t c_nop = 0x0;
static const uint16_t c_clearError = 0x4001;
static const uint16_t c_readAngle = 0xFFFF; //0xFFFF
static const uint16_t c_dataMask = 0x3FFF;

static const uint16_t endeffectorEncoderPin[2] = {35, 36};
static const uint16_t endeffectorEncoderPin2[2] = {34, 39};
Encoder* endeffectorEncoder[2];

std::vector<uint16_t> m_values;

int incomingByte = 0;    // for incoming serial data
SPIClass m_spi(HSPI);
SPISettings m_settings(10000000, SPI_MSBFIRST, SPI_MODE1);

int channel = 0;
int c_i = 0;
uint16_t buf = 0;

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

    m_values.resize(4, 0);

    m_spi.begin();
}


std::deque<uint16_t> previous_values[4];
uint16_t encoders[6] = {0,0,0,0,0,0};
uint16_t median[4] = {0,0,0,0};

void loop_encoders(){
    m_spi.beginTransaction(m_settings);
    digitalWrite(13, HIGH);
    digitalWrite(c_hspiSsPin1, HIGH);
    digitalWrite(c_hspiSsPin2, HIGH);

    delayMicroseconds(1);

    for (int channel = 0; channel < 2; channel++){

        if (channel == 0) {digitalWrite(c_hspiSsPin2, LOW);}
        else if(channel == 1) {digitalWrite(c_hspiSsPin1, LOW);}
        for(auto i = 0; i < m_values.size()/2; ++i)
        {
            buf = m_spi.transfer16(c_readAngle);
            //Serial.printf("%d", buf);
            //Serial.println();
        }
        digitalWrite(c_hspiSsPin1, HIGH);
        digitalWrite(c_hspiSsPin2, HIGH);

        delayMicroseconds(1);

//        if (channel == 0){
//            Serial.printf("\r\n");
//        }

        if(channel == 0) digitalWrite(c_hspiSsPin2, LOW);
        else if(channel == 1) digitalWrite(c_hspiSsPin1, LOW);
        for(auto i = 0; i < m_values.size()/2; ++i)
        {
            buf = m_spi.transfer16(c_nop);
            if (channel == 0){
              encoders[(!i) + channel*2] = buf & c_dataMask;
              //Serial.printf("%d,", encoders[(!i) + channel*2]);
            } else {
              encoders[i + channel*2] = buf & c_dataMask;
              //Serial.printf("%d,", encoders[i + channel*2]);
            }

        }
        digitalWrite(c_hspiSsPin1, HIGH);
        digitalWrite(c_hspiSsPin2, HIGH);

        m_spi.endTransaction();

//        if (channel == 1){
//            for (int i = 0; i < 2; i++){
//                Serial.printf("%d,", endeffectorEncoder[i]->read());
//            }
//        }
    }

    for (int i = 0; i < 4; i++){
        Serial.printf("%d,", encoders[i]);
    }

    uint16_t sorting_array[5] = {0,0,0,0,0};
    for (int i = 0; i < 4; i++){
        if (previous_values[i].size() >= 5){
            previous_values[i].pop_front();
        }
        previous_values[i].push_back(encoders[i]);
        if (previous_values[i].size() == 5){
            for (int j = 0; j < 5; j++){
                sorting_array[j] = previous_values[i][j];
            }
            std::sort(sorting_array, sorting_array + 5);
            median[i] = sorting_array[2];
            Serial.printf("%d,", median[i]);
        }
    }

    Serial.printf("\r\n");


//    c_i++;
//    channel = c_i % 2;
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


void setup()
{
    Serial.begin(9600);    // opens serial port, sets data rate to 9600 bps
    setup_encoders();
    setup_motors();
}

void loop(){
  loop_encoders();
  for (int i = 0; i < 4; i++){
    ledcWrite(i+4, 0);
    ledcWrite(i, 0.15*PWM_MAX);

  }
  delay(3);
  loop_encoders();
  for (int i = 0; i < 4; i++){
    ledcWrite(i, 0);
    ledcWrite(i+4, 0.15*PWM_MAX);
  }
  //loop_encoders();
  delay(2);
  //loop_encoders();
}