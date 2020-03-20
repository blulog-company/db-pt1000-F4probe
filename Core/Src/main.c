/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Max31865.h"
#include "frame.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_CS1_GPIO_Port SPI1_CS_GPIO_Port
#define SENSOR_CS1_Pin SPI1_CS_Pin
#define SENSOR_Connection_2Wire 2
#define SENSOR_Frequency_Filter 50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t DataToSend[40]; // Tablica zawierajaca dane do wyslania
Max31865_t  pt1000;
bool        pt1000isOK;
float       pt1000Temp;
float t;

char whole_frame_ascii[300];
char one_hex_number[4];
uint32_t licznik_pomiarow;
uint16_t period;
uint16_t frame_CRC;

uint8_t temp_ready;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void clear_buffer(uint8_t buffer[])
{
	for(int i = 0; i < 40; i++)
	{
		buffer[i] = 0;
	}
}

void send_text_over_usb(const char *text, uint8_t TxBuffer[])
{
	uint8_t MessageLength;
	strcpy(TxBuffer, text);
	MessageLength = strlen(TxBuffer);
	CDC_Transmit_FS(TxBuffer, MessageLength);
	clear_buffer(TxBuffer);
	HAL_Delay(100);
}

void usb_print(const char *text, uint8_t TxBuffer[])
{
	uint8_t MessageLength;
	strcpy(TxBuffer, text);
	MessageLength = strlen(TxBuffer);
	CDC_Transmit_FS(TxBuffer, MessageLength);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);

		//strcpy(DataToSend, "interrupt \r\n");
		//CDC_Transmit_FS(DataToSend, strlen(DataToSend));
		//HAL_Delay(1);
		usb_print("\r\n Przerwanie \r\n", DataToSend);
		pt1000isOK = Max31865_readTempC(&pt1000,&t);
		//if(!pt1000isOK)
		//	usb_print("blad odczytu \r\n", DataToSend);
		temp_ready = 1;
		licznik_pomiarow++;

	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM14_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	clear_buffer(DataToSend);
	HAL_Delay(4000);
	send_text_over_usb("Start Programu \r\n", DataToSend);

	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
	HAL_Delay(200);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);

	Max31865_init(&pt1000 ,&hspi1, SENSOR_CS1_GPIO_Port, SENSOR_CS1_Pin, SENSOR_Connection_2Wire, SENSOR_Frequency_Filter);
	HAL_Delay(3000);
	uint8_t initial_state_flag = 1;
	uint8_t counter = 0;
	licznik_pomiarow = 0;
	period = 5;
	frame_CRC = 0;
	temp_ready = 0;
	frame_initializer();
	frame_rewrite();

	for(int i = 0; i < 75; i++)
	{
		pt1000isOK = Max31865_readTempC(&pt1000,&t);
		if(!pt1000isOK)
			usb_print("blad odczytu \r\n", DataToSend);
		pt1000Temp = Max31865_Filter(t,pt1000Temp,0.1);   //  << For Smoothing data
		sprintf(DataToSend, "pomiar: %f \r\n", pt1000Temp);
		CDC_Transmit_FS(DataToSend, strlen(DataToSend));
	}

	HAL_Delay(200);
	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		  //send_text_over_usb("petla \r\n", DataToSend);
		if(temp_ready)
		{
			pt1000Temp = Max31865_Filter(t,pt1000Temp,0.1);   //  << For Smoothing data
			sprintf(DataToSend, "pomiar: %f \r\n", pt1000Temp);
			CDC_Transmit_FS(DataToSend, strlen(DataToSend));

			database_temperature_history_store(pt1000Temp);
			cast_counter(licznik_pomiarow);
			cast_period(period);
			cast_temperature();
			frame_rewrite();
			fill_CRC_buffer();
			frame_CRC = crc16(my_frame.CRC_buffer, 18);
			cast_CRC(frame_CRC);
			frame_rewrite();

			strcat(whole_frame_ascii,"JU0A");
			for(int i = 0; i < 33; i++)
			{
				if(my_frame.whole_frame[i] < 0x10)
				{
					strcat(whole_frame_ascii,"0");
				}

				sprintf(one_hex_number, "%X", my_frame.whole_frame[i]);
				strcat(whole_frame_ascii,one_hex_number);
			}
			send_text_over_usb("  ", DataToSend);
			CDC_Transmit_FS(whole_frame_ascii, strlen(whole_frame_ascii));
			clear_buffer_(whole_frame_ascii);

			temp_ready = 0;
		}
		HAL_Delay(50);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
