/**
  ******************************************************************************
  * @file    main.c
  * @author  MMY Application Team
  * @version $Revision: 1633 $
  * @date    $Date: 2016-02-10 16:31:14 +0100 (Wed, 10 Feb 2016) $
  * @brief   This file describe the main program.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup X_NUCLEO_NFC02A1_Applications
  * @{
  */

/** @addtogroup WriteURI_Application
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern void SystemClock_Config( void );
TIM_HandleTypeDef    TimHandle;
volatile static bool timeout;

/* Private functions ---------------------------------------------------------*/
void initGPIO();

int fadespeed = 3;  // Speed for fadein/fadeout effect

#define NUM_MODES 6 //number of blink modes

#define LED1 1		
#define LED2 2
#define LED3 4
#define LED4 8
#define LED5 16
#define LED6 32
#define LED7 64
#define LED8 128
#define LED9 256
#define LED10 512
#define LED11 1024
#define LED12 2048

//array which maps LEDs to pins
uint16_t GPIO2LEDMAP[] =
{
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_1,
    GPIO_PIN_7,
    GPIO_PIN_6,
    GPIO_PIN_5,
    GPIO_PIN_4,
    GPIO_PIN_3,
    GPIO_PIN_2,
    GPIO_PIN_1
};

//array which holds LED GPIO port
GPIO_TypeDef * PORT2LEDMAP[] =
{
    GPIOC,
    GPIOC,
    GPIOA,
    GPIOA,
    GPIOB,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA
};

//function prototypes
void setLEDs(uint32_t LED, GPIO_PinState state);
void fadein(int pin);
void fadeout(int pin);
void fadeinALL();
void fadeoutALL();
uint8_t getSetMode();
void initGPIO();


int main( void )
{
  /* STM32L0xx HAL library initialization */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  //initialize rng
  srand(42);
  //initialize gpio
  initGPIO();

  //get the last selected mode
  uint8_t mode = getSetMode();
  while( 1 )
  {
      switch (mode)
      {
        case 0:
        {
			//mode 0: fadein one additional LED per second. fade out LEDs one after the other per second after that.
            for (uint8_t i=0; i<255; i++)
            {
                fadespeed = 3;
                uint16_t shift = LED1;
                for (uint8_t k = 0; k < 12; k++)
                {
                    fadein(shift);
                    HAL_Delay(1000);
                    shift <<= 1;
                }
                shift = LED12;
                for (uint8_t k = 0; k < 12; k++)
                {
                    fadeout(shift);
                    HAL_Delay(1000);
                    shift >>= 1;
                }
            }
            break;
        }

        case 1:
        {
			//mode 1: fade in all LEDs, wait 2 seconds, fade out all LEDs, wait two seconds.
            for (uint8_t i=0; i<255; i++)
            {
                fadespeed = 3;
                fadeinALL();
                HAL_Delay(2000);
                fadespeed = 4;

                fadeoutALL();
                HAL_Delay(2000);
            }
            break;
        }

        case 2:
        {
			//mode 2: fade in a random LED, wait 700ms, fade out the selected LED
            fadespeed = 1;
            for (uint8_t i=0; i<255; i++)
            {
                uint32_t LED = 1 << (rand()%12);
                fadein(LED);
                HAL_Delay(700);
                fadeout(LED);
            }
            break;
        }

        case 3:
        {
			//mode 3: fade in all LEDs, fade out a random LED, wait 700 ms, fade in the selected LED (inverse to mode 2)
            fadeinALL();
            fadespeed = 1;
            for (uint8_t i=0; i<255; i++)
            {
                uint32_t LED = 1 << (rand()%12);
                fadeout(LED);
                HAL_Delay(700);
                fadein(LED);
            }
            break;
        }

        case 4:
        {
			//mode 4: fade in 3 random LEDs, wait 1.5 s, fade out 3 random LEDs
            fadespeed = 3;
            for (uint8_t i=0; i<255; i++)
            {
                uint32_t LED = (1 << (rand()%12)) | (1 << (rand()%12)) | (1 << (rand()%12));
                fadein(LED);
                HAL_Delay(1500);
                fadeout(LED);
            }
            break;
        }
        case 5:
        {
			//mode 5: enter standby mode
            HAL_PWR_EnterSTANDBYMode();
            break;
        }
      }
    HAL_PWR_EnterSTANDBYMode();
  }
}

void initGPIO()
{
	//enable clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

	//enable the GPIO banks
    GPIO_InitTypeDef gpio;
    gpio.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    gpio.Pull = GPIO_NOPULL;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &gpio);

    gpio.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
    gpio.Pull = GPIO_NOPULL;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_1;
    gpio.Pull = GPIO_NOPULL;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOB, &gpio);
	
	//disable all LEDs
    for (uint8_t k = 0; k < 12; k++)
        HAL_GPIO_WritePin(PORT2LEDMAP[k], GPIO2LEDMAP[k], GPIO_PIN_RESET);
}

//enable/disable one LED
void setLEDs(uint32_t LED, GPIO_PinState state)
{
    for (uint8_t i = 0; i < 12; i++)
    {
        if (LED & 1)
            HAL_GPIO_WritePin(PORT2LEDMAP[i], GPIO2LEDMAP[i], state);
        LED >>= 1;
    }
}

// Software-PWM fade effect
void fadein(int pin)
{
    for (uint16_t dutycycle = 2000; dutycycle>1; dutycycle=dutycycle/2)
    {
        for (int i=0; i<fadespeed*2; i++)
        {
			//the asm("") is necessary so the compiler doesn't optimize out the nops
            for (uint16_t duration=2000-dutycycle; duration>0; duration--)
                asm("");
            setLEDs(pin,GPIO_PIN_RESET);


            for (uint16_t duration=0; duration<dutycycle; duration++)
                asm("");
			
            setLEDs(pin,GPIO_PIN_SET);
        }
    }
}

void fadeout(int pin)
{
    for (uint16_t dutycycle = 2000; dutycycle>10; dutycycle=dutycycle/2)
    {
        for (int i=0; i<fadespeed; i++)
        {
            //for (uint16_t duration=0; duration<dutycycle; duration++)
            for (uint16_t duration=2000-dutycycle; duration>0; duration--)
                asm("");

            setLEDs(pin,GPIO_PIN_SET);

            //for (uint16_t duration=2000-dutycycle; duration>0; duration--)
            for (uint16_t duration=0; duration<dutycycle; duration++)
                asm("");

            setLEDs(pin,GPIO_PIN_RESET);
        }
    }

    return(0);
}

//fadein all LEDs
void fadeinALL()
{
    fadein(0xFFFFFFFF);

}

//fadeout all LEDs
void fadeoutALL()
{
    fadeout(0xFFFFFFFF);
}

//read the last blinking mode which was stored in the RTC backup registers
uint8_t getSetMode()
{
  RTC_HandleTypeDef hrtc;
  hrtc.Instance = RTC;

  HAL_PWR_EnableBkUpAccess();
  uint8_t mode = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
  uint8_t ret = mode;

  if (mode == NUM_MODES-1)
      mode = 0;
  else
      mode++;
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, mode);

  return ret;
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  */
void assert_failed( uint8_t* file, uint32_t line )
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while( 1 )
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
