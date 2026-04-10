/*
 * IRReceiver.cpp
 *
 *  Created on: May 24, 2025
 *      Author: froilan
 */

#include <IRReceiver.hpp>


IRReceiver::IRReceiver(TIM_HandleTypeDef* htim_value, uint32_t channel_value) : htim(htim_value), channel(channel_value) {
  is_data_ready_nec = false;
  decoded_data_nec = 0;
  HAL_TIM_IC_Start_IT(htim, channel);
  ir_type = enum_ir_type::NEC;
}

IRReceiver::~IRReceiver() {
	// TODO Auto-generated destructor stub
}


void IRReceiver::setNecType(){
	ir_type = enum_ir_type::NEC;
  is_data_ready_nec = false;
  is_data_ready_rc5sumo = false;
}

bool IRReceiver::isNecType(){
	return ir_type == enum_ir_type::NEC;

}

void IRReceiver::setRC5SumoType(){
	ir_type = enum_ir_type::RC5SUMO;
  is_data_ready_nec = false;
  is_data_ready_rc5sumo = false;
}

bool IRReceiver::isRC5SumoType(){
	return ir_type == enum_ir_type::RC5SUMO;
}


void IRReceiver::adquireData() {
  switch (ir_type) {
    case enum_ir_type::NEC: {
      uint32_t ir_current_value_ms = 0;
      uint32_t ir_diference_ms = 0;
      ir_current_value_ms = HAL_TIM_ReadCapturedValue(htim, channel);
      ir_diference_ms = ir_current_value_ms - ir_previous_value_nec_ms;
      ir_previous_value_nec_ms = ir_current_value_ms;

      if (ir_diference_ms > PULSE_BETWEEN_COMM_MIN) {
        __HAL_TIM_SET_COUNTER(htim, 0);
        ir_previous_value_nec_ms = 0;
        ir_step_nec = START_PULSE_BRUST;
      }
      else {
        switch(ir_step_nec){
        case (enum_ir_nec_protocol::START_PULSE_BRUST):
          if (START_PULSE_BRUST_MIN_MS < ir_diference_ms && ir_diference_ms < START_PULSE_BRUST_MAX_MS) { //9 ms
          ir_step_nec = enum_ir_nec_protocol::SPACE_SELECTOR;
          }
          else {
          ir_step_nec = enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          }
          break;
        case (enum_ir_nec_protocol::SPACE_SELECTOR):
          if (SPACE_SELECTOR_FULL_VALUE_MIN_MS < ir_diference_ms && ir_diference_ms < SPACE_SELECTOR_FULL_VALUE_MAX_MS) {// 4.5ms full value
          ir_step_nec = enum_ir_nec_protocol::FULL_VALUE_PULSE_BRUST;
          ir_value_position_nec = 0;
          decoded_data_tmp_nec = 0;
          value_repetitions_nec = 0;
          }
          else if (SPACE_SELECTOR_REPEAT_VALUE_MIN_MS < ir_diference_ms && ir_diference_ms < SPACE_SELECTOR_REPEAT_VALUE_MAX_MS) {// 2.25ms repeat code
          if (is_valid_value_nec) {
            ir_step_nec = enum_ir_nec_protocol::REPETITION_VALUE_PULSE_BRUST;
          }
          else {
            ir_step_nec =enum_ir_nec_protocol::ERROR_VALUE;
            is_valid_value_nec = false;
          }
          }
          else {
          ir_step_nec =enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          }
          break;
        case (enum_ir_nec_protocol::FULL_VALUE_PULSE_BRUST):
          if (FULL_VALUE_PULSE_BRUST_MIN_MS < ir_diference_ms && ir_diference_ms < FULL_VALUE_PULSE_BRUST_MAX_MS) { // 0.5625ms
          if (ir_value_position_nec == NUM_BITS_VALUE){
            ir_step_nec = END_FULL_VALUE;
            decoded_data_nec = decoded_data_tmp_nec;
            is_valid_value_nec = true;
          }
          else {
            ir_step_nec = enum_ir_nec_protocol::FULL_VALUE;
          }
          }
          else {
          ir_step_nec =enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          }
          break;
        case (enum_ir_nec_protocol::FULL_VALUE):
          if (FULL_VALUE_ONE_MIN_MS < ir_diference_ms && ir_diference_ms < FULL_VALUE_ONE_MAX_MS) { //1.6875ms para los 1
          decoded_data_tmp_nec |= (1UL << (NUM_BITS_VALUE - ir_value_position_nec - 1) );
          ir_value_position_nec++;
          ir_step_nec = enum_ir_nec_protocol::FULL_VALUE_PULSE_BRUST;
          }
          else if (FULL_VALUE_ZERO_MIN_MS < ir_diference_ms && ir_diference_ms < FULL_VALUE_ZERO_MAX_MS) { //0.5625ms para los 0
          ir_value_position_nec++;
          ir_step_nec = enum_ir_nec_protocol::FULL_VALUE_PULSE_BRUST;
          //no hace falta poner el valor ya que está a cero
          }
          else {
          ir_step_nec = enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          }
          break;
        case (enum_ir_nec_protocol::REPETITION_VALUE_PULSE_BRUST):
          if (REPETITION_VALUE_PULSE_BRUST_MIN_MS < ir_diference_ms && ir_diference_ms < REPETITION_VALUE_PULSE_BRUST_MAX_MS) { // 0.5625ms
          ir_step_nec = enum_ir_nec_protocol::END_REPETITION_VALUE;  //envia un solo pulso y no hace nada mas
          value_repetitions_nec++;
          }
          else {
          ir_step_nec =enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          }
          break;
        case (enum_ir_nec_protocol::END_FULL_VALUE): //si llega al final y hay mas datos es que hay algun error
        case (enum_ir_nec_protocol::END_REPETITION_VALUE):
        default:
          ir_step_nec =enum_ir_nec_protocol::ERROR_VALUE;
          is_valid_value_nec = false;
          break;
        }
      }

    //  if (ir_step_nec == END_FULL_VALUE || ir_step_nec == END_REPETITION_VALUE) // Si ya capturamos todos los bits
      if (ir_step_nec == END_FULL_VALUE) // Si ya capturamos todos los bits
      {
      is_data_ready_nec = true;
      HAL_TIM_IC_Stop_IT(htim, channel); // Detenemos para procesar
      }
      break;
    }
    case enum_ir_type::RC5SUMO: {
      uint32_t current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);

      // Calcular diferencia de tiempo (maneja el desbordamiento del timer)
      if (current_capture > last_capture_rc5sumo)
          diff_time_rc5sumo = current_capture - last_capture_rc5sumo;
      else
          diff_time_rc5sumo = (0xFFFFFFFF - last_capture_rc5sumo) + current_capture;

      last_capture_rc5sumo = current_capture;



      // --- Lógica de Decodificación RC-5 ---
      GPIO_PinState bitstatus = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
      // Si el tiempo es mayor a un bit y medio, es un error o el inicio de una trama
      if (diff_time_rc5sumo > (T_BIT_ENTERO + TOLERANCIA)) {
          buffer_bits_rc5sumo = 0;
          contador_bits_rc5sumo = 0;
          ir_step_rc5sumo = STEP_01_START_PULSE_1;
      }
      else {
        switch ( ir_step_rc5sumo ) {
          case STEP_01_START_PULSE_1:
            // Evaluamos si es un pulso corto o largo
            if (diff_time_rc5sumo > (T_MITAD_BIT - TOLERANCIA) && diff_time_rc5sumo < (T_MITAD_BIT + TOLERANCIA)) {
              if (bitstatus == GPIO_PIN_SET) {
                buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
                previous_bit_rc5sumo = 1;
                contador_bits_rc5sumo++;
                ir_step_rc5sumo = STEP_02_START_PULSE_2;
              }
              else {
                ir_step_prev_rc5sumo = ir_step_rc5sumo;
                ir_step_rc5sumo = STEP_99_ERROR;
              }
            }
            else if (diff_time_rc5sumo > (T_BIT_ENTERO - TOLERANCIA) && diff_time_rc5sumo < (T_BIT_ENTERO + TOLERANCIA)) {
              ir_step_prev_rc5sumo = ir_step_rc5sumo;
              ir_step_rc5sumo = STEP_99_ERROR;
            }
            else {
              ir_step_prev_rc5sumo = ir_step_rc5sumo;
              ir_step_rc5sumo = STEP_99_ERROR;
            }
            break;
          case STEP_02_START_PULSE_2:
            // Evaluamos si es un pulso corto o largo
            if (diff_time_rc5sumo > (T_MITAD_BIT - TOLERANCIA) && diff_time_rc5sumo < (T_MITAD_BIT + TOLERANCIA)) {
              if (bitstatus == GPIO_PIN_RESET) {
                buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
                previous_bit_rc5sumo = 1;
                contador_bits_rc5sumo++;
                ir_step_rc5sumo = STEP_03_TONGUE_BIT;
              }
              else {
                ir_step_prev_rc5sumo = ir_step_rc5sumo;
                ir_step_rc5sumo = STEP_99_ERROR;
              }
            }
            else if (diff_time_rc5sumo > (T_BIT_ENTERO - TOLERANCIA) && diff_time_rc5sumo < (T_BIT_ENTERO + TOLERANCIA)) {
              ir_step_prev_rc5sumo = ir_step_rc5sumo;
              ir_step_rc5sumo = STEP_99_ERROR;
            }
            else {
              ir_step_prev_rc5sumo = ir_step_rc5sumo;
              ir_step_rc5sumo = STEP_99_ERROR;
            }
            break;
          case STEP_03_TONGUE_BIT:
            read_rc5_bit(STEP_04_FIXED_ADDRESS_1);
            break;
          case STEP_04_FIXED_ADDRESS_1:
            read_rc5_bit(STEP_05_FIXED_ADDRESS_2);
            break;
          case STEP_05_FIXED_ADDRESS_2:
            read_rc5_bit(STEP_06_FIXED_ADDRESS_3);
            break;
          case STEP_06_FIXED_ADDRESS_3:
            read_rc5_bit(STEP_07_FIXED_ADDRESS_4);
            break;
          case STEP_07_FIXED_ADDRESS_4:
            read_rc5_bit(STEP_08_FIXED_ADDRESS_5);
            break;
          case STEP_08_FIXED_ADDRESS_5:
            read_rc5_bit(STEP_09_DOHYO_IDENTIFIER_1);
            break;
          case STEP_09_DOHYO_IDENTIFIER_1:
            read_rc5_bit(STEP_10_DOHYO_IDENTIFIER_2);
            break;
          case STEP_10_DOHYO_IDENTIFIER_2:
            read_rc5_bit(STEP_11_DOHYO_IDENTIFIER_3);
            break;
          case STEP_11_DOHYO_IDENTIFIER_3:
            read_rc5_bit(STEP_12_DOHYO_IDENTIFIER_4);
            break;
          case STEP_12_DOHYO_IDENTIFIER_4:
            read_rc5_bit(STEP_13_DOHYO_IDENTIFIER_5);
            break;
          case STEP_13_DOHYO_IDENTIFIER_5:
            read_rc5_bit(STEP_14_ON_OFF);
            break;
          case STEP_14_ON_OFF:
            read_rc5_bit(STEP_15_END);
            break;
          case STEP_15_END:
            ir_step_prev_rc5sumo = ir_step_rc5sumo;
            ir_step_rc5sumo = STEP_99_ERROR;
            break;
          case STEP_99_ERROR:
            read_rc5_bit(STEP_99_ERROR);
            break;
          default:
            ir_step_prev_rc5sumo = ir_step_rc5sumo;
            ir_step_rc5sumo = STEP_99_ERROR;
        }
      }

      // Si completamos los 14 bits
      if (contador_bits_rc5sumo >= 14) {

          uint8_t command = (buffer_bits_rc5sumo >> 6) & 0x1F;
          if (command == F_E_RC5SUMO_COMMAND_PROGRAM || command == F_E_RC5SUMO_COMMAND_START_STOP) {
            resultado_rc5sumo = buffer_bits_rc5sumo; // GUARDAMOS EL VALOR FINAL
            is_data_ready_rc5sumo = true;              // Avisamos al main
          }
//          else {
//            is_data_ready_rc5sumo = false;              // Avisamos al main
//          }
          if (index_buffer_bits_rc5sumo < 100) {
            array_buffer_bits_rc5sumo[index_buffer_bits_rc5sumo] = buffer_bits_rc5sumo;
            index_buffer_bits_rc5sumo++;
          }



          buffer_bits_rc5sumo = 0;
          contador_bits_rc5sumo = 0;
          ir_step_rc5sumo = STEP_01_START_PULSE_1;
      }



      break;
    }
    default:
      break;
  }
}


