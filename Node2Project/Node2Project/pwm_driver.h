#pragma once
#include <stdint.h>
#include "pwm_driver.h"

//void peripherial_init();
void setupPWM();
void set_duty(int x);