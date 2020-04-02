#include "frame.h"
uint8_t DataToPrint[40];

void clear_buffer_(char buffer[])
{
	for(int i = 0; i < strlen(buffer); i++)
	{
		buffer[i] = 0;
	}
}

void shift_register_init()
{
	for(int i = 0; i < liczba_pomiarow_w_ramce; i++)
	{
		pomiary_shift_register[i] = 0.0;
	}
}

void shift_register_add(float new)	//przesuniecie w prawo
{
	for(int i = liczba_pomiarow_w_ramce - 1; i > 0; i--)
	{
		pomiary_shift_register[i] = pomiary_shift_register[i-1];
	}
	pomiary_shift_register[0] = new;
}

void clear_BIG_database()
{
	for(int i = 0; i < BIG_DATABASE_NUMBER_OF_TEMPERATURES_BYTES; i++)
	{
		BIG_database_temperatures_bytes[i] = 0;
	}
}

void big_database_temperature_history_store(float temperature)
{

	fToTwelveBits(temperature, BIG_database_temperatures_bytes, &BIG_database_temperatures_index);

			//NARAZIE ZROBIE BEZ PRZEPELNIANIA
	if(BIG_database_temperatures_index > BIG_DATABASE_NUMBER_OF_TEMPERATURES - 1)
	{
		BIG_database_temperatures_index = 0;
		/*
		database_temperatures_index--;
		//database_temperature_history_shift_left();
		shift_left_order = 1;
		*/
	}
	//else database_temperatures_index++;	//inkrementacja indeksu

}

void database_temperature_history_store(float temperature)
{
	/*
	HAL_Delay(100);
	strcpy(DataToPrint, " database_temperature_history_store:  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);
	*/
	/*
	HAL_Delay(100);
	sprintf(DataToPrint, " *numOfMeasurement: %i", database_temperatures_index);
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);
	*/
	fToTwelveBits(temperature, database_temperatures_bytes, &database_temperatures_index);

	if(database_temperatures_index >= DATABASE_NUMBER_OF_TEMPERATURES - 1)
	{
		/*
		HAL_Delay(100);
		strcpy(DataToPrint, " indeks większy  ");
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);
		*/

		database_temperatures_index--;
		//database_temperature_history_shift_left();
		shift_left_order = 1;
	}
	else database_temperatures_index++;	//inkrementacja indeksu
}
void database_temperature_history_shift_left(void)
{
	/*
	HAL_Delay(100);
	strcpy(DataToPrint, " database_temperature_history_shift_left  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);
	*/

	for(int i = 0; i < DATABASE_NUMBER_OF_TEMPERATURES_BYTES - 3; i++)
	{
		/*
		HAL_Delay(100);
		strcpy(DataToPrint, " shift left iteracja  ");
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);
		*/

		database_temperatures_bytes[i] = database_temperatures_bytes[i + 3];
	}
	database_temperatures_bytes[DATABASE_NUMBER_OF_TEMPERATURES_BYTES - 1] = 0;
	database_temperatures_bytes[DATABASE_NUMBER_OF_TEMPERATURES_BYTES - 2] = 0;
	database_temperatures_bytes[DATABASE_NUMBER_OF_TEMPERATURES_BYTES - 3] = 0;

	/*
	HAL_Delay(100);
	strcpy(DataToPrint, " database_temperature_history_shift_left-bufor-temperatury:  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	HAL_Delay(100);
	CDC_Transmit_FS(database_temperatures_bytes, 3);
	*/
}

/*
 * @brief This function puts one temperature measurement into table of measurements. Each time measurement number
 *        is used to calculate the pointer where this measurement should be placed in the table.
 *
 * measurement - temperature measurement,
 * twelveBitsMeasurement - table of measurements,
 * numOfMeasurement - measurement number.
 *
 * Float measurement is multiplied by 10 and then it is convert to int value
 */
//Wywołanie		pomiar			       macierz przechowująca bajty
//fToTwelveBits(temperature,           database_temperatures_bytes, &database_temperatures_index, debug_measurementInt);
void fToTwelveBits(float measurement, uint8_t *twelveBitsMeasurement, uint8_t *numOfMeasurement)
{
	/*
	HAL_Delay(100);
	strcpy(DataToPrint, " fToTwelveBits:  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);

	HAL_Delay(100);
	sprintf(DataToPrint, " *numOfMeasurement: %i", *numOfMeasurement);
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);
	*/

    int measurementInt = (int) (measurement * 10);
    iToTwelveBits(measurementInt, twelveBitsMeasurement, numOfMeasurement);
}

