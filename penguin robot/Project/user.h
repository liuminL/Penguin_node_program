#ifndef __USER_H
#define __USER_H

// 板子定义USE_板子_BOARD
#define USE_LEFT_FRONT_BOARD
//RF 定义
// 坐标转换定义
#define yawBaseOff_RF 	18146
#define hipBaseOff_RF 	2346
#define kneeBaseOff_RF 	15590
// 坐标转换符号定义，正负
#define yawBaseSym_RF 	1  //-1
#define hipBaseSym_RF 	1
#define kneeBaseSym_RF	-1
// 电机转向符号定义，正负
#define yawSpeedSym_RF 	1  //-1
#define hipSpeedSym_RF 	-1
#define kneeSpeedSym_RF -1

//LF 定义
#define yawBaseOff_LF 	16200 
#define hipBaseOff_LF 	23034 
#define kneeBaseOff_LF 	24577 

#define yawBaseSym_LF 	1  //-1
#define hipBaseSym_LF		-1
#define kneeBaseSym_LF 	1 

#define yawSpeedSym_LF 	-1
#define hipSpeedSym_LF 	1
#define kneeSpeedSym_LF 1 
//RB 定义
#define yawBaseOff_RB 	15824
#define hipBaseOff_RB 	19744
#define kneeBaseOff_RB 	28050

#define yawBaseSym_RB 	-1
#define hipBaseSym_RB 	1
#define kneeBaseSym_RB 	-1

#define yawSpeedSym_RB 	-1
#define hipSpeedSym_RB 	1
#define kneeSpeedSym_RB 1 
//LB 定义
#define yawBaseOff_LB 	19176
#define hipBaseOff_LB 	5316
#define kneeBaseOff_LB 	3916  

#define yawBaseSym_LB 	-1
#define hipBaseSym_LB 	-1
#define kneeBaseSym_LB 	1 

#define yawSpeedSym_LB 	1
#define hipSpeedSym_LB	-1
#define kneeSpeedSym_LB -1 

#define Joint_Num 3

#define Knee_Minangle_LF 	2143+80  
#define Knee_Maxangle_LF 	2886-80   
#define Hip_Minangle_LF 	2057+80 
#define Hip_Maxangle_LF 	2822-80
#define Yaw_Minangle_LF 	1582+80 
#define Yaw_Maxangle_LF 	2059-80


#define Knee_Minangle_LB 	2739+80   
#define Knee_Maxangle_LB 	3487-80
#define Hip_Minangle_LB 	455+80 
#define Hip_Maxangle_LB 	1229-80 
#define Yaw_Minangle_LB 	1941+80
#define Yaw_Maxangle_LB 	2418-80

#define Knee_Minangle_RF 	1739+80
#define Knee_Maxangle_RF 	2480-80
#define Hip_Minangle_RF 	1060+80 
#define Hip_Maxangle_RF 	1850-80
#define Yaw_Minangle_RF 	1800+80      
#define Yaw_Maxangle_RF 	2284-80


#define Knee_Minangle_RB 	1059+80
#define Knee_Maxangle_RB 	1804-80
#define Hip_Minangle_RB 	1657+80
#define Hip_Maxangle_RB 	2417-80
#define Yaw_Minangle_RB 	1593+80
#define Yaw_Maxangle_RB 	2089-80

#define speedMax 5000
#define speedMin -5000

#define PC_ID										0X00
#define POWER_BOARD_ID					0X01
#define LEFT_FRONT_BOARD_ID     0X02
#define LEFT_BACK_BOARD_ID      0X03
#define RIGHT_FRONT_BOARD_ID    0X04
#define RIGHT_BACK_BOARD_ID			0X05
#define HEADER_BOARD_ID      		0X06
#define TAIL_BOARD_ID						0X07

