/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "port.h"
#include "wave.h"
#include "parser.h"
#include "LCD.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern uint8_t pauseFlag;
extern uint8_t buttonPress;
extern uint8_t finishReading;
extern int ffFlag;
extern int rewindFlag;
extern int nextFlag;
extern int prevFlag;
extern int _rcplt ;
extern int rotFlag;
extern int ccwFlag;
extern int cwFlag;
extern uint8_t RotaryTransition;

char files[FILE_LIST][FILE_NAME_SIZE] = {"FeelStill.wav"
		, "Happy.wav"
		, "temp.wav"
		, "Brain.wav"
		, "CestLaVie.wav"
		, "Aicha.wav"
		, "NancyAjram.wav"
		, "sharmoofers.wav"
		, "AmrDiab.wav"
		, "hall.wav"
		, "FellingGood.wav"
		, "AngelEyes.wav"
		, "heSoFine.wav"
		, "Logic.wav"
		, "LoveLies.wav"
		, "MissYou.wav"
		, "YaBanat.wav"} ;
extern FATFS FatFs;
uint8_t fileselect = 0;
int done = 0;




ApplicationState _appState = Idle;
PlayerState _state = Start;
PlayerState _prevState = Start;;
extern uint8_t buttonPress;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void updateState();
void MainApp();
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

int getState()
{
	return _state;
}


void updateState()
{
	if(_appState == Begin)
	{
		_appState = Running;
	}

	if(_state == Start)
	{
		_state = PlayState;
		return;
	}

	if(_state == PlayState)
	{
		if(finishReading == 1)
		{
			_state = Start;
			finishReading = 0;
			fileselect ++;
			return;
		}

		if(rotFlag == 1)
		{
			rotFlag = 0;
			_state = Increase;
			return;

		}


		if(ffFlag == 1)
		{
			ffFlag = 0;
			_state = FastForwardState;
			return;
		}

		if(prevFlag == 1)
		{
			prevFlag = 0;
			_state = PreviousState;
			return;
		}

		if(nextFlag == 1)
		{
			nextFlag = 0;
			_state = NextState;
			return;
		}

		if(rewindFlag == 1)
		{
			rewindFlag = 0;
			_state = RewindState;
			return;
		}
		switch(pauseFlag)
		{
		case 1:
			_state = PauseState;
			return;
			break;
		case 0:
			_state = PlayState;
			return;
			break;
		}


	}


	if(_state == FastForwardState || _state == NextState || _state == PreviousState || _state == RewindState || _state == Increase )
	{
		_state = PlayState;
		return;
	}


	if(_state == PauseState)
	{
		switch(pauseFlag)
		{
		case 1:
			_state = PauseState;
			return;
			break;
		case 0:
			_state = PlayState;
			return;
			break;
		}
		return;

	}


}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_DAC1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();
  MX_SPI2_Init();

  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start_IT(&htim1);
	//HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	if(HAL_UART_Receive_IT(&huart1, (uint8_t *)Rbuffer, 1) != HAL_OK)
	{
		Error_Handler();
	}

	//transmit("Hello Bitch\n");
	HAL_GPIO_WritePin(SS2_GPIO_Port, SS1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SS1_GPIO_Port, SS2_Pin, GPIO_PIN_RESET);
//	LCD_turnoff();
//	LCD_turnOn();
	LCD_init();
	LCD_BlinkCursor();
	LCD_Clear();
	//LCD_transmitString("Hello");
	RotaryTransition = HAL_GPIO_ReadPin(ROT1_GPIO_Port, ROT1_Pin) << 3 | (HAL_GPIO_ReadPin(ROT2_GPIO_Port, ROT2_Pin) << 2) |
    HAL_GPIO_ReadPin(ROT1_GPIO_Port, ROT1_Pin) << 1 | (HAL_GPIO_ReadPin(ROT2_GPIO_Port, ROT2_Pin));


	//		LCD_TOP_transmitString("Hello\n");
	//
	//


	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

	//TIM6->PSC = 999;
	//MX_TIM_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		//LCD_transmitString("Hello Motha FUCKAA!!!!!");
		Mount();
		//ParseDirectory();

		switch(_appState)
		{
		case Begin:
			MainApp();
			break;
		case Idle:
			break;
		case Running:
			break;
		}
	}
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void Mount()
{
	FRESULT fr;

	fr = f_mount(&FatFs, USERPath, 1);
	if(fr != FR_OK)
	{
		//Error_Handler();
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		return;

	}

	_appState = Start;
}

void MainApp()
{


	while(1)
	{


		switch(_state)
		{
		case Start:
			ParseFile(files[fileselect]);
			//_prevState = Start;
			updateState();
			LCD_init();
			LCD_Clear();
			LCD_transmitString(files[fileselect]);
			//LCD_Clear();
			if(_prevState == Start)
			{
				while(isButtonPress() == FALSE && isReceive() == FALSE);
				buttonPress = 0;
				_rcplt = 0;
			}
			_prevState = Start;

		case PlayState:
			Play();
			_prevState = PlayState;
			break;
		case PauseState:
			while(isButtonPress() == FALSE && isReceive() == FALSE);
			buttonPress = 0;
			_rcplt = 0;
			break;

		case PreviousState:
			CleanUp();
			fileselect = fileselect == 0? 0 : (fileselect-1);
			ParseFile(files[fileselect]);
			LCD_init();
			LCD_Clear();
			LCD_transmitString(files[fileselect]);
			break;

		case NextState:
			CleanUp();
			fileselect = fileselect == FILE_LIST ? 0: fileselect + 1 ;
			ParseFile(files[fileselect]);
			LCD_init();
			LCD_Clear();
			LCD_BlinkCursor();
			LCD_transmitString(files[fileselect]);
			break;

		case FastForwardState:
			done = 0;
			int counter = 0;
			while(stillff() == TRUE || isButtonPress() == TRUE)
			{
				TIM6->PSC = 999;
				Play();
				done = 1;
			}
			if(isReceive() == TRUE && done == 0)
			{
				_rcplt = 0;
				FastForward(10000);
				done = 1;
			}
			counter = 0;
			TIM6->PSC = 2999;
			break;
		case RewindState:
			done = 0;
			while(stillR() == TRUE || isButtonPress() == TRUE)
			{
				TIM6->PSC = 999;
				Play();
				done = 1;
			}
			if(isReceive() == TRUE && done == 0)
			{
				Rewind(10000);
				_rcplt = 0;
				done = 1;
			}

			TIM6->PSC = 2999;
			break;

		case Increase:
			while(isButtonPress() == FALSE)
			{
				Play();

				if(cwFlag == 1)
				{
					cwFlag = 0;
					TIM6->PSC -= 20;
				}
				else if(ccwFlag == 1)
				{
					ccwFlag = 0;
					TIM6 -> PSC += 20;
				}

			}
			buttonPress = 0;
			rotFlag = 0;
			TIM6 -> PSC = 2999;
			break;


		}
		updateState();

	}

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	_appState = Idle;
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