void iToTwelveBits(int measurement, uint8_t *twelveBitsMeasurement, uint8_t *numOfMeasurement) {
	HAL_Delay(100);
	strcpy(DataToPrint, " iToTwelveBits:  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);

	//t1 to jest zasadniczo ten pomiar co wchodzi.
	//tu go tylko ograniczam do 11 bitów, ale przy tych liczbach to nie powinno mieć znaczenia
    uint16_t t1 = measurement & 0x07FF;

    HAL_Delay(100);
    sprintf(DataToPrint, " t1: %i", t1);
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);

    /* Set bit 12 for "-" values */
	//Dla ujemnych wartosci
    if (measurement < 0) {
        t1 = (0x1000 + measurement) & 0x07FF;
        t1 = t1 | 0x0800;
    }

    HAL_Delay(100);
	sprintf(DataToPrint, " *numOfMeasurement: %i", *numOfMeasurement);
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);

    int p = ((*numOfMeasurement)) * 1.5;

    HAL_Delay(100);
	sprintf(DataToPrint, " p: %i", p);
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	clear_buffer_(DataToPrint);

    if (*numOfMeasurement % 2 == 0)
    {
    	HAL_Delay(100);
		strcpy(DataToPrint, " *numOfM parzyste ");
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);

        twelveBitsMeasurement[p] = t1 & 0xFF;			//do wektora w indeks p wrzucam prawe 8 bitów
        twelveBitsMeasurement[p + 1] = t1 >> 8 & 0x0F;	//do indeksu p+1 ląduje lewe 8 bitów

        HAL_Delay(100);
		sprintf(DataToPrint, " twelveBitsMeasurement[p]: %i ", twelveBitsMeasurement[p]);
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);

		HAL_Delay(100);
		sprintf(DataToPrint, " twelveBitsMeasurement[p+1]: %i ", twelveBitsMeasurement[p+1]);
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);


    }
    else
    {

    	HAL_Delay(100);
		strcpy(DataToPrint, " *numOfM nieparzyste ");
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);

        p += .5;
        uint8_t temp = twelveBitsMeasurement[p];
        temp = temp | ((t1 & 0x0F) << 4);
        uint8_t temp2 = t1 >> 4 & 0xFF;
        twelveBitsMeasurement[p] = temp;
        twelveBitsMeasurement[p + 1] = temp2;

        HAL_Delay(100);
		sprintf(DataToPrint, " twelveBitsMeasurement[p]: %i ", twelveBitsMeasurement[p]);
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);

		HAL_Delay(100);
		sprintf(DataToPrint, " twelveBitsMeasurement[p+1]: %i ", twelveBitsMeasurement[p+1]);
		CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
		clear_buffer_(DataToPrint);



    }
    *numOfMeasurement += 1;
    HAL_Delay(100);
	strcpy(DataToPrint, " iToTwelveBits-bufor-temperatury:  ");
	CDC_Transmit_FS(DataToPrint, strlen(DataToPrint));
	HAL_Delay(100);
	CDC_Transmit_FS(twelveBitsMeasurement, 3);
}

/**
 * @brief This function calculates crc16 of buffer.
 * 				buffer - buffer with bytes,
 * 				size - size of buffer expressed in number of bytes
 */
uint16_t crc16(uint8_t* buffer, uint16_t size)
{
	uint16_t checksum;
	checksum = CRC_INIT; // Init value for CRC calculation
	for(int i = 0; i < size; i++)
	{
		checksum = culCalcCRC(*(buffer + i), checksum);
	}
	return checksum;
}

/**
 * @brief https://www.ti.com/lit/an/swra111e/swra111e.pdf
 */
uint16_t culCalcCRC(uint8_t crcData, uint16_t crcReg)
{
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if ((crcReg & 0x8000) >> 8^ (crcData & 0x80))
			crcReg = (crcReg << 1) ^ CRC16_POLY;
		else
			crcReg = (crcReg << 1);
		crcData <<= 1;
	}
	return crcReg;
}

