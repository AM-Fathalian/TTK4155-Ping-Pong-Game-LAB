#pragma once
#include <stdint.h>
#include "sam.h"

void encoder_init();
int readEncoderPosition();
void resetEncoderPosition();
float calculateSpeed(int positionDifference, float timeIntervalSec);
void setup_punch_pin();
void set_punch_pin();
void clear_punch_pin();
void setupMOTOR();
void set_duty_MOTOR(int x);