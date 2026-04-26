/** @file   PS2_driver.h
 *  @author Ryan McBride   
 *  @brief  A header file defining all the necessary types for the transmission of data using the PS2 controller's protocol.
 *          This also features all the necessary function declarations so that the functions may be called within the main program.
 */

/* Preventing recurssion if the header file is called upon mutliple times. */
#ifndef _PS2_DRIVER_H_
#define _PS2_DRIVER_H_

/* ====================================================================================================================================================== */
/*                                                      Addition of include                                                                               */
/* ====================================================================================================================================================== */

#include <stdio.h>
#include <stdint.h>

/* ====================================================================================================================================================== */
/*                                                     Definition of types used                                                                           */
/* ====================================================================================================================================================== */

/* Definition of true. */
#ifndef true
#define true 1
#endif

/* Defintion of false. */
#ifndef false
#define false 0
#endif

// #ifdef TEST_PS2_CONTROLLER
#include "STM32_PS2_Example.h"
// #endif

/* THESE DEFINTIONS ARE TEMPORARY AND MUST BE ALTERED TO BE THE CORRECT DELAY FUNCTION FOR THE PLATFORM USED. */
/* Defintion of delay. */
#ifndef delayMicroseconds
#define delayMicroseconds(x) x 
#endif

/* Defintion of delay. */
#ifndef delay
#define delay(x) HAL_Delay(x) 
#endif

/* ====================================================================================================================================================== */
/*                                                      Addition of typedefs                                                                              */
/* ====================================================================================================================================================== */


/* A struct which holds data associated with the transmission of data as well as meta data for the controller. */
typedef struct{
    uint64_t last_read;
    uint8_t read_delay;
    uint8_t controller_type;
} PS2ControllerData_t;

/* A struct which holds variable to turn on and off the rumble and pressure sensing within the controller. */
typedef struct{
    _Bool en_Rumble;
    _Bool en_Pressures;
}  PS2FeedbackEnable_t;

/* A struct that holds all the necessary port and pin definitions of each connection to the controller. */
typedef struct{
    uint16_t                    clk_GPIO_Pin; 
    const GPIO_TypeDef *	    clk_GPIO_Port;
    uint16_t                    cmd_GPIO_Pin; 
    const GPIO_TypeDef *	    cmd_GPIO_Port;
    uint16_t                    att_GPIO_Pin; 
    const GPIO_TypeDef *	    att_GPIO_Port;
    uint16_t                    dat_GPIO_Pin; 
    const GPIO_TypeDef *	    dat_GPIO_Port;
} PS2Pins_t;

/* A struct which holds a memory of the button status' as well as the new status' which may be compared. */
typedef struct{
    unsigned int last_buttons;
    unsigned int buttons;   
} PS2ButtonHistory_t;

/*  A struct to store all the states of the controller. */
typedef struct{
    PS2Pins_t pins;
    PS2FeedbackEnable_t feedback;
    PS2ButtonHistory_t buttonHistory;
    PS2ControllerData_t data;
    unsigned char PS2data[21];
} PS2ControllerStates_t;


/* ====================================================================================================================================================== */
/*                                                      Addition of definitions                                                                           */
/* ====================================================================================================================================================== */

#define CTRL_CLK        6 //5
#define CTRL_CLK_HIGH   6 //5
#define CTRL_BYTE_DELAY 5 //4

/* Home buttons */
#define PS_SELECT      0x0001
#define PS_L3          0x0002
#define PS_R3          0x0004
#define PS_START       0x0008

/* D-Pad buttons*/
#define PS_PAD_UP      0x0010
#define PS_PAD_RIGHT   0x0020
#define PS_PAD_DOWN    0x0040
#define PS_PAD_LEFT    0x0080

/* Bumper buttons*/
#define PS_L2          0x0100
#define PS_R2          0x0200
#define PS_L1          0x0400
#define PS_R1          0x0800

/* Main buttons */
#define PS_TRIANGLE    0x1000
#define PS_CIRCLE      0x2000
#define PS_CROSS       0x4000
#define PS_SQUARE      0x8000

/* Analogue joystick identifiers. */
#define PS_RX 5
#define PS_RY 6
#define PS_LX 7
#define PS_LY 8


/* Bit math definitions. */
#define PS2_SET(x,y) (x|=(1<<y))
#define PS2_CLEAR(x,y) (x&=(~(1<<y)))
#define PS2_CHECK(x,y) (x & (1<<y))
#define PS2_TOGGLE(x,y) (x^=(1<<y))

/* ====================================================================================================================================================== */
/*                                                      Creation of the inline void functions                                                             */
/* ====================================================================================================================================================== */


/** @brief                  A function to set the clock pin high.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_CLK_SET(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.clk_GPIO_Port, controller->pins.clk_GPIO_Pin, GPIO_PIN_SET);
}

/** @brief                  A function to set the clock pin low.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_CLK_CLR(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.clk_GPIO_Port, controller->pins.clk_GPIO_Pin, GPIO_PIN_RESET);
}

/** @brief                  A function to set the command pin high.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_CMD_SET(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.cmd_GPIO_Port, controller->pins.cmd_GPIO_Pin, GPIO_PIN_SET);
}

/** @brief                  A function to set the command pin low.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_CMD_CLR(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.cmd_GPIO_Port, controller->pins.cmd_GPIO_Pin, GPIO_PIN_RESET);
}

/** @brief                  A function to set the attention pin high.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_ATT_SET(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.att_GPIO_Port, controller->pins.att_GPIO_Pin, GPIO_PIN_SET);
}

/** @brief                  A function to set the attention pin low.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline void PS2_ATT_CLR(PS2ControllerStates_t *controller) {
    HAL_GPIO_WritePin(controller->pins.att_GPIO_Port, controller->pins.att_GPIO_Pin, GPIO_PIN_RESET);
}

/** @brief                  A function to read the value of the data pin.
 *  @param  controller      A pointer to the PS2ControllerStates_t instance for the specified
 *                          controller.
 */