void frame_initializer()
{
	//ramka z pdf-a
//001F425342531877068B23300811001500010086E16B59FF030058029BA8CE0000- to jest mój rezultat crc dla tej ramki
//001F425342531877068B230380110051001000681EB695FF30008520B9D9B06BB2

	my_frame.length_of_received_data[0] = 0x00;	//stałe
	my_frame.length_of_received_data[1] = 0x1F;
	//Proper frame
	my_frame.synchronization_word[0] = 0x42;	//stałe
	my_frame.synchronization_word[1] = 0x53;
	my_frame.synchronization_word[2] = 0x42;
	my_frame.synchronization_word[3] = 0x53;
	my_frame.length_of_frame = 0x18;	//stałe
	my_frame.ID[0] = 0x77;		//stałe dla egzemplarza
	my_frame.ID[1] = 0x06;
	my_frame.ID[2] = 0x8B;
	my_frame.ID[3] = 0x23;
	my_frame.info_bits[0] = 0x30;	// zostawiam jak jest
	my_frame.info_bits[1] = 0x08;
	my_frame.info_bits[2] = 0x11;
	my_frame.info_bits[3] = 0x00;
	//Data
	my_frame.VID[0] = 0x15;		//stałe
	my_frame.VID[1] = 0x00;
	my_frame.PID[0] = 0x01;		//stałe
	my_frame.PID[1] = 0x00;
	my_frame.time_clock[0] = 0x86;		//licznik pomiarów, rzutuję w mainie
	my_frame.time_clock[1] = 0xE1;
	my_frame.time_clock[2] = 0x6B;
	my_frame.time_clock[3] = 0x59;
	my_frame.temperatures[0] = 0xFF;
	my_frame.temperatures[1] = 0x03;
	my_frame.temperatures[2] = 0x00;
	my_frame.time_of_period[0] = 0x58;	//rzutuję w mainie
	my_frame.time_of_period[1] = 0x02;
	my_frame.battery_voltage = 0x00;	//wstawiam zero
	my_frame._CRC[0] = 0x00;
	my_frame._CRC[1] = 0x00;
	//Additional
	my_frame.RSSI = 0x00;	//wstawiam zero
	my_frame.LQI = 0x00;	//wstawiam zero

	//Testowa ramka z gotowego urządzenia
//jU0A_001F_42534253_18_99190020_30081100_11F6_685E_EFF00EEF_F00EEF_F00E_1E_67D6_C9_1F;
//jU0A001F4253425318991900203008110011F6685EEFF00EEFF00EEFF00E1E67D6C91F
    //001F4253425318991900203008110011F6685EEFF00EEFF00EEFF00E1E297EC91F	//wyznaczone przeze mnie
	  																		//jest różnica w CRC
/*

	my_frame.length_of_received_data[0] = 0x00;	//stałe
	my_frame.length_of_received_data[1] = 0x1F;
	//Proper frame
	my_frame.synchronization_word[0] = 0x42;	//stałe
	my_frame.synchronization_word[1] = 0x53;
	my_frame.synchronization_word[2] = 0x42;
	my_frame.synchronization_word[3] = 0x53;
	my_frame.length_of_frame = 0x18;	//stałe
	my_frame.ID[0] = 0x99;		//stałe dla egzemplarza
	my_frame.ID[1] = 0x19;
	my_frame.ID[2] = 0x00;
	my_frame.ID[3] = 0x20;
	my_frame.info_bits[0] = 0x30;	// zostawiam jak jest
	my_frame.info_bits[1] = 0x08;
	my_frame.info_bits[2] = 0x11;
	my_frame.info_bits[3] = 0x00;
	//Data
	my_frame.VID[0] = 0x11;		//stałe
	my_frame.VID[1] = 0xF6;
	my_frame.PID[0] = 0x68;		//stałe
	my_frame.PID[1] = 0x5E;
	my_frame.time_clock[0] = 0xEF;		//licznik pomiarów, rzutuję w mainie
	my_frame.time_clock[1] = 0xF0;
	my_frame.time_clock[2] = 0x0E;
	my_frame.time_clock[3] = 0xEF;
	my_frame.temperatures[0] = 0xF0;
	my_frame.temperatures[1] = 0x0E;
	my_frame.temperatures[2] = 0xEF;
	my_frame.time_of_period[0] = 0xF0;	//rzutuję w mainie
	my_frame.time_of_period[1] = 0x0E;
	my_frame.battery_voltage = 0x1E;	//wstawiam zero
	my_frame._CRC[0] = 0x00;
	my_frame._CRC[1] = 0x00;
	//Additional
	my_frame.RSSI = 0xC9;	//wstawiam zero
	my_frame.LQI = 0x1F;	//wstawiam zero
	*/



}

