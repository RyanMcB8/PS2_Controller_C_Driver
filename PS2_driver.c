/* ==================== Including the necessary header files. ==================== */
#include "PS2_driver.h"


/* ==================== Defintion of any static variables. ==================== */

static char PS2CmdEnter_config[] = {0x01,0x43,0x00,0x01,0x00};
static char PS2CmdSet_mode[] = {0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
static char PS2CmdSet_bytes_large[] = {0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};
static char PS2CmdExit_config[] = {0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
static char PS2CmdEnable_rumble[] = {0x01,0x4D,0x00,0x00,0x01};
static char PS2CmdType_read[] = {0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};

static PS2ControllerData_t PS2ControllerData;

static PS2FeedbackEnable_t PS2FeedbackEnable;

static PS2ButtonHistory_t PS2ButtonHistory;

/* ==================== Defintion of any non static variables. ==================== */

unsigned char PS2data[21];

PS2Flags_t PS2Flags;

/* ==================== Defintion of functions. ==================== */

_Bool NewButtonState(uint16_t button) {
    return (((PS2ButtonHistory.last_buttons ^ PS2ButtonHistory.buttons) & button) > 0);
}

_Bool ButtonPressed(uint16_t button) {
    return(NewButtonState(button) & Button(button));
}


_Bool ButtonReleased(uint16_t button) {
    return((NewButtonState(button)) & ((~PS2ButtonHistory.last_buttons & button) > 0));
}

_Bool Button(uint16_t button) {
    return ((PS2ButtonHistory.buttons & button) > 0);
}


unsigned int ButtonDataByte() {
    return (~PS2ButtonHistory.buttons);
}

char Analog(char button) {
    return PS2data[button];
}


char _gamepad_shiftinout (char byte) {
    unsigned char tmp = 0;
    for(uint8_t i=0;i<8;i++) {

        if(PS2_CHECK(byte,i)) PS2_CMD_SET();
        else  PS2_CMD_CLR();
        PS2_CLK_CLR();

        delayMicroseconds(CTRL_CLK);

        if(PS2_DAT_CHK()) PS2_SET(tmp,i);
        PS2_CLK_SET();
#if CTRL_CLK_HIGH
        delayMicroseconds(CTRL_CLK_HIGH);
#endif	  
    }
    PS2_CMD_SET();
    delayMicroseconds(CTRL_BYTE_DELAY);
    return tmp;
}

_Bool read_gamepad(_Bool motor1, char motor2) {
    double temp = millis() - last_read;

    if (temp > 1500) //waited to long
        reconfig_gamepad();

    if(temp < read_delay)  //waited too short
        delay(read_delay - temp);



    if(motor2 != 0x00)
        motor2 = map(motor2,0,255,0x40,0xFF); //noting below 40 will make it spin

    byte dword[9] = {0x01,0x42,0,motor1,motor2,0,0,0,0};
    byte dword2[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

    // Try a few times to get valid data...
    for (byte RetryCnt = 0; RetryCnt < 5; RetryCnt++) {
        CMD_SET();
        CLK_SET();
        ATT_CLR(); // low enable joystick

        delayMicroseconds(CTRL_BYTE_DELAY);
        //Send the command to send button and joystick data;

        for (int i = 0; i<9; i++) {
            PS2data[i] = _gamepad_shiftinout(dword[i]);
        }


        if(PS2data[1] == 0x79) {  //if controller is in full data return mode, get the rest of data
            for (int i = 0; i<12; i++) {
                PS2data[i+9] = _gamepad_shiftinout(dword2[i]);
            }
        }

        ATT_SET(); // HI disable joystick
        // Check to see if we received valid data or not.  We should be in analog mode for our data
        // to be valie
        if ((PS2data[1] & 0xf0) == 0x70)
            break;

        // If we got to here, we are not in analog mode, try to recover...
        reconfig_gamepad();	// try to get back into Analog mode.
        delay(read_delay);
    }

    // If we get here and still not in analog mode, try increasing the read_delay...
    if ((PS2data[1] & 0xf0) != 0x70) {
        if (read_delay < 10)
            read_delay++;	// see if this helps out...
    }	


#ifdef PS2X_COM_DEBUG
    Serial.println("OUT:IN");
    for(int i=0; i<9; i++){
        Serial.print(dword[i], HEX);
        Serial.print(":");
        Serial.print(PS2data[i], HEX);
        Serial.print(" ");
    }
    for (int i = 0; i<12; i++) {
        Serial.print(dword2[i], HEX);
        Serial.print(":");
        Serial.print(PS2data[i+9], HEX);
        Serial.print(" ");
    }
    Serial.println("");	
#endif

    ButtonHistory.last_buttons = ButtonHistory.buttons; //store the previous buttons states

#if defined(__AVR__)
    uint16_t * tmp = (uint16_t*)(PS2data+3);
    ButtonHistory.buttons = *tmp;
#else
    ButtonHistory.buttons =  (uint16_t)(PS2data[4] << 8) + PS2data[3];   //store as one value for multiple functions
#endif
    last_read = millis();
    return ((PS2data[1] & 0xf0) == 0x70);
}


char config_gamepad(uint8_t clk, uint8_t cmd, uint8_t att, uint8_t dat, _Bool pressures, _Bool rumble) {

    byte temp[sizeof(type_read)];

#ifdef __AVR__
    _clk_mask = digitalPinToBitMask(clk);
    _clk_oreg = portOutputRegister(digitalPinToPort(clk));
    _cmd_mask = digitalPinToBitMask(cmd);
    _cmd_oreg = portOutputRegister(digitalPinToPort(cmd));
    _att_mask = digitalPinToBitMask(att);
    _att_oreg = portOutputRegister(digitalPinToPort(att));
    _dat_mask = digitalPinToBitMask(dat);
    _dat_ireg = portInputRegister(digitalPinToPort(dat));
#else

    uint32_t            lport;                   // Port number for this pin
    _clk_mask = digitalPinToBitMask(clk); 
    lport = digitalPinToPort(clk);
    _clk_lport_set = portOutputRegister(lport) + 2;
    _clk_lport_clr = portOutputRegister(lport) + 1;

    _cmd_mask = digitalPinToBitMask(cmd); 
    lport = digitalPinToPort(cmd);
    _cmd_lport_set = portOutputRegister(lport) + 2;
    _cmd_lport_clr = portOutputRegister(lport) + 1;

    _att_mask = digitalPinToBitMask(att); 
    lport = digitalPinToPort(att);
    _att_lport_set = portOutputRegister(lport) + 2;
    _att_lport_clr = portOutputRegister(lport) + 1;

    _dat_mask = digitalPinToBitMask(dat); 
    _dat_lport = portInputRegister(digitalPinToPort(dat));

#endif  

    pinMode(clk, OUTPUT); //configure ports
    pinMode(att, OUTPUT);
    pinMode(cmd, OUTPUT);
    pinMode(dat, INPUT);

#if defined(__AVR__)
    digitalWrite(dat, HIGH); //enable pull-up 
#endif

    CMD_SET(); // SET(*_cmd_oreg,_cmd_mask);
    CLK_SET();

    //new error checking. First, read gamepad a few times to see if it's talking
    read_gamepad();
    read_gamepad();

    //see if it talked
    if(PS2data[1] != 0x41 && PS2data[1] != 0x73 && PS2data[1] != 0x79){ //see if mode came back. If still anything but 41, 73 or 79, then it's not talking
#ifdef PS2X_DEBUG
        Serial.println("Controller mode not matched or no controller found");
        Serial.print("Expected 0x41 or 0x73, got ");
        Serial.println(PS2data[1], HEX);
#endif

        return 1; //return error code 1
    }

    //try setting mode, increasing delays if need be. 
    read_delay = 1;

    for(int y = 0; y <= 10; y++)
    {
        sendCommandString(enter_config, sizeof(enter_config)); //start config run

        //read type
        delayMicroseconds(CTRL_BYTE_DELAY);

        CMD_SET();
        CLK_SET();
        ATT_CLR(); // low enable joystick

        delayMicroseconds(CTRL_BYTE_DELAY);

        for (int i = 0; i<9; i++) {
            temp[i] = _gamepad_shiftinout(type_read[i]);
        }

        ATT_SET(); // HI disable joystick

        controller_type = temp[3];

        sendCommandString(set_mode, sizeof(set_mode));
        if(rumble){ sendCommandString(enable_rumble, sizeof(enable_rumble)); en_Rumble = true; }
        if(pressures){ sendCommandString(set_bytes_large, sizeof(set_bytes_large)); en_Pressures = true; }
        sendCommandString(exit_config, sizeof(exit_config));

        read_gamepad();

        if(pressures){
            if(PS2data[1] == 0x79)
                break;
            if(PS2data[1] == 0x73)
                return 3;
        }

        if(PS2data[1] == 0x73)
            break;

        if(y == 10){
#ifdef PS2X_DEBUG
            Serial.println("Controller not accepting commands");
            Serial.print("mode stil set at");
            Serial.println(PS2data[1], HEX);
#endif
            return 2; //exit function with error
        }

        read_delay += 1; //add 1ms to read_delay
    }

    return 0; //no error if here
}

void sendCommandString(uint8_t* string[], uint8_t len) {

#ifdef PS2X_COM_DEBUG
    char temp[len];
    ATT_CLR(); // low enable joystick
    delayMicroseconds(CTRL_BYTE_DELAY);

    for (uint8_t y=0; y < len; y++)
        temp[y] = _gamepad_shiftinout(string[y]);

    ATT_SET(); //high disable joystick  
    delay(read_delay);                  //wait a few

    Serial.println("OUT:IN Configure");
    for(int i=0; i<len; i++){
        Serial.print(string[i], HEX);
        Serial.print(":");
        Serial.print(temp[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");

#else
    PS2_ATT_CLR(); // low enable joystick
    for (uint8_t y=0; y < len; y++)
        _gamepad_shiftinout(string[y]);

    PS2_ATT_SET(); //high disable joystick  
    delay(PS2ControllerData.read_delay);                  //wait a few
#endif
}

char readType() {
    if(PS2ControllerData.controller_type == 0x03)
        return 1;
    else if(PS2ControllerData.controller_type == 0x01)
        return 2;

    return 0;
}

void enableRumble() {

    sendCommandString(PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(PS2CmdEnable_rumble, sizeof(PS2CmdEnable_rumble));
    sendCommandString(PS2CmdExit_config, sizeof(PS2CmdExit_config));
    PS2FeedbackEnable.en_Rumble = true;

}

_Bool enablePressures() {

    sendCommandString(PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(PS2CmdSet_bytes_large, sizeof(PS2CmdSet_bytes_large));
    sendCommandString(PS2CmdExit_config, sizeof(PS2CmdExit_config));

    read_gamepad();
    read_gamepad();

    if(PS2data[1] != 0x79)
        return false;

    PS2FeedbackEnable.en_Pressures = true;
    return true;
}

void reconfig_gamepad(){

    sendCommandString(PS2CmdEnter_config, sizeof(PS2CmdEnter_config));
    sendCommandString(PS2CmdSet_mode, sizeof(PS2CmdSet_mode));
    if (PS2FeedbackEnable.en_Rumble)
        sendCommandString(PS2CmdEnable_rumble, sizeof(PS2CmdEnable_rumble));
    if (PS2FeedbackEnable.en_Pressures)
        sendCommandString(PS2CmdSet_bytes_large, sizeof(PS2CmdSet_bytes_large));
    sendCommandString(PS2CmdExit_config, sizeof(PS2CmdExit_config));

}



