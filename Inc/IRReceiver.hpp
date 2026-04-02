/*
 * IRReceiver.h
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#ifndef IRRECEIVER_H_
#define IRRECEIVER_H_

#include <stm32f4xx_hal.h>

///////////////////////////////////
// NEC
///////////////////////////////////
#define PULSE_BETWEEN_COMM_MIN 10000
#define START_PULSE_BRUST_MIN_MS 8500
#define START_PULSE_BRUST_MAX_MS 9250
#define SPACE_SELECTOR_FULL_VALUE_MIN_MS 4250
#define SPACE_SELECTOR_FULL_VALUE_MAX_MS 4750
#define SPACE_SELECTOR_REPEAT_VALUE_MIN_MS 2000
#define SPACE_SELECTOR_REPEAT_VALUE_MAX_MS 2500
#define FULL_VALUE_PULSE_BRUST_MIN_MS 320
#define FULL_VALUE_PULSE_BRUST_MAX_MS 812
#define FULL_VALUE_ONE_MIN_MS 1437
#define FULL_VALUE_ONE_MAX_MS 1937
#define FULL_VALUE_ZERO_MIN_MS 312
#define FULL_VALUE_ZERO_MAX_MS 812
#define REPETITION_VALUE_PULSE_BRUST_MIN_MS 320
#define REPETITION_VALUE_PULSE_BRUST_MAX_MS 812
#define NUM_BITS_VALUE 32

#define F_E_RC5SUMO_COMMAND_PROGRAM 0xb
#define F_E_RC5SUMO_COMMAND_START_STOP 0x7
///////////////////////////////////
// RC5 SUMO
///////////////////////////////////
// Tiempos en microsegundos (basado en un Timer a 1MHz)
#define T_MITAD_BIT    889   // Tiempo ideal mitad de bit
#define T_BIT_ENTERO  1778   // Tiempo ideal bit completo
#define TOLERANCIA     400   // Margen de error para el sensor IR


class IRReceiver  {
public:
  IRReceiver(TIM_HandleTypeDef* htim_value, uint32_t channel_value);
  virtual ~IRReceiver();

  virtual void adquireData();
  virtual bool isDataReady();
  virtual uint32_t getValueNec();

  struct ValueRC5Sumo {
    uint32_t command;
    uint32_t address;
  };
  virtual ValueRC5Sumo getValueRC5Sumo();

  virtual void setNecType();
  virtual bool isNecType();
  virtual void setRC5SumoType();
  virtual bool isRC5SumoType();

private:
  enum enum_ir_type {
    NEC,
	  RC5SUMO,
  };
  enum_ir_type ir_type;

  TIM_HandleTypeDef *htim;
  uint32_t channel;

  enum enum_ir_nec_protocol {
     START_PULSE_BRUST,
     SPACE_SELECTOR,
     FULL_VALUE_PULSE_BRUST,
     FULL_VALUE,
     REPETITION_VALUE_PULSE_BRUST,
     END_FULL_VALUE,
     END_REPETITION_VALUE,
     ERROR_VALUE
  };

  bool is_data_ready_nec;
  uint32_t decoded_data_nec;
  enum_ir_nec_protocol ir_step_nec;
  uint32_t ir_previous_value_nec_ms;
  bool is_valid_value_nec;
  uint8_t ir_value_position_nec;
  uint32_t decoded_data_tmp_nec;
  uint32_t value_repetitions_nec;

  enum enum_ir_rc5sumo_protocol { // <-- El nuevo nombre de tipo se define aquí al final
     STEP_01_START_PULSE_1,
     STEP_02_START_PULSE_2,
     STEP_03_TONGUE_BIT,
     STEP_04_FIXED_ADDRESS_1,
     STEP_05_FIXED_ADDRESS_2,
     STEP_06_FIXED_ADDRESS_3,
     STEP_07_FIXED_ADDRESS_4,
     STEP_08_FIXED_ADDRESS_5,
     STEP_09_DOHYO_IDENTIFIER_1,
     STEP_10_DOHYO_IDENTIFIER_2,
     STEP_11_DOHYO_IDENTIFIER_3,
     STEP_12_DOHYO_IDENTIFIER_4,
     STEP_13_DOHYO_IDENTIFIER_5,
     STEP_14_ON_OFF,
     STEP_15_END,
     STEP_99_ERROR
  };
  enum_ir_rc5sumo_protocol ir_step_rc5sumo;
  enum_ir_rc5sumo_protocol ir_step_prev_rc5sumo;
  uint8_t short_signal_bit_rc5sumo = 0;
  uint8_t previous_bit_rc5sumo = 0;

  /* Variables para la decodificación */
  uint32_t last_capture_rc5sumo = 0;
  uint32_t diff_time_rc5sumo = 0;

  /* Estructura del protocolo RC-5 (14 bits) */
  uint16_t buffer_bits_rc5sumo = 0;    // Variable temporal para ir armando el dato
  uint16_t array_buffer_bits_rc5sumo[100];    // array Variable temporal para ir armando el dato
  uint8_t index_buffer_bits_rc5sumo = 0;    // index Variable temporal para ir armando el dato
  uint8_t contador_bits_rc5sumo = 0;  // Cuenta cuántos bits llevamos
  uint16_t resultado_rc5sumo = 0;  // ¡Aquí se guardará el código final!
  bool is_data_ready_rc5sumo = false;     // Indica que tenemos un código nuevo para leer

  virtual uint8_t read_rc5_bit(enum_ir_rc5sumo_protocol next_ir_step);
};

#endif /* IRRECEIVER_H_ */