static inline _Bool PS2_DAT_CHK(PS2ControllerStates_t *controller) {
    return HAL_GPIO_ReadPin(controller->pins.att_GPIO_Port, controller->pins.att_GPIO_Pin);
}

/* ====================================================================================================================================================== */
/*                                                      Addition of function declarations                                                                 */
/* ====================================================================================================================================================== */
/** @brief A function that is capable of updating the button states
 *  @param button An unsigned 16 bit integer referencing the button ID defined in "PS2_driver.h"
 *  @retval Boolean value indicating if there has been a change in button states from the last time it was run.
*/
_Bool NewButtonState(PS2ControllerStates_t *controller, uint16_t button);

/** @brief A function that checks if the button has been pressed.
 *  @param button An unsigned 16 bit integer referencing the button ID defined in "PS2_driver.h".
 *  @retval Returns a boolean value indicating if the button referenced has changed state.
 */
_Bool ButtonPressed(PS2ControllerStates_t *controller, uint16_t button);

/** @brief A function which can check if a specific button has been released.
 *  @param button The button which is being tested.
 *  @retval A value of type boolean indicating if the button has been relesed or not.
 */
extern _Bool ButtonReleased(PS2ControllerStates_t *controller, uint16_t button);

/** @brief A function which returns the current state of the button which is being tested. 1 if the button is being pressed, otherwise 0.
 *  @param button The button which is being pressed.
 *  @retval A value of type boolean describing the state of the button.
 */
extern _Bool Button(PS2ControllerStates_t *controller, uint16_t button) ;

/** @brief A function which returns the button data as its byte form.
 *  @retval The binary representation of the current button states.
 */
unsigned int ButtonDataByte(PS2ControllerStates_t *controller);


/** @brief A function which returns the ADC value for the specified analogue button with an 8-bit resolution.
 *  @param button The button in which is being tested.
 *  @retval The analogue value of the button being tested in 8 bit precision.
 */
uint8_t Analogue(PS2ControllerStates_t *controller, uint8_t button);

/** @brief  Bit banging the data through to the controller from the mcu.
 *  @details    This is not necessary if SPI is implemented properly instead. 
 *              This is duplex communication with the shift registers so data
 *              is being both transmitted and received.
 *  @param byte The data byte which should be transmitted.
 *  @retval     Returns the received byte.
 */
uint8_t gamepad_shiftinout (PS2ControllerStates_t *controller, uint8_t byte);

/** @brief A function which reads the value of the gamepad buttons and saves their values to the `buttons` variable. 
 *  This function is also responsible for controlling the internal motors for vibration feedback.
 *  @param motor1 The state of the small internal motor. 1 is on, 0 is off.
 *  @param motor2 The 8-bit resolution value for how much power should be supplied to the larger motor.
 *  @retval I am not sure what exactly is being returned for this function. Need to run more tests first.
 */
_Bool read_gamepad(PS2ControllerStates_t *controller, _Bool motor1, uint8_t motor2);

/** @brief A function responsible for initialising the controller by transmitting the necessary
 *         values to address the correct registers holding parameters for enabling the rumble etc. 
 *  @param clk The clock parameter
 *  @param cmd The command that is being sent
 *  @param att The .... 
 *  @param dat The data which follows the command.
 *  @param pressures Whether the pressure should be tracked or not.
 *  @param rumble Whether the rumble feature should be enabled or not.
 *  @retval The type of controller which has been detected.
 * 
 */
uint8_t config_gamepad(PS2ControllerStates_t *controller, _Bool pressures, _Bool rumble) ;

/** @brief A function which can transmit a string of uint8_tacters of any length to the controller.
 *  @param string A pointer to the array of uint8_tacters which should be transmitted.
 *  @param len The length of the array that is being transmitted.
 */
void sendCommandString(PS2ControllerStates_t *controller, uint8_t* string, uint8_t len);

/** @brief A function which checks which type of controller is connected. It can detect if it is a dualshock controller or a guitar or not compatible.
 *  @retval The type of controller detected: 0 is not compatible, 1 is guitar and 2 is the dualshock controller.
 */
uint8_t readType(PS2ControllerStates_t *controller);

/** @brief A function which transmits a command to enable the rumble setting on the controller.
 * 
 */
void enableRumble(PS2ControllerStates_t *controller);

/** @brief A function which transmits a command to enable the pressure settings on the controller.
 * 
 */
_Bool enablePressures(PS2ControllerStates_t *controller);

/** @brief A function responsible for reconfiguring the gamepad. This transmits the most 
 *  recent rumble and pressure settings as well as other gamepad options previously selected. 
 *
 */
void reconfig_gamepad(PS2ControllerStates_t *controller);


#endif /* End of recurssion prevention. */

