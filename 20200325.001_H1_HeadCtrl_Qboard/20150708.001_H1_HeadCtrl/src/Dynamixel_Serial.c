
#include "asf.h"
#include "user.h"
#include "Dynamixel_Serial.h"


//##############################################################################
//############################ Public Methods ##################################
//##############################################################################

uint8_t   Instruction_Packet_Array[14];   // Array to hold instruction packet data
uint8_t   Status_Packet_Array[8];         // Array to hold returned status packet data
unsigned long   Time_Counter;                   // Timer for time out watchers
char            Direction_Pin;                  // Pin to control TX/RX buffer chip
uint8_t   Status_Return_Value;            // Status packet return states ( NON , READ , ALL )
uint32_t ServoCom_Idx = 0xffffffff;

void fDinamyxel_setComDirection(uint32_t UARTinstance, uint32_t direction)
{

	if(UARTinstance == 0xffffffff)return;

   if(direction)
   {
		usart_disable_transceiver(UARTinstance, USART_TRANSCEIVER_RX);
		usart_enable_transceiver(UARTinstance, USART_TRANSCEIVER_TX);
   }
   else
   {
		usart_disable_transceiver(UARTinstance, USART_TRANSCEIVER_TX);
		usart_enable_transceiver(UARTinstance, USART_TRANSCEIVER_RX);
   }

}
uint16_t fDinamyxel_reset(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = RESET_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_RESET;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_ping(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = PING_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_PING;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
        return (Status_Packet_Array[0]);            // Return SERVO ID
    }else{
        return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
    }

}