// 板子切换定义
#ifdef USE_RIGHT_BACK_BOARD
	#define FILTER_ID	RIGHT_BACK_BOARD_ID
	
	#define yawBaseOff yawBaseOff_RB
	#define hipBaseOff hipBaseOff_RB
	#define kneeBaseOff kneeBaseOff_RB
	
	#define yawBaseSym yawBaseSym_RB
	#define hipBaseSym hipBaseSym_RB
	#define kneeBaseSym kneeBaseSym_RB
	
	#define yawSpeedSym yawSpeedSym_RB
	#define hipSpeedSym hipSpeedSym_RB
	#define kneeSpeedSym kneeSpeedSym_RB	
	
	#define Knee_Minangle Knee_Minangle_RB
	#define Knee_Maxangle Knee_Maxangle_RB
	#define Hip_Maxangle Hip_Maxangle_RB
	#define Hip_Minangle Hip_Minangle_RB
	#define Yaw_Maxangle Yaw_Maxangle_RB
	#define Yaw_Minangle Yaw_Minangle_RB

#endif

#ifdef USE_LEFT_BACK_BOARD
	#define FILTER_ID	LEFT_BACK_BOARD_ID
	
	#define yawBaseOff yawBaseOff_LB
	#define hipBaseOff hipBaseOff_LB
	#define kneeBaseOff kneeBaseOff_LB
	
	#define yawBaseSym yawBaseSym_LB
	#define hipBaseSym hipBaseSym_LB
	#define kneeBaseSym kneeBaseSym_LB
	
	#define yawSpeedSym yawSpeedSym_LB
	#define hipSpeedSym hipSpeedSym_LB
	#define kneeSpeedSym kneeSpeedSym_LB
	
	#define Knee_Minangle Knee_Minangle_LB
	#define Knee_Maxangle Knee_Maxangle_LB
	#define Hip_Maxangle Hip_Maxangle_LB
	#define Hip_Minangle Hip_Minangle_LB
	#define Yaw_Maxangle Yaw_Maxangle_LB
	#define Yaw_Minangle Yaw_Minangle_LB	
	
#endif

#ifdef USE_LEFT_FRONT_BOARD
	#define FILTER_ID	LEFT_FRONT_BOARD_ID
	
	#define yawBaseOff yawBaseOff_LF
	#define hipBaseOff hipBaseOff_LF
	#define kneeBaseOff kneeBaseOff_LF
	
	#define yawBaseSym yawBaseSym_LF
	#define hipBaseSym hipBaseSym_LF
	#define kneeBaseSym kneeBaseSym_LF
	
	#define yawSpeedSym yawSpeedSym_LF
	#define hipSpeedSym hipSpeedSym_LF
	#define kneeSpeedSym kneeSpeedSym_LF
	
	#define Knee_Minangle Knee_Minangle_LF
	#define Knee_Maxangle Knee_Maxangle_LF
	#define Hip_Maxangle Hip_Maxangle_LF
	#define Hip_Minangle Hip_Minangle_LF
	#define Yaw_Maxangle Yaw_Maxangle_LF
	#define Yaw_Minangle Yaw_Minangle_LF	
	
#endif

#ifdef USE_RIGHT_FRONT_BOARD
	#define FILTER_ID	RIGHT_FRONT_BOARD_ID
	
	#define yawBaseOff yawBaseOff_RF
	#define hipBaseOff hipBaseOff_RF
	#define kneeBaseOff kneeBaseOff_RF
	
	#define yawBaseSym yawBaseSym_RF
	#define hipBaseSym hipBaseSym_RF
	#define kneeBaseSym kneeBaseSym_RF
	
	#define yawSpeedSym yawSpeedSym_RF
	#define hipSpeedSym hipSpeedSym_RF
	#define kneeSpeedSym kneeSpeedSym_RF
	
	#define Knee_Minangle Knee_Minangle_RF
	#define Knee_Maxangle Knee_Maxangle_RF
	#define Hip_Maxangle Hip_Maxangle_RF
	#define Hip_Minangle Hip_Minangle_RF
	#define Yaw_Maxangle Yaw_Maxangle_RF
	#define Yaw_Minangle Yaw_Minangle_RF
	
#endif

#endif
