/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
#include "INA219.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	int pwm=0;

// SILNIK DC
	volatile int32_t count;
	volatile int16_t speed;
	volatile int16_t speed_prev;
	float number = 15; // dla czasu 10ms
// UART
	char msg[64];
	volatile int sign = 0;

	static uint16_t cnt = 1; // Licznik wyslanych wiadomosci
	uint16_t dataRozruch[5000];
	uint8_t data[100]; // Tablica przechowujaca wysylana wiadomosc.
	uint16_t size = 0;
	uint8_t Received[10];
	volatile float Kpw = 1.1f;
	volatile float Kiw = 1.2f;
	volatile float Kdw = 1.3f;
	volatile float Kpi = 1.4f;
	volatile float Kii = 1.5f;
	volatile float Kdi = 1.6f;
	volatile int WZZ = 2; //zadana predkosc obrotowa
	volatile int KRK = 1; //zadana ilosc krokow
	_Bool LogFlag=0;
	volatile _Bool Flaga = false;
	volatile _Bool Flaga2 = false;


	//pomiar pradu
	INA219_t ina219;
	volatile float vbus, vshunt, current;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM15) {
//		if (sign == 1) {
//			count = __HAL_TIM_GET_COUNTER(&htim4);
//			speed = number * count / 4; //obr min
//			__HAL_TIM_SET_COUNTER(&htim4, 0);
//		}
//		if (sign == -1) {
//			count = __HAL_TIM_GET_COUNTER(&htim4);
//			count = 65000 - count;
//			speed = -number * count / 4; //obr min
//			__HAL_TIM_SET_COUNTER(&htim4, 0);
//		}
		count = __HAL_TIM_GET_COUNTER(&htim4);
		speed = number * count / 4; //obr min
		__HAL_TIM_SET_COUNTER(&htim4, 0);

		if(LogFlag==1)
					{
							size = sprintf(data, "{ \"cnt1\" : %d , \"cnt2\" : %d  } \r\n", cnt, WZZ);
							HAL_UART_Transmit(&huart2, data, size, 1000);
					}
	//	vbus = INA219_ReadBusVoltage(&ina219);
	//	current = INA219_ReadCurrent(&ina219);
	//	vshunt = INA219_ReadShuntVolage(&ina219);
		if(!Flaga2)
		{
			Flaga=true;
		}
	}


	 if (htim->Instance == TIM3) {
		size = sprintf(data, "parm %d %d %.2f %.2f %.2f %.2f %.2f %.2f \r\n", cnt,WZZ,Kpw,Kiw,Kdw,Kpi,Kii,Kdi); // @suppress("Float formatting support")
		HAL_UART_Transmit(&huart2, data, size, 1000);

	}

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart2) {

		unsigned int value = 0; //wartość do wpisania do nastawy
		char str[3];
		uint8_t Received_str[3]; // słowo okreslajace nastawe
		uint8_t Received_val[5]; // zmienna pomocnicza do przechowywania wartości zadanej z UART
		//przepisanie wartości do nowego bufora ostatnie 5 znakow okresla liczbe do wpisania dla nastawy
		for (int i = 0; i < 5; i++) {
			Received_val[i] = Received[3 + i];
		}
		int rx_n = sscanf((char*) Received_val, "%d", &value);


		if (Received[0] == 'K' && Received[1] == 'P' && Received[2] == 'W') {
			//wpisanie wartosci do zmiennej

			if (rx_n == 1) {
				Kpw = value / 100.0f;

			}
		}

			// jesli komenda dotyczy Kiw( wzmocnienie calkowania petli predkosci) to ustaw wartosc
			if (Received[0] == 'K' && Received[1] == 'I' && Received[2] == 'W') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					Kiw = value / 100.0f;
				}
			}
			// jesli komenda dotyczy Kdw( wzmocnienie różniczkowania petli predkosci) to ustaw wartosc
			if (Received[0] == 'K' && Received[1] == 'D' && Received[2] == 'W') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					Kdw = value / 100.0f;
				}
			}

			// jesli komenda dotyczy Kpi( wzmocnienie petli pradu) to ustaw wartosc
			if (Received[0] == 'K' && Received[1] == 'P' && Received[2] == 'I') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					Kpi = value / 100.0f;
				}
			}
			// jesli komenda dotyczy Kii( wzmocnienie calkowania petli pradu) to ustaw wartosc
			if (Received[0] == 'K' && Received[1] == 'I' && Received[2] == 'I') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					Kii = value / 100.0f;
				}
			}
			// jesli komenda dotyczy Kdw( wzmocnienie różniczkowania petli pradu) to ustaw wartosc
			if (Received[0] == 'K' && Received[1] == 'D' && Received[2] == 'I') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					Kdi = value / 100.0f;
				}
			}

			//jeśli komenda dotyczy predkosci ustaw predkosc
			if (Received[0] == 'W' && Received[1] == 'Z' && Received[2] == 'Z') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					WZZ = value;
				}
			}


			if (Received[0] == 'D' && Received[1] == 'O' && Received[2] == 'F') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					KRK = value;
					LogFlag=false;

				}
			}
			if (Received[0] == 'D' && Received[1] == 'O' && Received[2] == 'N') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					KRK = value;
					LogFlag=true;

				}
			}


		HAL_UART_Receive_DMA (&huart2, Received, 8);
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	}
}

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
  MX_I2C1_Init();
  MX_TIM15_Init();
  MX_TIM17_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();

  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	//timer do PWM DC
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 5000);
	// timer do przerwań- pomiar predkosci silnika dc
	HAL_TIM_Base_Start_IT(&htim15);
	HAL_TIM_Base_Start_IT(&htim3);
	// timer do enkodera DC
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	HAL_UART_Receive_DMA (&huart2, Received, 8);

	 while(!INA219_Init(&ina219, &hi2c1, INA219_ADDRESS))
	   {
		 HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	   }
//	   INA219_setPowerMode(&ina219, INA219_CONFIG_MODE_ADCOFF);
	   vbus = INA219_ReadBusVoltage(&ina219);
	   vshunt = INA219_ReadShuntVolage(&ina219);
	   current = INA219_ReadCurrent(&ina219);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(Flaga){
		  Flaga2=true;

		  vbus = INA219_ReadBusVoltage(&ina219);
		  current = INA219_ReadCurrent(&ina219);
		  vshunt = INA219_ReadShuntVolage(&ina219);
		  Flaga=false;
		  Flaga2=false;
	  }
	  	  //HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
