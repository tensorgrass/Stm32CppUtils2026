/*
 * Servo.hpp
 *
 *  Created on: Jun 9, 2025
 *      Author: froilan
 */

#ifndef INC_SERVO_HPP_
#define INC_SERVO_HPP_

#include <cstdint>
#include <stm32f4xx_hal.h>

#define SERVO_MIN_PULSE  500   // Ancho de pulso para 0° (en us / ticks)
#define SERVO_MID_PULSE  1500  // Ancho de pulso para 90°
#define SERVO_MAX_PULSE  2500  // Ancho de pulso para 180°

class Servo {
public:
  Servo(TIM_HandleTypeDef *htim_value, uint32_t channel_value);
  ~Servo() = default;

  void setPosition(uint16_t angle);
  void setPulse(uint32_t pulse_us);
protected:
  TIM_HandleTypeDef *htim;
  uint32_t channel;
};


#endif /* INC_SERVO_HPP_ */
