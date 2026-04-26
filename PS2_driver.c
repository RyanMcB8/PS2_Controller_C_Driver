/* ==================== Including the necessary header files. ==================== */
#include "PS2_driver.h"


/* ==================== Defintion of any static variables. ==================== */

static char PS2CmdEnter_config[] = {0x01,0x43,0x00,0x01,0x00};
static char PS2CmdSet_mode[] = {0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
static char PS2CmdSet_bytes_large[] = {0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};
static char PS2CmdExit_config[] = {0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
static char PS2CmdEnable_rumble[] = {0x01,0x4D,0x00,0x00,0x01};
static char PS2CmdType_read[] = {0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};

/* ==================== Defintion of functions. ==================== */

_Bool NewButtonState(PS2ControllerStates_t *controller, uint16_t button) {
    return (((controller->buttonHistory.last_buttons ^ controller->buttonHistory.buttons) & button) > 0);
}

_Bool ButtonPressed(PS2ControllerStates_t *controller, uint16_t button) {
    return ((controller->buttonHistory.last_buttons ^ controller->buttonHistory.buttons) & button) & 
           controller->buttonHistory.buttons;
}

_Bool ButtonReleased(PS2ControllerStates_t *controller, uint16_t button) {
    return (((controller->buttonHistory.last_buttons ^ controller->buttonHistory.buttons) & button) & 
            (~controller->buttonHistory.last_buttons & button));
}

_Bool Button(PS2ControllerStates_t *controller, uint16_t button) {
    return ((controller->buttonHistory.buttons & button) > 0);
}


unsigned int ButtonDataByte(PS2ControllerStates_t *controller) {
    /*  Returning the inverted button mask. */
    return (~controller->buttonHistory.buttons);
}

uint8_t Analogue(PS2ControllerStates_t *controller, uint8_t button) {
    return controller->PS2data[button];
}

uint8_t gamepad_shiftinout(PS2ControllerStates_t *controller, uint8_t byte) {
    uint8_t tmp = 0;

    for(uint8_t i = 0; i < 8; i++) {
        /*  Matching the CMD pin state to the bit in byte index. */
        PS2_CHECK(byte, i) ? PS2_CMD_SET(controller) : PS2_CMD_CLR(controller);
        
        PS2_CLK_CLR(controller);  /*  Triggering clock low. */
        delayMicroseconds(CTRL_CLK);  /*    Debounce delay. */

        if (PS2_DAT_CHK(controller)) PS2_SET(tmp, i);  /* Reading the data. */
        PS2_CLK_SET(controller);  /* Triggering clock. */

        delayMicroseconds(CTRL_CLK_HIGH);  
    }

    PS2_CMD_SET(controller);  /*  Transmission complete. */
    delayMicroseconds(CTRL_BYTE_DELAY);  
    return tmp;  /* Returning the received byte. */
}

_Bool read_gamepad(PS2ControllerStates_t *controller, _Bool motor1, uint8_t motor2) {
    double temp = HAL_GetTick() - controller->data.last_read;

    if (temp > 1500) /* Too long between last poll, need to reconfigure the gamepad. */
        reconfig_gamepad(controller);

    if(motor2 != 0x00)
        /*  Scaling the input motor value to be between the minimum value for the motor to move
            and the maximum to have a linear relationship instead of an initial step. */
        motor2 = motor2 = (motor2 * (0xFF - 0x40) / 255) + 0x40;

    uint8_t dword[9] = {0x01,0x42,0,motor1,motor2,0,0,0,0};
    uint8_t dword2[12] = {0};

    PS2_CMD_SET(controller);
    PS2_CLK_SET(controller);
    PS2_ATT_CLR(controller); /* low enable joystick */

    delayMicroseconds(CTRL_BYTE_DELAY);

    /*  Transmitting the message by bitbashing instead of SPI protocol. */
    for (int i = 0; i<sizeof(dword); i++) {
        controller->PS2data[i] = gamepad_shiftinout(controller, dword[i]);
    }

    if(controller->PS2data[1] == 0x79) {  /* If controller is in full data return mode, get the rest of data. */
        for (int i = 0; i<sizeof(dword2); i++) {
            controller->PS2data[i+9] = gamepad_shiftinout(controller, dword2[i]);
        }
    }

    PS2_ATT_SET(controller); /* Disabling joystick. */
    /*  Checking if valid data was received. */
    if ((controller->PS2data[1] & 0xf0) == 0x70){
        /*  Invalid returned data. Reconfiguring and returning an error code.*/
        reconfig_gamepad(controller);
        return false;
    }

    controller->buttonHistory.last_buttons = controller->buttonHistory.buttons; //store the previous buttons states

    controller->buttonHistory.buttons =  (uint16_t)(controller->PS2data[4] << 8) + controller->PS2data[3];   //store as one value for multiple functions

    controller->data.last_read = HAL_GetTick();
    return ((controller->PS2data[1] & 0xf0) == 0x70);
}

uint8_t config_gamepad(PS2ControllerStates_t *controller, _Bool pressures, _Bool rumble) {
    uint8_t temp[9]; /* Reading response. */

    /*  Checking that the controller is responsive. */
    if (read_gamepad(controller, 0, 0) != 0 || read_gamepad(controller, 0, 0) != 0) {
        return 1; /*    Controller is not responsive - exit. */
    }

    /*  Ensuring that the response mode corresponds to one of the 3 valid options. */
    if (controller->PS2data[1] != 0x41 && controller->PS2data[1] != 0x73 && controller->PS2data[1] != 0x79) {
        return 1; // Error if mode is not correct
    }

    /*  Running this script until the controller is sucessfully configured or the maximum number of reattempts is reached. */
    for (int attempt = 0; attempt <= 10; attempt++) {
        /* Enter configuration mode. */
        sendCommandString(controller, PS2CmdEnter_config, sizeof(PS2CmdEnter_config));

        /* Reading the gamepad type. */
        HAL_GPIO_WritePin(controller->pins.att_GPIO_Port, controller->pins.att_GPIO_Pin, GPIO_PIN_RESET);
        delayMicroseconds(CTRL_BYTE_DELAY);

        /*  Bit banging the command type read. */
        for (int i = 0; i < sizeof(PS2CmdType_read); i++) {
            temp[i] = gamepad_shiftinout(controller, PS2CmdType_read[i]);
        }

        HAL_GPIO_WritePin(controller->pins.att_GPIO_Port, controller->pins.att_GPIO_Pin, GPIO_PIN_SET); /*    Disabling gamepad. */

        controller->data.controller_type = temp[3];

        /*  Setting the rumble and pressures to be enabled/disabled accordingly. */
        sendCommandString(controller, PS2CmdSet_mode, sizeof(PS2CmdSet_mode));
        if (rumble) {
            sendCommandString(controller, PS2CmdEnable_rumble, sizeof(PS2CmdEnable_rumble));
            controller->feedback.en_Rumble = true;
        }

        if (pressures) {
            sendCommandString(controller, PS2CmdSet_bytes_large, sizeof(PS2CmdSet_bytes_large));
            controller->feedback.en_Pressures = true;
        }

        /*  Configuration complete. */
        sendCommandString(controller, PS2CmdExit_config, sizeof(PS2CmdExit_config));

        /*  Reading the gamepad to ensure success. */
        read_gamepad(controller, 0, 0);

        if (pressures) {
            if (controller->PS2data[1] == 0x79) break;  // Successful, pressures enabled
            if (controller->PS2data[1] == 0x73) return 3;  // Pressure mode error
        }

        if (controller->PS2data[1] == 0x73) break;  // Success with no pressures

        // If we've reached the maximum attempts, return error
        if (attempt == 10) {
            return 2; // Configuration failed after 10 attempts
        }

        controller->data.read_delay += 1; // Increase delay for the next attempt
    }

    return 0;
}

void sendCommandString(PS2ControllerStates_t *controller, uint8_t* string, uint8_t len) {

    PS2_ATT_CLR(controller); // low enable joystick

    /*  Bit banging again. */
    for (uint8_t y=0; y < len; y++)
        gamepad_shiftinout(controller, string[y]);

    PS2_ATT_SET(controller); //high disable joystick  
    delay(controller->data.read_delay);
}

uint8_t readType(PS2ControllerStates_t *controller) {
    if(controller->data.controller_type == 0x03)
        return 1;
    else if(controller->data.controller_type == 0x01)
        return 2;

    return 0;
}

void enableRumble(PS2ControllerStates_t *controller) {

    sendCommandString(controller, PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(controller, PS2CmdEnable_rumble, sizeof(PS2CmdEnable_rumble));
    sendCommandString(controller, PS2CmdExit_config, sizeof(PS2CmdExit_config));
    controller->feedback.en_Rumble = true;

}

_Bool enablePressures(PS2ControllerStates_t *controller) {

    /*  Entering configuration mode.*/
    sendCommandString(controller, PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(controller, PS2CmdSet_bytes_large, sizeof(PS2CmdSet_bytes_large));
    sendCommandString(controller, PS2CmdExit_config, sizeof(PS2CmdExit_config));

    read_gamepad(controller, controller->feedback.en_Rumble, 0xff);
    read_gamepad(controller, controller->feedback.en_Rumble, 0xff);

    /*  Ensuring that valid data was returned and that there was no error. */
    if(controller->PS2data[1] != 0x79){
        return false;
    }

    controller->feedback.en_Pressures = true;
    return true;
}

void reconfig_gamepad(PS2ControllerStates_t *controller){
    /*  Sending the command to enter configuration mode instead of analogue. */
    sendCommandString(controller, PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(controller, PS2CmdSet_mode, sizeof(PS2CmdSet_mode));
    
    /*  Enabling rumble if it has been enabled in settings. */
    if (controller->feedback.en_Rumble){
        sendCommandString(controller, PS2CmdEnable_rumble, sizeof(PS2CmdEnable_rumble));
    }

    /*  Enabling pressures if it has been enabled in settings. */
    if (controller->feedback.en_Pressures){
        sendCommandString(controller, PS2CmdSet_bytes_large, sizeof(PS2CmdSet_bytes_large));
    }
    sendCommandString(controller, PS2CmdExit_config, sizeof(PS2CmdExit_config));

}