uint16_t fDinamyxel_setStatusPaketReturnDelay(uint8_t ID,uint8_t ReturnDelay){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_RETURN_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_RETURN_DELAY_TIME;
    Instruction_Packet_Array[4] = (uint8_t)(ReturnDelay/2);

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_setID(uint8_t ID, uint8_t New_ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_ID_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_ID;
    Instruction_Packet_Array[4] = New_ID;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_setBaudRate(uint8_t ID, long Baud){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_BD_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_BAUD_RATE;

    switch (Baud){
        case 1000000:
            Instruction_Packet_Array[4] = 0x01;
            break;
        case 2250000:
            Instruction_Packet_Array[4] = 0xFA;
            break;
        case 2500000:
            Instruction_Packet_Array[4] = 0xFB;
            break;
        case 3000000:
            Instruction_Packet_Array[4] = 0xFC;
            break;
        default:
        Instruction_Packet_Array[4] = (uint8_t)((2000000/Baud) - 1);
    }

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_setMaxTorque( uint8_t ID, int Torque){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_MAX_TORQUE_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_MAX_TORQUE_L ;
    Instruction_Packet_Array[4] = (uint8_t)(Torque);
    Instruction_Packet_Array[5] = (uint8_t)(Torque >> 8);

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_setHoldingTorque(uint8_t ID, bool Set){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_HOLDING_TORQUE_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_TORQUE_ENABLE;
    Instruction_Packet_Array[4] = Set;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_setAlarmShutdown(uint8_t  ID,uint8_t Set){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_ALARM_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_ALARM_SHUTDOWN;
    Instruction_Packet_Array[4] = Set;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_setStatusPaket(uint8_t  ID,uint8_t Set){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_RETURN_LEVEL_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_RETURN_LEVEL;
    Instruction_Packet_Array[4] = Set;

    Status_Return_Value = Set;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}


uint16_t fDinamyxel_setMode(uint8_t ID, bool Dynamixel_Mode, uint16_t Dynamixel_CW_Limit,uint16_t Dynamixel_CCW_Limit){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_MODE_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_CW_ANGLE_LIMIT_L;
    if (Dynamixel_Mode == WHEEL) {                                    // Set WHEEL mode, this is done by setting both the clockwise and anti-clockwise angle limits to ZERO
        Instruction_Packet_Array[4] = 0x00;
        Instruction_Packet_Array[5] = 0x00;
        Instruction_Packet_Array[6] = 0x00;
        Instruction_Packet_Array[7] = 0x00;
    }else {                                                             // Else set SERVO mode
        Instruction_Packet_Array[4] = (uint8_t)(Dynamixel_CW_Limit);
        Instruction_Packet_Array[5] = (uint8_t)((Dynamixel_CW_Limit & 0x0F00) >> 8);
        Instruction_Packet_Array[6] = (uint8_t)(Dynamixel_CCW_Limit);
        Instruction_Packet_Array[7] = (uint8_t)((Dynamixel_CCW_Limit & 0x0F00) >> 8);
    }

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (Status_Return_Value == ALL){
    fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] != 0)
		{
			return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
		else
			return 0;
    }
 }

 uint16_t fDinamyxel_setPunch(uint8_t ID,uint16_t Punch){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_PUNCH_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_PUNCH_L;
    Instruction_Packet_Array[4] = (uint8_t)(Punch);
    Instruction_Packet_Array[5] = (uint8_t)(Punch >> 8);

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

 }

 uint16_t fDinamyxel_setPID(uint8_t ID ,uint8_t P,uint8_t I,uint8_t D){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_PID_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_DERIVATIVE_GAIN;
    Instruction_Packet_Array[4] = D;
    Instruction_Packet_Array[5] = I;
    Instruction_Packet_Array[6] = P;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

 }

uint16_t fDinamyxel_setTemp(uint8_t ID,uint8_t temp){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_TEMP_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_LIMIT_TEMPERATURE;
    Instruction_Packet_Array[4] = temp;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_setVoltage(uint8_t ID,uint8_t Volt_L, uint8_t Volt_H){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SET_VOLT_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = EEPROM_LOW_LIMIT_VOLTAGE;
    Instruction_Packet_Array[4] = Volt_L;
    Instruction_Packet_Array[5] = Volt_H;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_servo(uint8_t ID,uint16_t Position,uint16_t Speed){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SERVO_GOAL_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_GOAL_POSITION_L;
    Instruction_Packet_Array[4] = (uint8_t)(Position);
    Instruction_Packet_Array[5] = (uint8_t)((Position & 0x0F00) >> 8);
    Instruction_Packet_Array[6] = (uint8_t)(Speed);
    Instruction_Packet_Array[7] = (uint8_t)((Speed & 0x0F00) >> 8);

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);


    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_servoPreload(uint8_t ID,uint16_t Position,uint16_t Speed){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = SERVO_GOAL_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_REG_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_GOAL_POSITION_L;
    Instruction_Packet_Array[4] = (uint8_t)(Position);
    Instruction_Packet_Array[5] = (uint8_t)(Position >> 8);
    Instruction_Packet_Array[6] = (uint8_t)(Speed);
    Instruction_Packet_Array[7] = (uint8_t)(Speed >> 8);

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_wheel(uint8_t ID, bool Rotation,uint16_t Speed){

    uint8_t Speed_H,Speed_L;
    Speed_L = Speed;
    if (Rotation == 0){                         // Move Left
        Speed_H = Speed >> 8;

    }else if (Rotation == 1){                    // Move Right
        Speed_H = (Speed >> 8)+4;
    }

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = WHEEL_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_GOAL_SPEED_L;
    Instruction_Packet_Array[4] = Speed_L;
    Instruction_Packet_Array[5] = Speed_H;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

void fDinamyxel_wheelSync(uint8_t ID1, bool Dir1, uint16_t Speed1, uint8_t ID2, bool Dir2, uint16_t Speed2, uint8_t ID3, bool Dir3, uint16_t Speed3){

    uint8_t Speed1_H,Speed1_L;
    Speed1_L = Speed1;
        if (Dir1 == 0){                          // Move Left
            Speed1_H = Speed1 >> 8;
        }
        else if (Dir1 == 1)                     // Move Right
        {
            Speed1_H = (Speed1 >> 8)+4;
        }

    uint8_t Speed2_H,Speed2_L;
    Speed2_L = Speed2;
        if (Dir2 == 0){                          // Move Left
            Speed2_H = Speed2 >> 8;
        }
        else if (Dir2 == 1)                     // Move Right
        {
            Speed2_H = (Speed2 >> 8)+4;
        }

    uint8_t Speed3_H,Speed3_L;
    Speed3_L = Speed3;
        if (Dir3 == 0){                          // Move Left
            Speed3_H = Speed3 >> 8;
        }
        else if (Dir3 == 1)                     // Move Right
        {
            Speed3_H = (Speed3 >> 8)+4;
        }

    Instruction_Packet_Array[0] = 0xFE;         // When Writing a Sync comman you must address all(0xFE) servos
    Instruction_Packet_Array[1] = SYNC_LOAD_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_SYNC_WRITE;
    Instruction_Packet_Array[3] = RAM_GOAL_SPEED_L;
    Instruction_Packet_Array[4] = SYNC_DATA_LENGTH;
    Instruction_Packet_Array[5] = ID1;
    Instruction_Packet_Array[6] = Speed1_L;
    Instruction_Packet_Array[7] = Speed1_H;
    Instruction_Packet_Array[8] = ID2;
    Instruction_Packet_Array[9] = Speed2_L;
    Instruction_Packet_Array[10] = Speed2_H;
    Instruction_Packet_Array[11] = ID3;
    Instruction_Packet_Array[12] = Speed3_L;
    Instruction_Packet_Array[13] = Speed3_H;

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

}

uint16_t fDinamyxel_wheelPreload(uint8_t ID, bool Dir,uint16_t Speed){

    uint8_t Speed_H,Speed_L;
    Speed_L = Speed;
        if (Dir == 0){                          // Move Left
            Speed_H = Speed >> 8;
        }
        else if (Dir == 1)                      // Move Right
        {
            Speed_H = (Speed >> 8)+4;
        }

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = WHEEL_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_REG_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_GOAL_SPEED_L;
    Instruction_Packet_Array[4] = Speed_L;
    Instruction_Packet_Array[5] = Speed_H;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }

}

uint16_t fDinamyxel_action(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = RESET_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_ACTION;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_ledState(uint8_t ID, bool Status){

    delay_ms(100);                                           // Sending packets too fast has been problematic

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = LED_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_WRITE_DATA;
    Instruction_Packet_Array[3] = RAM_LED;
    Instruction_Packet_Array[4] = Status;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);

    if (ID == 0XFE || Status_Return_Value != ALL ){     // If ID of FE is used no status packets are returned so we do not need to check it
        return (0x00);
    }else{
        fDinamyxel_readStatusPacket(ServoCom_Idx);
        if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
            return (Status_Packet_Array[0]);            // Return SERVO ID
        }else{
            return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
        }
    }
}

uint16_t fDinamyxel_readTemperature(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_TEMP_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_PRESENT_TEMPERATURE;
    Instruction_Packet_Array[4] = READ_ONE_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
        return Status_Packet_Array[3];
    }else{
        return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_readPosition(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_POS_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_PRESENT_POSITION_L;
    Instruction_Packet_Array[4] = READ_TWO_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value                                          // If there is no status packet error return value
        return Status_Packet_Array[4] << 8 | Status_Packet_Array[3];    // Return present position value
    }else{
        return (Status_Packet_Array[2] | 0xF000);                           // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_readLoad(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_LOAD_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_PRESENT_LOAD_L;
    Instruction_Packet_Array[4] = READ_TWO_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){                                           // If there is no status packet error return value
        return ((Status_Packet_Array[4] << 8) | Status_Packet_Array[3]);    // Return present load value
    }else{
        return (Status_Packet_Array[2] | 0xF000);                                   // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_readSpeed(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_SPEED_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_PRESENT_SPEED_L;
    Instruction_Packet_Array[4] = READ_TWO_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){                                           // If there is no status packet error return value
        return (Status_Packet_Array[4] << 8) | Status_Packet_Array[3];  // Return present position value
    }else{
        return (Status_Packet_Array[2] | 0xF000);                           // If there is a error Returns error value
    }
}


uint16_t fDinamyxel_readVoltage(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_VOLT_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_PRESENT_VOLTAGE;
    Instruction_Packet_Array[4] = READ_ONE_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){                   // If there is no status packet error return value
        return Status_Packet_Array[3];                  // Return voltage value (value retured by Dynamixel is 10 times actual voltage)
    }else{
        return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_checkRegister(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_REGISTER_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_REGISTER;
    Instruction_Packet_Array[4] = READ_ONE_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){                   // If there is no status packet error return value
        return (Status_Packet_Array[3]);            // Return register value
    }else{
        return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_checkMovement(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_MOVING_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_MOVING;
    Instruction_Packet_Array[4] = READ_ONE_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){                   // If there is no status packet error return value
        return (Status_Packet_Array[3]);            // Return movement value
    }else{
        return (Status_Packet_Array[2] | 0xF000);            // If there is a error Returns error value
    }
}

uint16_t fDinamyxel_checkLock(uint8_t ID){

    Instruction_Packet_Array[0] = ID;
    Instruction_Packet_Array[1] = READ_LOCK_LENGTH;
    Instruction_Packet_Array[2] = COMMAND_READ_DATA;
    Instruction_Packet_Array[3] = RAM_LOCK;
    Instruction_Packet_Array[4] = READ_ONE_BYTE_LENGTH;

    fDinamyxel_clearRXbuffer(ServoCom_Idx);

    fDinamyxel_transmitInstructionPacket(ServoCom_Idx);
    fDinamyxel_readStatusPacket(ServoCom_Idx);

    if (Status_Packet_Array[2] == 0){               // If there is no status packet error return value
        return (Status_Packet_Array[3]);            // Return Lock value
    }else{
        return (Status_Packet_Array[2] | 0xF000);   // If there is a error Returns error value
    }
}


//##############################################################################
//########################## HAL Methods ###################################
//##############################################################################

void fDinamyxel_transmitInstructionPacket(uint32_t UARTinstance)
{
	uint8_t TX_Buffer[sizeof(Instruction_Packet_Array)+3];
	uint8_t i;
	uint16_t checksum_packet;

	if(UARTinstance == 0xffffffff)return;

	// Transmit instruction packet to Dynamixel

	// UART_HAL_SetTransmitterDir(g_uartBase[UARTinstance],kUartSinglewireTxdirOut);
	
	usart_disable_transceiver(UARTinstance, USART_TRANSCEIVER_RX);
	usart_enable_transceiver(UARTinstance, USART_TRANSCEIVER_TX);


    TX_Buffer[0] = HEADER;                                                             // 1 Write Header (0xFF) data 1 to serial
    TX_Buffer[1] = HEADER;                                                             // 2 Write Header (0xFF) data 2 to serial
    TX_Buffer[2] = Instruction_Packet_Array[0];                                        // 3 Write Dynamixel ID to serial
    TX_Buffer[3] = Instruction_Packet_Array[1];                                        // 4 Write packet length to serial
    TX_Buffer[4] = Instruction_Packet_Array[2];                                        // 5 Write instruction type to serial

    checksum_packet = Instruction_Packet_Array[0] + Instruction_Packet_Array[1] + Instruction_Packet_Array[2];

    for (i = 3; i <= Instruction_Packet_Array[1]; i++)
    {
        TX_Buffer[i+2] = Instruction_Packet_Array[i];                                    // Write Instuction & Parameters (if there are any) to serial
        checksum_packet += Instruction_Packet_Array[i];
    }
    i += 2;
    if(i < sizeof(TX_Buffer))
    {
    	TX_Buffer[i] = ~checksum_packet & 0xFF;                                            // Write low bit of checksum to serial
    }
	usart_write_buffer_wait(UARTinstance, TX_Buffer, sizeof(TX_Buffer));

	
	/*UART_HAL_SendDataPolling(g_uartBase[UARTinstance], TX_Buffer, sizeof(TX_Buffer));
  	UART_HAL_SetTransmitterDir(g_uartBase[UARTinstance],kUartSinglewireTxdirIn);*/
}


uint16_t fDinamyxel_readStatusPacket(uint32_t UARTinstance)
{
	uint8_t RX_Buffer[sizeof(Instruction_Packet_Array)+3];
    uint8_t First_Header = 0x00;
    uint8_t counter = 0;
    uint8_t nByteReceived = 0 , i = 0;

    if(UARTinstance == NULL)
		return 0xffffffff;

    Status_Packet_Array[0] = 0x00;
    Status_Packet_Array[1] = 0x00;
    Status_Packet_Array[2] = 0x00;
    Status_Packet_Array[3] = 0x00;

	if(usart_read_buffer_wait(UARTinstance, &nByteReceived, STATUS_FRAME_BUFFER) == STATUS_ERR_TIMEOUT)
				return Status_Packet_Array[2] = 0x80;                  // Return with Error if Serial data not received with in time limit

/*
	if(UART_DRV_ReceiveDataBlocking (UARTinstance, &nByteReceived, STATUS_FRAME_BUFFER, STATUS_PACKET_TIMEOUT) == kStatus_UART_Timeout)
	{
		return Status_Packet_Array[2] = 0x80;                  // Return with Error if Serial data not received with in time limit
	}*/



    if (RX_Buffer[i] == 0xFF && First_Header != 0xFF)
    {
        First_Header =RX_Buffer[i++];                                                 // Clear 1st header from RX buffer
    }

    if(RX_Buffer[i++] == 0xFF && First_Header == 0xFF)
    {
        Status_Packet_Array[0] =RX_Buffer[i++];                	                   // ID sent from Dynamixel
        Status_Packet_Array[1] = RX_Buffer[i++];    	                           // Frame Length of status packet
        Status_Packet_Array[2] = RX_Buffer[i++]; 	                               // Error byte
		i = 0;

        if(usart_read_buffer_wait(UARTinstance, &nByteReceived, Status_Packet_Array[1] - 2) == STATUS_ERR_TIMEOUT)
		{
			return Status_Packet_Array[2] = 0x80;                  // Return with Error if Serial data not received with in time limit
		}

        for(counter = 0; counter < Status_Packet_Array[1]; counter++)
        {
            Status_Packet_Array[3 + counter] = RX_Buffer[i++];
        }

        Status_Packet_Array[counter + 4] = RX_Buffer[i++];                         // Read Check sum

    }
    else
    {
        return Status_Packet_Array[2] = 0x80;                                      // Return with Error if two headers are not found
    }
	
	return 0;		

}

void fDinamyxel_clearRXbuffer(uint32_t UARTinstance)
{
	if(UARTinstance == NULL)
		return;
		/* Flush receiver */
	usart_disable_transceiver(UARTinstance, USART_TRANSCEIVER_RX);
	usart_enable_transceiver(UARTinstance, USART_TRANSCEIVER_RX);
}

void fDinamyxel_Init(uint32_t UARTinstance)
{
	ServoCom_Idx = UARTinstance;
}



