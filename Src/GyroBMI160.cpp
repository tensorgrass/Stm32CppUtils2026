/*
 * LedBase.cpp
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#include <GyroBMI160.hpp>


GyroBMI160::GyroBMI160(I2C_HandleTypeDef *hi2c_value) : hi2c(hi2c_value) {
  if (BMI160_Init(hi2c) == HAL_OK) {
	  // Sensor inicializado correctamente
  } else {
	  // Error: Revisa conexiones y resistencias Pull-up
//	      while(1);
  }
}

GyroBMI160::~GyroBMI160() {
	// TODO Auto-generated destructor stub
}


void GyroBMI160::read_values() {
	BMI160_Read_All(hi2c, &myAccel, &myGyro);
}

int16_t GyroBMI160::get_accel_x() {
	return myAccel.x;
}

int16_t GyroBMI160::get_accel_y() {
	return myAccel.y;
}

int16_t GyroBMI160::get_accel_z() {
	return myAccel.z;
}

int16_t GyroBMI160::get_gyro_x() {
	return myGyro.x;
}

int16_t GyroBMI160::get_gyro_y() {
	return myGyro.y;
}

int16_t GyroBMI160::get_gyro_z() {
	return myGyro.z;
}
