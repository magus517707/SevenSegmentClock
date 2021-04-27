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
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "stm32g4xx_it.h"
#include "utilities.h"
#include "pca9685.h"
#include "ws2812b.h"
#include "clock.h"
#include "seven_segment.h"
#include "uart_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PCA9685_SERVO_MODE
//#define SERIAL_TIME
#ifdef SERIAL_TIME
#define DEL 500
#else
#define DEL 100
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
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
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_RTC_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  //Setup DMA UART
  //Note to self: Find out why the DMA UART TX would get stuck in a TX loop.
  //For the time being just use the normal UART transmit
  RX_Init();
  init_handler();


  HAL_TIM_Base_Start(&htim1); //Used for uS delay

  // Initialize the RTC
  sgTime clock;

  //set initial clock state
  clock_state = 1;

  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2){
      //   Set the time
  	init_clock(&clock);
  }

  uint8_t current_sum = 0; //Used for determining a change in time
  uint8_t last_sum = 0; //Used for determining a change in time

  // Setup the PCA9685 devices for controlling the servos
  I2C_HandleTypeDef *pca_hours;
  I2C_HandleTypeDef *pca_minutes;
  pca_hours = &hi2c1;
  pca_minutes = &hi2c2;
  PCA9685_Init(pca_hours);
  PCA9685_Init(pca_minutes);

  //Setup all the LEDs structures
  LightStrip LS_HrTen; 	//Tens place for hours
  LS_init(&LS_HrTen, LED_COUNT, &hdma_tim3_ch1, &htim3, TIM_CHANNEL_1);
  LightStrip LS_HrOne;	//Ones place for hours
  LS_init(&LS_HrOne, LED_COUNT, &hdma_tim3_ch2, &htim3, TIM_CHANNEL_2);
  LightStrip LS_MnTen;	//Tens place for minutes
  LS_init(&LS_MnTen, LED_COUNT, &hdma_tim3_ch3, &htim3, TIM_CHANNEL_3);
  LightStrip LS_MnOne;	//Ones place for minutes
  LS_init(&LS_MnOne, LED_COUNT, &hdma_tim4_ch2, &htim4, TIM_CHANNEL_2);
  LightStrip TimSep;	//Time Separator
  LS_init(&TimSep  , 20	      , &hdma_tim4_ch1, &htim4, TIM_CHANNEL_1);

  //Setup the clock Digits
#define HOME 90

  //All 4 digits are setup in the Seven_Segment.h file. You should look into creating an Init header.

  uint8_t arr_HrTen[7] = {0,1,2,3,4,5,6}; 					// Pinout values for servos on pca_hours
  init_Digit(&D_HrTen, &LS_HrTen,arr_HrTen,pca_hours,H10); 	// Initialize the HrTen Digit
  all_Digit_servo(&D_HrTen, HOME); 							// Set Digit servos to Home position

  uint8_t arr_HrOne[7] = {8,9,10,11,12,13,14};				// Pinout values for servos on pca_hours
  init_Digit(&D_HrOne, &LS_HrOne,arr_HrOne,pca_hours,H1);	// Initialize the HrOne Digit
  all_Digit_servo(&D_HrOne, HOME);							// Set Digit servos to Home position

  uint8_t arr_MnTen[7] = {0,1,2,3,4,5,6};					// Pinout values for servos on pca_minutes
  init_Digit(&D_MnTen, &LS_MnTen,arr_MnTen,pca_minutes,M10);// Initialize the MnTen Digit
  all_Digit_servo(&D_MnTen, HOME);							// Set Digit servos to Home position

  uint8_t arr_MnOne[7] = {8,9,10,11,12,13,14};				// Pinout values for servos on pca_minutes
  init_Digit(&D_MnOne, &LS_MnOne,arr_MnOne,pca_minutes,M1);	// Initialize the MnOne Digit
  all_Digit_servo(&D_MnOne, HOME);							// Set Digit servos to Home position

  // Variables for color sweep on separator
  uint8_t angle = 0;
  uint8_t angle_difference =15;
  uint8_t i = 20;

  // Set Clock Digits to current time
  update_Digits(&D_HrTen, &D_HrOne, &D_MnTen, &D_MnOne, &clock);
  render_Digits(&D_HrTen, &D_HrOne, &D_MnTen, &D_MnOne);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1){
	 // If clock state is active clock update is available
	 if(clock_state == 1){
		 get_time(&clock); // Refresh current time
		 current_sum = clock.gTime.Hours+clock.gTime.Minutes;// Sum Hours and Minutes
		 if(current_sum != last_sum){
			  // Set Clock Digits to current time
			  update_Digits(&D_HrTen, &D_HrOne, &D_MnTen, &D_MnOne, &clock);
			  render_Digits(&D_HrTen, &D_HrOne, &D_MnTen, &D_MnOne);
		 }
		 render_sep(&TimSep, angle + (i * angle_difference), 255, 60);// Update separator color
		 i++;
		 change_time(&clock, &cv);// Check clock vars and change if necessary
		 last_sum = current_sum;
#ifdef SERIAL_TIME
	 serialTime(&clock); // Output time over Serial
#endif

#ifdef SERIAL_DATE
	  serialDate(&clock); // Output date over Serial
#endif
	 }
	 HAL_Delay(DEL);

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
  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
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
