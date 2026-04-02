/*
 * TofVL53L4CD2.cpp
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#include <TofVL53L4CD2.hpp>


TofVL53L4CD2::TofVL53L4CD2(GPIO_TypeDef* gpiox_value, uint16_t gpio_pin_value) : gpiox(gpiox_value), gpio_pin(gpio_pin_value) {
//  HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
  dev = 0x52;
  HAL_Delay(5);
}

TofVL53L4CD2::~TofVL53L4CD2() {
	// TODO Auto-generated destructor stub
}

void TofVL53L4CD2::init(Dev_t dev_valor) {
  HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_SET);
  HAL_Delay(5);
  status = VL53L4CD_SetI2CAddress(dev, dev_valor);
  dev = dev_valor;

  /* (Optional) Check if there is a VL53L4CD sensor connected */
  if (!status) {
    status = VL53L4CD_GetSensorId(dev, &sensor_id);
    if(status || (sensor_id != 0xEBAA))
    {
//        printf("VL53L4CD not detected at requested address\n");
        //      return status_left;
        init_failed = 1;
    }

    /* (Mandatory) Init VL53L4CD sensor */
    status = VL53L4CD_SensorInit(dev);
    if(status)
    {
//        printf("VL53L4CD ULD Loading failed\n");
  //      return status_left;
        init_failed = 1;
    }

//    printf("VL53L4CD ULD ready !\n");
  }

  /*********************************/
  /*     Sensor configuration      */
  /*********************************/

  /* Program the lowest possible TimingBudget, without enabling the
   * low power mode. This gives the highest ranging frequency (100Hz) */
  if(!status) {
    status = VL53L4CD_SetRangeTiming(dev, 10, 0);
    if(status)
    {
//        printf("VL53L4CD_SetRangeTiming failed with status %u\n", status);
        init_failed = 1;
  //      return status_left;
    }
  }

  /*********************************/
  /*         Ranging loop          */
  /*********************************/
  if(!status) {
    status = VL53L4CD_StartRanging(dev);
  }

}

bool TofVL53L4CD2::isReady() {
  uint8_t is_ready;
  bool ready = false;
  if(!init_failed) {
	status = VL53L4CD_CheckForDataReady(dev, &is_ready);
  }
  if (!init_failed && !status && is_ready) {
	  ready = true;
  }
  return ready;
}

uint16_t TofVL53L4CD2::getDistance(uint16_t min_distance, uint16_t max_distance) {
  uint16_t distance = 999;
  if(isReady()) {
    /* (Mandatory) Clear HW interrupt to restart measurements */
    VL53L4CD_ClearInterrupt(dev);

    /* Read measured distance. RangeStatus = 0 means valid data */
    VL53L4CD_GetResult(dev, &results);
    if (results.range_status == 0) {
      if (results.distance_mm > min_distance && results.distance_mm < max_distance) {
        distance = results.distance_mm;
      }
    }
    last_distance = distance;
    return distance;
  }
  else {
    return last_distance;
  }
}

uint16_t TofVL53L4CD2::getLastDistance() {
	return last_distance;
}