void frame_rewrite()
{
	my_frame.whole_frame[0] = my_frame.length_of_received_data[0];
	my_frame.whole_frame[1] = my_frame.length_of_received_data[1];
	//Proper frame
	my_frame.whole_frame[2] = my_frame.synchronization_word[0];
	my_frame.whole_frame[3] = my_frame.synchronization_word[1];
	my_frame.whole_frame[4] = my_frame.synchronization_word[2];
	my_frame.whole_frame[5] = my_frame.synchronization_word[3];
	my_frame.whole_frame[6] = my_frame.length_of_frame;
	my_frame.whole_frame[7] = my_frame.ID[0];
	my_frame.whole_frame[8] = my_frame.ID[1];
	my_frame.whole_frame[9] = my_frame.ID[2];
	my_frame.whole_frame[10] = my_frame.ID[3];
	my_frame.whole_frame[11] = my_frame.info_bits[0];
	my_frame.whole_frame[12] = my_frame.info_bits[1];
	my_frame.whole_frame[13] = my_frame.info_bits[2];
	my_frame.whole_frame[14] = my_frame.info_bits[3];
	//Data
	my_frame.whole_frame[15] = my_frame.VID[0];
	my_frame.whole_frame[16] = my_frame.VID[1];
	my_frame.whole_frame[17] = my_frame.PID[0];
	my_frame.whole_frame[18] = my_frame.PID[1];
	my_frame.whole_frame[19] = my_frame.time_clock[0];
	my_frame.whole_frame[20] = my_frame.time_clock[1];
	my_frame.whole_frame[21] = my_frame.time_clock[2];
	my_frame.whole_frame[22] = my_frame.time_clock[3];
	my_frame.whole_frame[23] = my_frame.temperatures[0];
	my_frame.whole_frame[24] = my_frame.temperatures[1];
	my_frame.whole_frame[25] = my_frame.temperatures[2];
	my_frame.whole_frame[26] = my_frame.time_of_period[0];
	my_frame.whole_frame[27] = my_frame.time_of_period[1];
	my_frame.whole_frame[28] = my_frame.battery_voltage;
	my_frame.whole_frame[29] = my_frame._CRC[0];
	my_frame.whole_frame[30] = my_frame._CRC[1];
	//Additional
	my_frame.whole_frame[31] = my_frame.RSSI;
	my_frame.whole_frame[32] = my_frame.LQI;

}

void cast_counter(uint32_t counter)
{
	uint8_t buffer8 = 0;
	buffer8 = (uint8_t)(counter >> 24);	//najbardziej znaczący Bajt
	my_frame.time_clock[0] = buffer8;
	buffer8 = (uint8_t)(counter >> 16);
	my_frame.time_clock[1] = buffer8;
	buffer8 = (uint8_t)(counter >> 8);
	my_frame.time_clock[2] = buffer8;
	buffer8 = (uint8_t)(counter);
	my_frame.time_clock[3] = buffer8;

}

void cast_period(uint16_t period)
{
	uint8_t buffer8 = 0;
	buffer8 = (uint8_t)(period >> 8);
	my_frame.time_of_period[0] = buffer8;
	buffer8 = (uint8_t)(period);
	my_frame.time_of_period[1] = buffer8;
}

void fill_CRC_buffer()
{
	for(int i = 0; i < 14; i++)
	{
		my_frame.CRC_buffer[i] = my_frame.whole_frame[i+15];
	}
}

void cast_CRC(uint16_t calculated_CRC)
{
	uint8_t buffer8 = 0;
	buffer8 = (uint8_t)(calculated_CRC >> 8);
	my_frame._CRC[0] = buffer8;
	buffer8 = (uint8_t)(calculated_CRC);
	my_frame._CRC[1] = buffer8;
}

void cast_temperature()
{
	for(int i = 0; i < 3; i++)
	{
		my_frame.temperatures[i] = database_temperatures_bytes[i];
	}
}

