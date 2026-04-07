/*
 * MotorPWM.cpp
 *
 *  Created on: May 30, 2025
 *      Author: froilan
 */

#include "MotorPWM.hpp"

MotorPWM::MotorPWM(TIM_HandleTypeDef* htim_value, unsigned int channel_value, __IO uint32_t* channel_address_value) : htim(htim_value), channel(channel_value), channel_address(channel_address_value) {
    HAL_TIM_PWM_Start(htim, channel);
    channel_address_disable = *channel_address_value;
}

void MotorPWM::setSpeed(uint32_t motor_speed_value)
{
    *channel_address = motor_speed_value;
}

void MotorPWM::disable(){
  //Mirar si se utiliza, no tiene sentido en PWM
  *channel_address = 1500;
}

