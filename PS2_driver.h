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
/* Boolean data */
#ifndef _Bool
#define _Bool boolean
#endif

/* 16 bit unsigned integers. */
#ifndef uint16_t
#define uint16_t unsigned int
#endif

/* 16 bit unsigned integers. */
#ifndef uint8_t
#define uint8_t char
#endif

/* Definition of true. */
#ifndef true
#define true 1
#endif

/* Defintion of false. */
#ifndef false
#define false 0
#endif

/* THESE DEFINTIONS ARE TEMPORARY AND MUST BE ALTERED TO BE THE CORRECT DELAY FUNCTION FOR THE PLATFORM USED. */
/* Defintion of delay. */
#ifndef delayMicroseconds
#define delayMicroseconds(x) x*10 
#endif

/* Defintion of delay. */
#ifndef delay
#define delay(x) x*10 
#endif

/* ====================================================================================================================================================== */
/*                                                      Addition of typedefs                                                                              */
/* ====================================================================================================================================================== */


/* A struct which holds data associated with the transmission of data as well as meta data for the controller. */
typedef struct{
    unsigned long last_read;
    char read_delay;
    char controller_type;
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
} PS2ButtonHistory_t;

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

inline void PS2_CLK_SET(void);
inline void PS2_CLK_CLR(void);
inline void PS2_CMD_SET(void);
inline void PS2_CMD_CLR(void);
inline void PS2_ATT_SET(void);
inline void PS2_ATT_CLR(void);
inline _Bool PS2_DAT_CHK(void);

/* ====================================================================================================================================================== */
/*                                                      Addition of function declarations                                                                 */
/* ====================================================================================================================================================== */
/** @brief A function that is capable of updating the button states
 *  @param button An unsigned 16 bit integer referencing the button ID defined in "PS2_driver.h"
 *  @retval Boolean value indicating if there has been a change in button states from the last time it was run.
*/
_Bool NewButtonState(uint16_t button);

/** @brief A function that checks if the button has been pressed.
 *  @param button An unsigned 16 bit integer referencing the button ID defined in "PS2_driver.h".
 *  @retval Returns a boolean value indicating if the button referenced has changed state.
 */
_Bool ButtonPressed(uint16_t button);



_Bool Button(uint16_t);



#endif /* End of recurssion prevention. */

