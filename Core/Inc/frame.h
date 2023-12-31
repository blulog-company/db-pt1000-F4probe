/*
 * frame.h
 *
 *  Created on: 11 mar 2020
 *      Author: blulog
 */

#ifndef INC_FRAME_H_
#define INC_FRAME_H_

#include "stm32f4xx_hal.h"
#include "usb_device.h"

#define CRC16_POLY 0x8005
#define CRC_INIT 0xFFFF
#define DATABASE_NUMBER_OF_TEMPERATURES							2
#define DATABASE_NUMBER_OF_TEMPERATURES_BYTES					3
#define BIG_DATABASE_NUMBER_OF_TEMPERATURES								6
#define BIG_DATABASE_NUMBER_OF_TEMPERATURES_BYTES						9
uint8_t database_temperatures_bytes[DATABASE_NUMBER_OF_TEMPERATURES_BYTES];
uint16_t database_temperatures_index;		/* 0 means that next measurement will be stored at address 0 */
uint8_t BIG_database_temperatures_bytes[BIG_DATABASE_NUMBER_OF_TEMPERATURES_BYTES];
uint16_t BIG_database_temperatures_index;		/* 0 means that next measurement will be stored at address 0 */
uint8_t shift_left_order;
#define liczba_pomiarow_w_ramce 6
float pomiary_shift_register[liczba_pomiarow_w_ramce];
#define ID_HEX_address 0x080001C4

void database_temperature_history_store(float temperature);
void database_temperature_history_shift_left(void);
uint16_t crc16(uint8_t* buffer, uint16_t size);
uint16_t culCalcCRC(uint8_t crcData, uint16_t crcReg);
void fToTwelveBits(float measurement, uint8_t *twelveBitsMeasurement, uint8_t *numOfMeasurement);
void iToTwelveBits(int measurement, uint8_t *twelveBitsMeasurement, uint8_t *numOfMeasurement);

void clear_buffer_(char buffer[]);
void shift_register_init();
void shift_register_add(float new);
void big_database_temperature_history_store(float temperature);
void clear_BIG_database();

struct frame
{
	uint8_t length_of_received_data[2];
	//Proper frame
	uint8_t synchronization_word[4];
	uint8_t length_of_frame;
	uint8_t ID[4];
	uint8_t info_bits[4];
	//Data
	uint8_t VID[2];
	uint8_t PID[2];
	uint8_t time_clock[4];
	uint8_t temperatures[3];
	uint8_t time_of_period[2];
	uint8_t battery_voltage;
	uint8_t _CRC[2];
	//Additional
	uint8_t RSSI;
	uint8_t LQI;

	uint8_t whole_frame[33];
	uint8_t CRC_buffer[14];

} my_frame;

struct BIG_frame
{
	uint8_t length_of_received_data[2];
	//Proper frame
	uint8_t synchronization_word[4];
	uint8_t length_of_frame;
	uint8_t ID[4];
	uint8_t info_bits[4];
	//Data
	uint8_t VID[2];
	uint8_t PID[2];
	uint8_t time_clock[4];
	uint8_t temperatures[BIG_DATABASE_NUMBER_OF_TEMPERATURES_BYTES];
	uint8_t time_of_period[2];
	uint8_t battery_voltage;
	uint8_t _CRC[2];
	//Additional
	uint8_t RSSI;
	uint8_t LQI;

	uint8_t whole_frame[39];
	uint8_t CRC_buffer[20];

} myBIG_frame;




void frame_initializer();
void cast_counter(uint32_t counter);
void fill_CRC_buffer();
void cast_CRC(uint16_t calculated_CRC);
void get_ID_from_memory();

void BIG_frame_initializer();
void BIG_fill_CRC_buffer();
void BIG_fill_CRC_buffer();
void BIG_cast_CRC(uint16_t calculated_CRC);
void BIG_cast_period(uint16_t period);
void BIG_get_ID_from_memory();



#endif /* INC_FRAME_H_ */
