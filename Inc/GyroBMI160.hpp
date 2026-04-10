/*
 * LedBase.h
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#ifndef GYROBMI160_H_
#define GYROBMI160_H_

#include <bmi160.hpp>
#include <stm32f4xx_hal.h>


class GyroBMI160  {
public:
  GyroBMI160(I2C_HandleTypeDef *hi2c_value);
  virtual ~GyroBMI160();

  virtual void read_values();
  virtual int16_t get_accel_x();
  virtual int16_t get_accel_y();
  virtual int16_t get_accel_z();
  virtual int16_t get_gyro_x();
  virtual int16_t get_gyro_y();
  virtual int16_t get_gyro_z();

private:
  I2C_HandleTypeDef *hi2c;
  BMI160_RawData_t myAccel, myGyro;


  uint32_t tick_ini = 0;
  uint32_t tick_change = 0;
  uint32_t tick_tmp = 0;
  int16_t prev_accel_y = 0;
};

#endif /* GYROBMI160_H_ */
