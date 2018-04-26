/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN 0 */
volatile int XferCpltFlag = 1;
// Obviously these are button debounce variables. Pretty self explanatory
volatile uint8_t pausePrev= 0;
volatile uint8_t pauseCurr = 0;
volatile uint8_t pauseFlag = 1;
volatile uint8_t buttonPress = 0;

volatile int ffPrev;
volatile int ffCurr;
volatile int ffFlag = 0;
//
volatile int rewindPrev;
volatile int rewindCurr;
volatile int rewindFlag =0;
//
volatile int nextPrev;
volatile int nextCurr;
volatile int nextFlag;

volatile int prevPrev;
volatile int prevCurr;
volatile int prevFlag = 0;


/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_dac1_ch1;
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 channel 2 and 3 interrupts.
 */
void DMA1_Channel2_3_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
	if(__HAL_DMA_GET_FLAG(hdma_dac1_ch1, DMA_FLAG_TC3)  > 0)
	{
		XferCpltFlag = 1;
		HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
		HAL_DMA_Abort_IT(&hdma_dac1_ch1);
		// completeFLAG = 1;

	}
	HAL_GPIO_WritePin(LD4_GPIO_Port,LD4_Pin,GPIO_PIN_SET);
	/* USER CODE END DMA1_Channel2_3_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_dac1_ch1);
	/* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */
	// __HAL_DMA_GET_COUNTER(hdma_dac1_ch1);

	/* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
 * @brief This function handles TIM1 break, update, trigger and commutation interrupts.
 */
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	/* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 0 */
	pausePrev = pauseCurr;
	//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	pauseCurr = HAL_GPIO_ReadPin(PLAY_GPIO_Port, PLAY_Pin) == 0? 1 : 0;
	if(pausePrev == 0 && pauseCurr == 1){
		buttonPress = 1;
		pauseFlag = !pauseFlag;
		//pauseState = pauseCurr;

	}

	ffPrev = ffCurr;
	ffCurr = HAL_GPIO_ReadPin(FF_GPIO_Port, FF_Pin) == 0 ? 1: 0;
	if(ffPrev == 0 && ffCurr == 1){
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

		ffFlag = 1;
		buttonPress = 1;
	}

	rewindPrev = rewindCurr;
	rewindCurr = HAL_GPIO_ReadPin(REWIND_GPIO_Port, REWIND_Pin) == 0? 1 : 0;
	if(rewindPrev == 0 && rewindCurr == 1) {

		rewindFlag = 1;
		buttonPress = 1;
	}
	//
	nextPrev = nextCurr;
	nextCurr = HAL_GPIO_ReadPin(NEXT_GPIO_Port, NEXT_Pin) == 0? 1 : 0;
	if(nextPrev == 0 && nextCurr == 1) {
		nextFlag = 1;
		buttonPress = 1;
	}

	prevPrev = prevCurr;
	prevCurr = HAL_GPIO_ReadPin(PREVIOUS_GPIO_Port, PREVIOUS_Pin) == 0 ? 1 : 0;
	if(prevPrev == 0 && prevCurr == 1) {
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		prevFlag = 1;
		buttonPress = 1;
	}

	/* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 0 */
	HAL_TIM_IRQHandler(&htim1);
	/* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 1 */

	/* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 1 */
}

/**
 * @brief This function handles TIM6 global and DAC underrun error interrupts.
 */
void TIM6_DAC_IRQHandler(void)
{
	/* USER CODE BEGIN TIM6_DAC_IRQn 0 */

	/* USER CODE END TIM6_DAC_IRQn 0 */
	HAL_TIM_IRQHandler(&htim6);
	HAL_DAC_IRQHandler(&hdac1);
	/* USER CODE BEGIN TIM6_DAC_IRQn 1 */

	/* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
 */
void USART1_IRQHandler(void)
{
	/* USER CODE BEGIN USART1_IRQn 0 */

	/* USER CODE END USART1_IRQn 0 */
	HAL_UART_IRQHandler(&huart1);
	/* USER CODE BEGIN USART1_IRQn 1 */

	/* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
