#ifndef __DRIVER2_H__
#define __DRIVER2_H__

#define BLDC_Position_Mode              0x01  
//#define BLDC_RePosition_Mode              0x02

#define BLDC_Velocity1_Mode               0x03
#define BLDC_Velocity2_Mode               0xFD
#define BLDC_Moment_Mode                  0x04
#define BLDC_OriginPoint_Mode             0x06

#define BLDC_ControlWord_Off              0x06   //Let the motor to be offLine
#define BLDC_AbControlWord_On             0x1F   //Let the motor to be onLine
#define BLDC_ReControlWord_On             0x0F
#define BLDC_ControlWord_AlarmClean       0x86   //Get rid of Alarm

static unsigned char BLDC_State_1 = 0x06;        //Flag for the motor state
static unsigned char BLDC_State_2 = 0x06;
static unsigned char BLDC_State_3 = 0x06;
static unsigned char BLDC_State_4 = 0x06;

char get_CAN_BLDC_MOTOR_STATE(unsigned char Number);
void set_CAN_BLDC_MOTOR_STATE(unsigned char Number, char state);

static void CAN_BLDC_Delay_Us(unsigned int t);
void CAN_BLDC(unsigned char Number, int RW, long obj, long setValue, char* returnDate);
void CAN_BLDC_ResetMod(unsigned char Number, unsigned char Mode);
void CAN_BLDC_Control(unsigned char Number, unsigned char ControlWord);
void CAN_BLDC_accelerationSet(unsigned char Number, long acceleration);
void CAN_BLDC_decelerationSet(unsigned char Number, long deceleration);
void CAN_BLDC_Trapezoidal_speed(unsigned char Number, long speed);
void CAN_BLDC_Stop(unsigned char Number, long flag);
void CAN_BLDC_AbPositionMod(unsigned char Number, long Temp_Position);
void CAN_BLDC_RePositionMod(unsigned char Number, long Temp_Position, long acceleration, long deceleration);

#endif