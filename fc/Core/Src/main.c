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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "barometer.h"
#include "cam.h"
#include "mavlink.h"
#include "mpu.h"
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
I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum DroneMode{
	DISARMED,
	ARMED
};

int main(void)
{
	//initialize modules
	HAL_Init();
	MX_USART1_UART_Init();
	MX_TIM4_Init();
	MX_I2C2_Init();
	MX_GPIO_Init();
	//MX_RTC_Init();
	Mpu_Wake();
	HAL_Delay(20);
	Mpu_Calibrate();//sets initial acc values as calibration
	//initialize variables
	//uint8_t baro_test = 9;
	//uint8_t baro_ready_status;
	//uint8_t baro_prom_rx_buffer[2];
	//MAVlink stuff
	//mavlink_system_t mavlink_system = {
	//    1, // System ID2 (1-255)
	//    1  // Component ID (a MAV_COMPONENT value)
	//};
	//Motor_Arm();
	//radio - incoming data will be packeted into four sections: [m1][m2][m3][m4]

	uint8_t payload_test[10] = {1,2,3,4,5,6,7,8,9,10};
	//uint8_t payload_test[10];
	for (uint8_t i=0; i<10;i++)
	{
		payload_test[i] = i+1;
	}
	//uint8_t test[5] = {0x55, 0x55, 0x55, 0x55, 0x55};
	//uint8_t baro_flag = 5;
	cam_photo_rx_buffer[10] = 0x44;
	cam_photo_rx_buffer[11] = 0x55;
	cam_photo_rx_buffer[12] = 0x66;
	//HAL_Delay(3000);//wait for ESC's to arm, old
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
	//uint8_t timChannels[] = {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};
	while(1){
		//HAL_UART_Transmit(&huart1,(uint8_t*) &rx_buffer, sizeof(rx_buffer),HAL_MAX_DELAY);
		//HAL_Delay(1);
		//Radio_Transmit_Raw((uint8_t*) &test, 5);
		//HAL_UART_Receive (&huart1,(uint8_t*) &rx_buffer, 4 ,100);
		//Radio_Recieve_Raw((uint8_t*) &rx_buffer, 4);
		//MAV_Send_Debug_Statement_Default();
		//Motor_Set_Speed_All(0,0,0,0);
		//MAV_Parse_Data();
		//**************start of control test code*************
		MAV_Parse_Data();
		Control_Tick();
		HAL_Delay(20);
		MAV_Send_Raw_Imu();
		//****start of MPU test code****
		//uint8_t mpu_status = Mpu_Is_Ready();
		//Radio_Transmit_Raw(&mpu_status, 1);
		/*
		if(Mpu_Is_Ready() == HAL_OK)
		{
			Mpu_Get_Acc_Data(MPU_AXIS_X);
			HAL_Delay(5);
			Mpu_Get_Acc_Data(MPU_AXIS_Y);
			HAL_Delay(5);
			Mpu_Get_Acc_Data(MPU_AXIS_Z);
			HAL_Delay(5);
			Mpu_Get_Gyro_Data(MPU_AXIS_X);
			HAL_Delay(5);
			Mpu_Get_Gyro_Data(MPU_AXIS_Y);
			HAL_Delay(5);
			//Mpu_Get_Gyro_Data(MPU_AXIS_Z);
			//uint8_t buff[2];
			//Helper_Int16arr_To_Int8arr(&mpu_acc, 1, &buff);
			//Radio_Transmit_Raw(&buff, 2);
			//HAL_Delay(200);
			MAV_Send_Raw_Imu();
		}
		HAL_Delay(50);
		*/
		//****start of FTP test code****
		//cam FTP test
		//cam_photo_rx_buffer[100] = 0x44;
		//cam_photo_rx_buffer[101] = 0x55;
		//cam_photo_rx_buffer[102] = 0x66;
		//Cam_Transmit_Photo(100, 3);
		//MAV FTP test
		//MAV_send_File_Transfer_Protocol(&payload_test, 10);
		//HAL_Delay(500);
		//Radio_Transmit_Raw(&payload_test, 10);
		//HAL_Delay(1000);
		//**************end of FTP test code*************

		//****start of cam test code****
		/*
		//--code part 1--

		Cam_Transmit_Photo(10, 3);
		//HAL_Delay(1000);
		//uint8_t test4 = 0xFD;
		//Radio_Transmit_Raw(&test4, 1);
		MAV_Send_Debug_Statement_Default();
		HAL_Delay(1000);
		*/
		//MAV_Send_Debug_Statement_Default();
		//--code part 2--
		/*
		//Cam_Set_I2C(1);
		HAL_Delay(200);
		Cam_Transmit_Alert();
		uint8_t cam_status = Cam_Is_Ready();
		Radio_Transmit_Raw(&cam_status, 1);
		HAL_Delay(50);
		if(Cam_Is_Ready() == HAL_OK)
		{
			//Cam_Poll_Alert();
			Cam_Poll_Image(10, 5);
		}
		//Radio_Transmit_Raw(&cam_alert_rx_buffer, 1);
		Cam_Transmit_Photo_Debug(10, 5);
		//Cam_Transmit_Alert();
		HAL_Delay(1000);
		//Cam_Set_I2C(0);
		HAL_Delay(1000);
		*/
		//-- code part 3 --
		/*
		Cam_Handle();
		HAL_Delay(100);
		*/
		//-- code part 4 --
		/*
		cam_photo_rx_buffer[100] = 0x44;
		cam_photo_rx_buffer[101] = 0x55;
		cam_photo_rx_buffer[102] = 0x66;
		Cam_Transmit_Photo(100, 3);
		HAL_Delay(1000);
		*/
		//**************end of cam test code*************

		//*******start of barometer code*******
/*
		uint8_t mode[4] = {0x44, 0x00, 0x44};
		if(baro_flag == 0)
		{
			mode[1] = 0x00;
			Baro_Is_Ready();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&baro_ready_status, 1);

			Baro_Request_Press(BARO_OSR_4096);
			baro_flag = 1;
			HAL_Delay(10);//currently, this is needed to get proper values back
		}
		else if (baro_flag == 1 && ( Baro_Is_Ready()== HAL_OK ))
		{
			mode[1] = 0x11;
			Baro_Get_Press();
			//Radio_Transmit_Raw(&baro_temp_rx_buffer, 3);
			//Radio_Transmit_Raw(&mode, 4);
			Baro_Request_Temp(BARO_OSR_4096);
			baro_flag = 2;
			HAL_Delay(10);
		}
		else if (baro_flag == 2 && ( Baro_Is_Ready()== HAL_OK ))
		{
			//char newline[2] = {'\n', '\n'};
			mode[1] = 0x22;
			Baro_Get_Temp();
			Baro_Calculate_Altitude();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&newline, 2);
			uint8_t pressArr[4];
			Helper_Int32_To_Int8arr(pressure, &pressArr);
			Radio_Transmit_Raw(&pressArr, 4);
			HAL_Delay(100); //DO NOT Delay when real time flight controls are required
			baro_flag = 0;
		}
		else if(baro_flag == 5 && ( Baro_Is_Ready() == HAL_OK))
		{//startup - get coefficients and send them out
			mode[1] = 0x55;
			Baro_Get_Coefficients();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&rx_buffer, 4);
			//Radio_Transmit_Raw(&baro_coeffs, 2*6);
			baro_flag = 0;
			HAL_Delay(1000);
		}
*/
//*******end of barometer code*******


	}//end while

}//end main
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 10;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 16000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 800;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600*2;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  //huart1.Init.BaudRate = huart1.Init.BaudRate*2;//must double due to oversampling (dont do here, do above)
  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
