/** @file   PS2_driver.h
 *  @author Ryan McBride    @RyanMcB8
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
/*                                                      Addition of typedefs                                                                              */
/* ====================================================================================================================================================== */
// #ifndef _Bool
// #define _Bool boolean
// #endif


/* A struct which holds data associated with the transmission of data as well as meta data for the controller. */
typedef struct{
    unsigned long last_read;
    uint8_t read_delay;
    uint8_t controller_type;
} PS2ControllerData_t;

/* A struct which holds variable to turn on and off the rumble and pressure sensing within the controller. */
typedef struct{
    _Bool en_Rumble;
    _Bool en_Pressures;
}  PS2FeedbackEnable_t;

/* A struct that holds all the necessary flags and masks for the transmission of the data. */
typedef struct{
    uint16_t 				_clk_mask; 
    volatile uint32_t *		_clk_lport_set;
    volatile uint32_t *		_clk_lport_clr;
    uint16_t 				_cmd_mask; 
    volatile uint32_t *		_cmd_lport_set;
    volatile uint32_t *		_cmd_lport_clr;
    uint16_t 				_att_mask; 
    volatile uint32_t *		_att_lport_set;
    volatile uint32_t *		_att_lport_clr;
    uint16_t 				_dat_mask; 
    volatile uint32_t *		_dat_lport;
} PS2Flags_t;

/* A struct which holds a memory of the button status' as well as the new status' which may be compared. */
typedef struct{
    unsigned int last_buttons;
    unsigned int buttons;   
} ButtonHistory_t;

/* A struct for holding the commands necessary for transmission and reception of data using the PS2 controller protocol. */
typedef struct{
    uint8_t enter_config[5];
    uint8_t set_mode[9];
    uint8_t set_bytes_large[9];
    uint8_t exit_config[9];
    uint8_t enable_rumble[5];
    uint8_t type_read[9];
} PS2Cmds_t;

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
/*                                                      Addition of function declarations                                                                 */
/* ====================================================================================================================================================== */

/** @brief A function that returns true when the one of the buttons has changed state. Either from on to off, or off to on.
 *  @param button The button which is being tested.
 *  @retval A value of type boolean confirming if there has been a change in state.
 */
extern _Bool NewButtonState(unsigned int button);

/** @brief A function which checks if a specific button has been pressed.
 *  @param button The button which is being tested.
 *  @retval A value of type boolean indiciating whether the button has been pressed or not.
 */
extern _Bool ButtonPressed(unsigned int button);

/** @brief A function which can check if a specific button has been released.
 *  @param button The button which is being tested.
 *  @retval A value of type boolean indicating if the button has been relesed or not.
 */
extern _Bool ButtonReleased(unsigned int button);

/** @brief A function which returns the current state of the button which is being tested. 1 if the button is being pressed, otherwise 0.
 *  @param button The button which is being pressed.
 *  @retval A value of type boolean describing the state of the button.
 */
extern _Bool Button(uint16_t button);

/** @brief A function which returns the button data as its byte form.
 *  @retval The binary representation of the current button states.
 */
unsigned int ButtonDataByte(void);


/** @brief A function which returns the ADC value for the specified analogue button with an 8-bit resolution.
 *  @param button The button in which is being tested.
 *  @retval The analogue value of the button being tested in 8 bit precision.
 */
uint8_t Analog(uint8_t button);

/** @brief I am actually not sure what this function does yet.
 *  @param byte Not sure about this either.
 *  @retval Nor this.
 */
uint8_t gamepad_shiftinout (uint8_t byte);

/** @brief A function which reads the value of the gamepad buttons and saves their values to the `buttons` variable. 
 *  This function is also responsible for controlling the internal motors for vibration feedback.
 *  @param motor1 The state of the small internal motor. 1 is on, 0 is off.
 *  @param motor2 The 8-bit resolution value for how much power should be supplied to the larger motor.
 *  @retval I am not sure what exactly is being returned for this function. Need to run more tests first.
 */
_Bool read_gamepad(_Bool motor1, uint8_t motor2);

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
uint8_t config_gamepad(uint8_t clk, uint8_t cmd, uint8_t att, uint8_t dat, _Bool pressures, _Bool rumble) ;

/** @brief A function which can transmit a string of uint8_tacters of any length to the controller.
 *  @param string A pointer to the array of uint8_tacters which should be transmitted.
 *  @param len The length of the array that is being transmitted.
 */
void sendCommandString(uint8_t* string[], uint8_t len);

/** @brief A function which checks which type of controller is connected. It can detect if it is a dualshock controller or a guitar or not compatible.
 *  @retval The type of controller detected: 0 is not compatible, 1 is guitar and 2 is the dualshock controller.
 */
uint8_t readType();

/** @brief A function which transmits a command to enable the rumble setting on the controller.
 * 
 */
void enableRumble();

/** @brief A function which transmits a command to enable the pressure settings on the controller.
 * 
 */
_Bool enablePressures();

/** @brief A function responsible for reconfiguring the gamepad. This transmits the most 
 *  recent rumble and pressure settings as well as other gamepad options previously selected. 
 *
 */
void reconfig_gamepad();

#endif

