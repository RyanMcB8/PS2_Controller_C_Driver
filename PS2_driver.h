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
#ifndef _Bool
#define _Bool boolean
#endif


/* A struct which holds data associated with the transmission of data as well as meta data for the controller. */
typedef struct{
    unsigned long last_read;
    char read_delay;
    char controller_type;
} PS2ControllerData;

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
    char enter_config[5];
    char set_mode[9];
    char set_bytes_large[9];
    char exit_config[9];
    char enable_rumble[5];
    char type_read[9];
} PS2Cmds_t;

/* ====================================================================================================================================================== */
/*                                                      Addition of definitions                                                                           */
/* ====================================================================================================================================================== */

#define CTRL_CLK        6 //5
#define CTRL_CLK_HIGH   6 //5
#define CTRL_BYTE_DELAY 5 //4
#endif 

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


#endif