uint8_t IRReceiver::read_rc5_bit(enum_ir_rc5sumo_protocol next_ir_step) {
  uint8_t bit_value = 0;
  // Evaluamos si es un pulso corto o largo
  GPIO_PinState current_ir_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
  if (diff_time_rc5sumo > (T_MITAD_BIT - TOLERANCIA) && diff_time_rc5sumo < (T_MITAD_BIT + TOLERANCIA)) {
    if (previous_bit_rc5sumo == 0 && current_ir_state == GPIO_PIN_RESET) {
//      buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
      bit_value = 0;
      previous_bit_rc5sumo = 0;
      short_signal_bit_rc5sumo = 1;
      contador_bits_rc5sumo++;

    }
    else if (short_signal_bit_rc5sumo == 1 && current_ir_state == GPIO_PIN_SET) {
      short_signal_bit_rc5sumo = 0;
      ir_step_rc5sumo = next_ir_step;
    }
    else if (previous_bit_rc5sumo == 1 && current_ir_state == GPIO_PIN_SET) {
      buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
      bit_value = 1;
      previous_bit_rc5sumo = 1;
      short_signal_bit_rc5sumo = 1;
      contador_bits_rc5sumo++;
    }
    else if (short_signal_bit_rc5sumo == 1 && current_ir_state == GPIO_PIN_RESET) {
      short_signal_bit_rc5sumo = 0;
      ir_step_rc5sumo = next_ir_step;
    }
    else {
      ir_step_prev_rc5sumo = ir_step_rc5sumo;
      ir_step_rc5sumo = STEP_99_ERROR;
    }
  }
  else if (diff_time_rc5sumo > (T_BIT_ENTERO - TOLERANCIA) && diff_time_rc5sumo < (T_BIT_ENTERO + TOLERANCIA)) {
    if (previous_bit_rc5sumo == 0 && current_ir_state == GPIO_PIN_RESET) {
      buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
      bit_value = 1;
      previous_bit_rc5sumo = 1;
      contador_bits_rc5sumo++;
      ir_step_rc5sumo = next_ir_step;
    }
    else if (previous_bit_rc5sumo == 1 && current_ir_state == GPIO_PIN_SET) {
//      buffer_bits_rc5sumo |= (1 << (13 - contador_bits_rc5sumo));
      bit_value = 0;
      previous_bit_rc5sumo = 0;
      contador_bits_rc5sumo++;
      ir_step_rc5sumo = next_ir_step;
    }
    else {
      ir_step_prev_rc5sumo = ir_step_rc5sumo;
      ir_step_rc5sumo = STEP_99_ERROR;
    }
  }
  else {
    ir_step_prev_rc5sumo = ir_step_rc5sumo;
    ir_step_rc5sumo = STEP_99_ERROR;
  }
  return bit_value;
}

bool IRReceiver::isDataReady() {
  bool is_ready = false;
  switch (ir_type) {
    case enum_ir_type::NEC:
      is_ready = is_data_ready_nec;
      break;
    case enum_ir_type::RC5SUMO:
      is_ready = is_data_ready_rc5sumo;
      break;
  }
  return is_ready;
}

uint32_t IRReceiver::getValueNec() {
  is_data_ready_nec = false;
  HAL_TIM_IC_Start_IT(htim, channel);
  return decoded_data_nec;
}

IRReceiver::ValueRC5Sumo IRReceiver::getValueRC5Sumo() {
  is_data_ready_rc5sumo = false;
  uint8_t command = (resultado_rc5sumo >> 6) & 0x1F;
  uint8_t address = resultado_rc5sumo & 0x3F;
  return {command, address};
//  re.command res.address
}
