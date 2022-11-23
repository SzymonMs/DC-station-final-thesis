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
	volatile int16_t speed=0;
	volatile int16_t speed_1=0; // do rysowania
	volatile int16_t speed_prev;
	float number = 15; // dla czasu 10ms
// UART
	char msg[64];


	uint8_t data[100]; // Tablica przechowujaca wysylana wiadomosc.
	uint16_t size = 0;
	uint8_t Received[10];
	volatile float Kpw = 0.11f;
	volatile float Kiw = 0.25f;
	volatile float Kpi = 0.0f;
	volatile float Kii = 5.0f;

	volatile int WZZ = 2; //zadana predkosc obrotowa
	volatile int KRK = 1; //zadana ilosc krokow
	_Bool LogFlag=0;
	volatile _Bool Flaga = false;
	volatile _Bool Flaga2 = false;


	//pomiar pradu
	INA219_t ina219;
	volatile float vbus, vshunt, current;
	volatile float current1 = 0.0f;
	volatile float current2 = 0.0f;
	volatile float currentObsv;
	volatile float Tf = 0.2f;
	//Wszystkie wartości z czapy
	//Nastawy regulatora prądu

	//Nastawy regulatora prędkości

	//Warunki początkowe, okres, wypełnienie
	float prevIntI = 0.0f;
	float prevErrorI = 0.0f;
	float prevIntW = 0.0f;
	float prevErrorW = 0.0f;
	volatile uint16_t duty; // PWM %
	volatile float setValueW = 1000.0f; // value of omega- setpoint
	volatile float setValueI = 500.0f; //value of I- setpoint
	float Tp = 0.01f; //Sample time
	volatile float Wmess; // zmierzona omega
	volatile float Imess; // zmierzony prąd
	float kFi = 0.127f; // stała elektromotoryczna napędu;
	volatile int REG=0;
	volatile int PWM=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM15) {

		if (HAL_GPIO_ReadPin(A_GPIO_Port, A_Pin)==SET && HAL_GPIO_ReadPin(B_GPIO_Port, B_Pin)==SET) {
			htim17.Instance->CCR1 = 0;
		}
		if (HAL_GPIO_ReadPin(A_GPIO_Port, A_Pin)==RESET && HAL_GPIO_ReadPin(B_GPIO_Port, B_Pin)==RESET) {
			htim17.Instance->CCR1 = 0;
		}


		if (HAL_GPIO_ReadPin(A_GPIO_Port, A_Pin)==SET && HAL_GPIO_ReadPin(B_GPIO_Port, B_Pin)==RESET) {
//			count = __HAL_TIM_GET_COUNTER(&htim4);
//			speed = number * count / 4; //obr min
//
//
//			__HAL_TIM_SET_COUNTER(&htim4, 0);

			if(REG==0)
			{
				htim17.Instance->CCR1 = duty;
			}
			if(REG==1)
			{
				TorqueControl();
			}
			if(REG==2)
			{
				CascadeControl();
			}


		}
		if (HAL_GPIO_ReadPin(A_GPIO_Port, A_Pin)==RESET && HAL_GPIO_ReadPin(B_GPIO_Port, B_Pin)==SET) {
//			count = __HAL_TIM_GET_COUNTER(&htim4);
//			count = 65000 - count;
//			speed = -number * count / 4; //obr min
//
//			__HAL_TIM_SET_COUNTER(&htim4, 0);

			//CascadeControl();
		}
		if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4))
				{
			count = __HAL_TIM_GET_COUNTER(&htim4);
						count = 65000 - count;
						speed = -number * count / 4; //obr min

						__HAL_TIM_SET_COUNTER(&htim4, 0);

				}
		else if(!__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4))
		{
			count = __HAL_TIM_GET_COUNTER(&htim4);
			speed = number * count / 4; //obr min


			__HAL_TIM_SET_COUNTER(&htim4, 0);
		}




		if(speed>5000 || speed<-5000)
		{
			speed_1=0;
		}
		else
			speed_1=speed;


//		count = __HAL_TIM_GET_COUNTER(&htim4);
//		speed = number * count / 4; //obr min
//		__HAL_TIM_SET_COUNTER(&htim4, 0);

		if(LogFlag==1)
					{
							size = sprintf(data, "data %.2f %d %.2f %d \r\n", current, speed_1, vbus/1000.0f, duty/100);
							HAL_UART_Transmit(&huart2, data, size, 1000);
					}
		//uruchomic konwersje na przetworniku , przerwanie od konca konwersji





	//	vbus = INA219_ReadBusVoltage(&ina219);
		//current = INA219_ReadCurrent(&ina219);
	//	vshunt = INA219_ReadShuntVolage(&ina219);
		if(!Flaga2)
		{
			Flaga=true;
		}
		//TorqueControl();
