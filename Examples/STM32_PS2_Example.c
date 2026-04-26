/** @file           STM32_PS2_Example.c
 *  @author         Ryan McBride
 *  @details        This file is used to show how the functions defined in the 
 *                  PS2_driver.c file should be implemented to allow for a PS2
 *                  dualshock controller to be connected to an STM32 
 *                  microcontroller to receive inputs and control the vibration
 *                  of the controller.
 */

 /* Adding the necessary include header files. */
 #include "PS2_driver.h"

 /** @brief     Initialises the controller with the chosen pin values and then returns 0 if there are no errors.
  *             1 otherwise.
  */
 _Bool initController(PS2ControllerStates_t *controller);
 

 int main(){
    /* Create an instance of the controller. */
    PS2ControllerStates_t controller;

    /*  Initiate the controller settings. */
    initController(&controller);

    /*  Declaring variables to store the values being read. */
    uint8_t left_x;
    uint8_t left_y;
    
    while(true){
        read_gamepad(&controller, 0, 0);
        left_x = Analogue(&controller, PS_LX); 
        left_y = Analogue(&controller, PS_LY); 
    }    

    return 0;
 }

 _Bool initController(PS2ControllerStates_t *controller){
    /* Setting the pins which are being used for the controller. */
    controller->pins.att_GPIO_Pin = 0x00;
    controller->pins.clk_GPIO_Pin = 0x01;
    controller->pins.cmd_GPIO_Pin = 0x02;
    controller->pins.dat_GPIO_Pin = 0x04;
    controller->pins.att_GPIO_Port = GPIOA;
    controller->pins.clk_GPIO_Port = GPIOB;
    controller->pins.cmd_GPIO_Port = GPIOC;
    controller->pins.dat_GPIO_Port = GPIOD;

    /*  Setting whether the rumble and pressures should be enabled. */
    controller->feedback.en_Pressures = true;
    controller->feedback.en_Rumble = true;

    /* Setting the initial configuration. */
    if (0 !=config_gamepad(controller, controller->feedback.en_Pressures, controller->feedback.en_Rumble)) return 1;
    return 0;
 }
