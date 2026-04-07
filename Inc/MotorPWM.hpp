/*
 * MotorPWM.hpp
 *
 *  Created on: May 30, 2025
 *      Author: froilan
 */

#ifndef INC_MOTORPWM_HPP_
#define INC_MOTORPWM_HPP_

#include <cstdint>
#include <stm32f4xx_hal.h>
#include <UartSerial.hpp>

class MotorPWM {
public:
    MotorPWM(TIM_HandleTypeDef* htim_value, unsigned int channel_value, __IO uint32_t* channel_address_value);
    void setSpeed(uint32_t motor_speed_value);
    void disable();
private:
    TIM_HandleTypeDef* htim;
    unsigned int channel;
    __IO uint32_t* channel_address;
    uint32_t motor_speed;
    uint32_t channel_address_disable;
};


#endif /* INC_MOTORPWM_HPP_ */
