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
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
 volatile int32_t count;
  volatile int32_t old_count;
  volatile int32_t delta_count;
  volatile float speed;
  volatile float speed_n;
  int time=1; //PRZELICZAĆ
  float number = 15; // dla czasu 10ms
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == htim4.Instance){
    	delta_count = count-old_count;
    	speed = delta_count/time;
    	speed_n = speed*number;
    	old_count = count;

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
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

//zad1
//HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,1000);
//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,500);
//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,500);
//zad2
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//	uint16_t duty = 0;
//zad3
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//zad4
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	// zad5
//	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
//	char msg[64];
//	uint32_t count=0;
HAL_TIM_Encoder_Start(&htim1,TIM_CHANNEL_ALL);
char msg[64];
HAL_TIM_Base_Start_IT(&htim4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		  count=__HAL_TIM_GET_COUNTER(&htim1);
		  HAL_Delay(0);
//			  count=count2/0.001f;
//			  count=count/400.0;
//			  sprintf((char*)msg, "Odczyt %f \n \r",count);
//			  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg),1000);
//			  HAL_Delay(1000);
		//zad2cd
//		if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE)==SET)

//		{
//			__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
//
//			if (duty > 1000) {
//				duty = 0;
//			}
//			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
//			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, duty);
//			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, duty);
//			duty =duty+ 100;
//		}
		//zad3cd
//		uint8_t mess[4];
//		uint16_t duty = 0;
//		HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, &mess, 4, 100);
//		if (status == HAL_OK) {
//
//			if (mess[1] == '1') {
//				duty = 1000;
//			} else if (mess[1] == '0') {
//				uint8_t l1 = mess[2] - 48;
//				uint8_t l2 = mess[3] - 48;
//				duty = l1 * 100 + l2 * 10;
//
//			}
//			if (mess[0] == 'R') {
//				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
//			}
//		}
		//zad4cd
//		uint8_t mess[12];
//				uint16_t dutyR = 0;
//				uint16_t dutyG = 0;
//				uint16_t dutyB = 0;
//				HAL_StatusTypeDef status = HAL_UART_Receive(&huart3, &mess, 12, 100);
//				if (status == HAL_OK) {
//
//					if (mess[1] == '1') {
//						dutyR = 1000;
//					} else if (mess[1] == '0') {
//						uint8_t l1 = mess[2] - 48;
//						uint8_t l2 = mess[3] - 48;
//						dutyR = l1 * 100 + l2 * 10;
//
//					}
//					if (mess[5] == '1') {
//						dutyG = 1000;
//					} else if (mess[5] == '0') {
//						uint8_t l1 = mess[6] - 48;
//						uint8_t l2 = mess[7] - 48;
//						dutyG = l1 * 100 + l2 * 10;
//
//					}
//					if (mess[9] == '1') {
//						dutyR = 1000;
//					} else if (mess[9] == '0') {
//						uint8_t l1 = mess[10] - 48;
//						uint8_t l2 = mess[11] - 48;
//						dutyB = l1 * 100 + l2 * 10;
//
//					}
//
//					if (mess[0] == 'R'&&mess[4] == 'G' &&mess[8] == 'B') {
//						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, dutyR);
//						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, dutyG);
//						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, dutyB);
//					}
//				}
		//zad5cd
//		count = __HAL_TIM_GET_COUNTER(&htim3);
//
//		sprintf((char*) msg, "odczyt= %d\r\n", count);
//		HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg), 1000);
//		HAL_Delay(1000);

	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//}
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
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
