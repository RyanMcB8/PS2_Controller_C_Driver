/** @file           STM32_PS2_Example.c
 *  @author         Ryan McBride
 *  @details        This file is used to set the platform dependent functions
 *                  that would not be able to be compiled if the .
 */

 /*    Preventing recursion. */
 #ifndef _STM32_PS2_EXAMPLE_H_ 
 #define _STM32_PS2_EXAMPLE_H_

 /* Adding necessary include headers. */
 #include <stdint.h>

 /* Definition functions that would be declared within the STM libraries. */
 #define GPIO_PIN_SET   1
 #define GPIO_PIN_RESET 0
 #define HAL_Delay(x)   x
 #define GPIO_TypeDef   int8_t
 #define HAL_GPIO_WritePin(x, y, z) ;
 #define HAL_GPIO_ReadPin(x, y) 1
 
 #define GPIOA  (const int8_t *) 0
 #define GPIOB  (const int8_t *) 1
 #define GPIOC  (const int8_t *) 2
 #define GPIOD  (const int8_t *) 3

 #define HAL_GetTick() 10

 #endif /* End of STM32_PS2_Example.h file. */