//		CascadeControl();

	}
	if (htim->Instance == TIM2) {
		//HAL_GPIO_TogglePin(TEST_GPIO_Port, TEST_Pin);
		//current = INA219_ReadCurrent(&ina219);
	}

	 if (htim->Instance == TIM3) {
		size = sprintf(data, "parm %.2f %.2f %.2f %.2f %.2f %.2f \r\n", setValueI,setValueW,Kpw,Kiw,Kpi,Kii); // @suppress("Float formatting support")
		HAL_UART_Transmit(&huart2, data, size, 1000);

	}

}

void TorqueControl(){
	float uI;
	Imess = current; //Pomiar prądu
	float errorI = setValueI - Imess; // pytanie jak się ma zmierzony prąd do uW do zbadania
	//P prądu
	float uKpI = Kpi*errorI;
	//I prądu
	float errorSumI = errorI + prevErrorI + prevIntI;
	prevErrorI = errorI;
	float uKiI = errorSumI * Tp/2.0f* Kii;
	prevIntI = errorSumI;
	uI = uKpI + uKiI;
	duty = (uint16_t) uI; // pytanie czy to uI będzie w sensownych wartościach czy np tylko małe
	if( duty > 9999 ){
		duty = 9999;
	}
	htim17.Instance->CCR1 = duty; //Odpowiedni timer od PWM silnika musi mieć takie duty
}
void CascadeControl(){
	Wmess = speed_1*3.14f/30.0f; //Tutaj mierzenie prędkości

	float uW;
	float uI;
	float errorW2 = 0.16f*setValueW*3.14f/30.0f - Wmess;
	float errorW = setValueW*3.14f/30.0f - Wmess;
	//P omega
	float uKpW = Kpw*errorW2;
	//I omega
	float errorSumW = errorW + prevErrorW + prevIntW;
	prevErrorW = errorW;
	float uKiW = errorSumW * Tp/2.0f* Kiw;
	prevIntW = errorSumW;
	uW = (uKpW + uKiW) / kFi; // wartość zadana dla regulatora prądu
	Imess = current; // Tutaj mierzenie prądu
	float errorI = uW - Imess; // pytanie jak się ma zmierzony prąd do uW do zbadania
	//P prądu
	float uKpI = Kpi*errorI;
	//I prądu
	float errorSumI = errorI + prevErrorI + prevIntI;
	prevErrorI = errorI;
	float uKiI = errorSumI * Tp/2.0f* Kii;
	prevIntI = errorSumI;
	uI = uKpI + uKiI;
	duty = (uint16_t) uI; // pytanie czy to uI będzie w sensownych wartościach czy np tylko małe
	if( duty > 9999 ){
		duty = 9999;
	}
	htim17.Instance->CCR1 = duty; //Odpowiedni timer od PWM silnika musi mieć takie duty
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
			if (Received[0] == 'I' && Received[1] == 'Z' && Received[2] == 'Z') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					setValueI = value/10.0f;
				}
			}

			//jeśli komenda dotyczy predkosci ustaw predkosc
			if (Received[0] == 'W' && Received[1] == 'Z' && Received[2] == 'Z') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					setValueW = value;
				}
			}
			if (Received[0] == 'P' && Received[1] == 'W' && Received[2] == 'M') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					duty=value*9999/100;
					if( duty > 9999 ){
							duty = 9999;
						}

				}
			}


			if (Received[0] == 'D' && Received[1] == 'O' && Received[2] == 'F') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {

					LogFlag=false;

				}
			}
			if (Received[0] == 'D' && Received[1] == 'O' && Received[2] == 'N') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {

					LogFlag=true;

				}
			}
			if (Received[0] == 'R' && Received[1] == 'E' && Received[2] == 'G') {
				//wpisanie wartosci do zmiennej

				if (rx_n == 1) {
					if(value==0)
						REG=0;
					else if(value==1)
						REG=1;
					else if(value==2)
						REG=2;


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
  MX_TIM15_Init();
  MX_TIM17_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	//timer do PWM DC
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 9999);
	// timer do przerwań- pomiar predkosci silnika dc
	HAL_TIM_Base_Start_IT(&htim15);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim2);
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
//		  current2 = current1;
//		  current1 = current;
//		  currentObsv = (Tf)/(Tf+Tp)*current2 + (Tp)/(Tf+Tp)*current1;
		  Flaga=false;
		  Flaga2=false;
	  }
	  //current = INA219_ReadCurrent(&ina219);
	 // vbus = INA219_ReadBusVoltage(&ina219);
	  	  HAL_Delay(0);
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
