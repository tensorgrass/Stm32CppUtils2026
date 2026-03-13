/*
 * TofVL53L4CD2.h
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#ifndef TOFVL53L4CD2_H_
#define TOFVL53L4CD2_H_

#include <VL53L4CD_api.h>
#include <stm32f4xx_hal.h>


class TofVL53L4CD2  {
public:
  TofVL53L4CD2(GPIO_TypeDef* gpiox_value, uint16_t gpio_pin_value);
  virtual ~TofVL53L4CD2();

  virtual void init(Dev_t dev_valor);
  virtual bool isReady();
  virtual uint16_t getDistance(uint16_t min_distance, uint16_t max_distance);
  virtual uint16_t getLastDistance();


private:
  GPIO_TypeDef* gpiox;
  uint16_t gpio_pin;
  VL53L4CD_ResultsData_t results;
  Dev_t dev;
  uint8_t status;
  uint8_t loop;

  uint16_t sensor_id;
  uint8_t init_failed = 0;
  uint16_t last_distance;
};

#endif /* TofVL53L4CD2_H_ */
