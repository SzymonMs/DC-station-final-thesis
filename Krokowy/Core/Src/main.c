/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "adc.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stepper.h"
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
// ADC
	const uint32_t ADC_REG_MAX=0xfff; //12bit
	const float ADC_VOLTAGE_MAX = 3.3; //[V]
//	const uint32_t ADC_TIMEOUT= 100; //[ms]
	uint32_t ADC_measurment=0;
	float ADC_voltage = 0;
	uint32_t ADC_voltage_mV=0;
	int pwm=0;
// SILNIK KROKOWY
	struct stepper_s stepper = { 0 };
	volatile int32_t count_stepper;
// SILNIK DC
	volatile int32_t count;
	volatile int16_t speed;
	volatile int16_t speed_prev;
	float number = 15; // dla czasu 10ms
// UART
	char msg[64];
	volatile int sign = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == htim15.Instance) {
		if (sign == 1) {
			count = __HAL_TIM_GET_COUNTER(&htim4);
			speed = number * count / 4; //obr min
			__HAL_TIM_SET_COUNTER(&htim4, 0);
		}
		if (sign == -1) {
			count = __HAL_TIM_GET_COUNTER(&htim4);
			count = 65000 - count;
			speed = -number * count / 4; //obr min
			__HAL_TIM_SET_COUNTER(&htim4, 0);
		}
	}
}
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  MX_TIM15_Init();
  MX_TIM20_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_LPUART1_UART_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
	// HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	// HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
#if STEPPER_ANGLE_MODE == STEPPER_ANGLE_MODE_SLAVE_TIM
  stepper_init(&stepper, &htim2, TIM_CHANNEL_3, &htim1);

#elif STEPPER_ANGLE_MODE == STEPPER_ANGLE_MODE_MANUAL
	stepper_init(&stepper, &htim2, TIM_CHANNEL_3);
#endif
	direction dir = CW;
// stepper_set_angle(&stepper, dir, 20,360);
//stepper_set_continous(&stepper, dir, 10);

//----------------------------------------SILNIK DC------------------------------------------

	//timer do PWM DC
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	//	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 500);
	// timer do przerwań- pomiar predkosci silnika dc
	HAL_TIM_Base_Start_IT(&htim15);

	// timer do enkodera DC
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

	// timer enkodera do krokowego
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		// USTAWIANIE PWM PRZEZ ADC
			  HAL_ADC_Start(&hadc3);
			  HAL_ADC_PollForConversion(&hadc3, 100);
			  ADC_measurment=HAL_ADC_GetValue(&hadc3);
			  ADC_voltage = ((float)ADC_measurment/(float)ADC_REG_MAX)*ADC_VOLTAGE_MAX;
			  ADC_voltage_mV = (uint32_t)(1000.0*ADC_voltage);
			  pwm=map(ADC_voltage_mV,100,1550,0,100);
			  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, pwm*10);
			  count_stepper = __HAL_TIM_GET_COUNTER(&htim3);

			  HAL_Delay(1000);

			  // KROKOWY NA PRZYCISK

			  if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)) {
				  __HAL_TIM_SET_COUNTER(&htim3, 0);
				  stepper_set_angle(&stepper, dir, 10, 200); // ilosć krokow jako 4 argument
			  }

			  if (HAL_GPIO_ReadPin(SWITCH1_GPIO_Port, SWITCH1_Pin)) {
				  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
				  sign = 1;
			  }
			  else {
				  sign = -1;
				  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
			  }

			  //UART WYSY�?ANIE NA TERMINAL
			  //				sprintf((char*) msg, "%d ", count);
			  //				HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 1000);
			  //				sprintf((char*) msg, "%d \r\n", speed);
			  //				HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 1000);

			  // UART TEST
			  sprintf((char*) msg, "%d \r\n", 12);
			  HAL_UART_Transmit(&huart1, (uint8_t*) msg, 10, 1000);
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_ADC345;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.Adc345ClockSelection = RCC_ADC345CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#if STEPPER_ANGLE_MODE == STEPPER_ANGLE_MODE_SLAVE_TIM
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == stepper.slave_timer.htim->Instance)
	{
		stepper_stop(&stepper);
	}
}
#elif STEPPER_ANGLE_MODE == STEPPER_ANGLE_MODE_MANUAL
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == stepper.timer.htim->Instance) {
		stepper.step_counter++;

		if (stepper.step_counter >= stepper.steps_to_count) {
			stepper_stop(&stepper);
		}
	}
}
#endif
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
	while (1) {
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
