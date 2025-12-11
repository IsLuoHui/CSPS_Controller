/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "oledSPI.h"
#include "oledui.h"
#include <stdio.h>
#include "CSPScom.h"
#include "loopbuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern volatile uint8_t uCRefreshFlag;
extern UART_HandleTypeDef huart1;
/* USER CODE END Variables */
/* Definitions for testTask */
osThreadId_t testTaskHandle;
const osThreadAttr_t testTask_attributes = {
  .name = "testTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for oledRefreshTask */
osThreadId_t oledRefreshTaskHandle;
uint32_t oledRefreshTaskBuffer[ 512 ];
osStaticThreadDef_t oledRefreshTaskControlBlock;
const osThreadAttr_t oledRefreshTask_attributes = {
  .name = "oledRefreshTask",
  .cb_mem = &oledRefreshTaskControlBlock,
  .cb_size = sizeof(oledRefreshTaskControlBlock),
  .stack_mem = &oledRefreshTaskBuffer[0],
  .stack_size = sizeof(oledRefreshTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTestTask(void *argument);
void StartOLEDRefreshTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of testTask */
  testTaskHandle = osThreadNew(StartTestTask, NULL, &testTask_attributes);

  /* creation of oledRefreshTask */
  oledRefreshTaskHandle = osThreadNew(StartOLEDRefreshTask, NULL, &oledRefreshTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTestTask */
/**
  * @brief  Function implementing the testTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTestTask */
void StartTestTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTestTask */
  UNUSED(argument);
  uint8_t command[50];
  int commandLength = 0;
  uint8_t PowOn[]=  {0xAA,0x05,0x01,0x00,0xB0};
  uint8_t PowOff[]= {0xAA,0x05,0x01,0x01,0xB1};
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    osDelay(100);

    commandLength = Command_GetCommand(command);
    if (commandLength != 0){
      // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)command, commandLength);
      for (int i=0;i<5;i++)
      {
        if (PowOn[i] != command[i])break;
        if (i==4)CSPS_Set_Power(1);
      }
      for (int i=0;i<5;i++)
      {
        if (PowOff[i] != command[i])break;
        if (i==4)CSPS_Set_Power(0);
      }
       //for (int i = 2; i < commandLength - 1; i ++){
      //   GPIO_PinState state = GPIO_PIN_SET;
      //   if (command[i + 1] == 0x00){
      //     state = GPIO_PIN_RESET;
      //   }
      //   if (command[i] == 0x01){
      //     HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, state);
      //   }else if (command[i] == 0x02){
      //     HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, state);
      //   }else if (command[i] == 0x03){
      //     HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, state);
      //   }
       //}
    }

  }
  /* USER CODE END StartTestTask */
}

/* USER CODE BEGIN Header_StartOLEDRefreshTask */
/**
* @brief Function implementing the oledRefreshTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOLEDRefreshTask */
void StartOLEDRefreshTask(void *argument)
{
  /* USER CODE BEGIN StartOLEDRefreshTask */
  UNUSED(argument);
  OLED_Init();
  EaseVar_Init();
  OLEDUI_Init();

  /* Infinite loop */
  for(;;)
  {
    if (uCRefreshFlag==1) {
      CSPS_Var_Refresh();
      uCRefreshFlag=0;
      //UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
      //printf("stack last: %lu words\r\n", freeStack);
    }
    OLEDUI_Refresh();
    //OLED_Draw_Rect(anim1.currentValue,0,anim1.currentValue+ICON48W,ICON48H,OLED_MIX_COVER);

    /*
    for (uint8_t i = 0;i < menu.optnum;i++)
    {
      OLED_Draw_Element(menu.opt[i].ele);
      OLED_Draw_Text(menu.opt[i].text);
      for (uint8_t j = 0;j < menu.opt[i].listnum;j++)
        OLED_Draw_Text(menu.opt[i].list[j].text);
    }
    OLED_Draw_FillRect(cursor.x0, cursor.y0, cursor.x1, cursor.y1, OLED_MIX_XOR);
    OLED_Draw_Point(cursor.x0, cursor.y0, OLED_MIX_XOR);
    OLED_Draw_Point(cursor.x0, cursor.y1 - 1, OLED_MIX_XOR);
    OLED_Draw_Point(cursor.x1 - 1, cursor.y0, OLED_MIX_XOR);
    OLED_Draw_Point(cursor.x1 - 1, cursor.y1 - 1, OLED_MIX_XOR);

    //滚动条背景
    OLED_Draw_Rect(121 + scrollbarOffset, 0, OLED_WIDTH_PIXEL - 1 + scrollbarOffset, OLED_HEIGHT_PIXEL - 1, OLED_MIX_COVER);
    //滚动条滑块
    OLED_Draw_FillRect(122 + scrollbarOffset, 0, OLED_WIDTH_PIXEL - 1 + scrollbarOffset, scrollOffset, OLED_MIX_COVER);
    */


    //osDelay(5);
  }
  /* USER CODE END StartOLEDRefreshTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

