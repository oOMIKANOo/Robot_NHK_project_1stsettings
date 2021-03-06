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
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"stm32f4_printf.h"
#include"ps3.h"
#include "canmd_controller.h"
#include"stm32f4_easy_can.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
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
//グロバル変数
 static uint8_t uart6_buf[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void canmd_status_set_data(int canmd_num,int motor_select,unsigned int DutyOrPid,int p_value,int i_value,int d_value);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int i=0; int x=0;
int id= 1;
int stop =0;
int mode =0;
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
  MX_CAN1_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  stm32f4_printf_init(&huart3);
  HAL_UART_Receive_IT(&huart6,uart6_buf,8);
  canmd_controller_init();
  ps3_init();
  stm32f4_easy_can_init(&hcan1,0,0b11111);
  canmd_status_set_data(1,0,0,50,50,50);
  canmd_status_set_data(1,1,0,50,50,50);
  canmd_status_set_data(2,0,0,50,50,50);
  canmd_status_set_data(2,1,0,50,50,50);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		Ps3 ps3_data;
		ps3_get_data(&ps3_data);

		//GPIO出力制御
		if(ps3_data.up==1){
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET);
		}
		if(ps3_data.down==1){
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
		}
		if(ps3_data.shikaku==1){
			HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_11);
		}

		//コントローラ値確認
		stm32f4_printf("up=%d\n\r",ps3_data.up);
		stm32f4_printf("down=%d\n\r",ps3_data.down);

		int encoder[5][2];

		for (int md_id_i = 1; md_id_i <= 4; md_id_i++) {
		 canmd_controller_get_encoder_count(md_id_i, &encoder[md_id_i][0], &encoder[md_id_i][1]);
		}
		HAL_Delay(100);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
}

/* USER CODE BEGIN 4 */


//UART受信割り込み関数

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
if(huart->Instance == huart6.Instance) {
 ps3_uart_interrupt_routine(uart6_buf,8);
 HAL_UART_Receive_IT(&huart6,uart6_buf,8);
 }
}

void canmd_status_set_data(int canmd_num,int motor_select,unsigned int DutyOrPid,int p_value,int i_value,int d_value)
{
 int id;
 int dlc = 4;
unsigned char data[8];

 id = canmd_num;

 data[0] = ((motor_select << 6) & 0b11000000 ) | DutyOrPid;
data[1] = p_value & 0XFF;
 data[2] = i_value & 0XFF;
 data[3] = d_value & 0XFF;


 HAL_Delay(100);

 stm32f4_easy_can_transmit_message(id, dlc, data);
}

void stm32f4_easy_can_interrupt_handler(void)
{
 int id;
int dlc;
unsigned char data[8];

 stm32f4_easy_can_get_receive_message(&id, &dlc, data);

canmd_controller_set_can_receive_data(id, dlc,data);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
if(htim->Instance == TIM6){

 int motor_output[CANMD_MD_NUM+1][2]={};
 int id =1;

 Ps3 ps3_data;
 ps3_get_data(&ps3_data);

 int motor_output01=0;

 if(ps3_data.sankaku==1){
	 motor_output01=500;
 }
 if(ps3_data.batsu==1){
	 motor_output01=-500;
 }

 motor_output[1][0]=motor_output01;


 for(id = 1; id <(CANMD_MD_NUM + 1);id++){

 canmd_controller_set_motor_control_data(id,motor_output[id][0],motor_output[id][1]);
 }
 }
 if(htim->Instance == TIM7){
 int id = 1;
 int dlc =3;
 unsigned char data[8];

 canmd_controller_get_can_transmit_data(&id, &dlc, data);
 stm32f4_easy_can_transmit_message(id,dlc,data);
 }
}
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
