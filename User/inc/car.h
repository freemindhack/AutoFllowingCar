#ifndef __CAR_H_
#define __CAR_H_

#include "stm32f10x.h"
#include "pid.h"

#define WHEEL_RCC           RCC_APB2Periph_GPIOB
#define WHEEL_PORT          GPIOB

#define WHEEL_FRONT_LEFT_1  GPIO_Pin_6 	  //左前
#define WHEEL_FRONT_LEFT_2  GPIO_Pin_7 

#define WHEEL_FRONT_RIGHT_1 GPIO_Pin_8	 //右前
#define WHEEL_FRONT_RIGHT_2 GPIO_Pin_9

#define HALL_RCC           RCC_APB2Periph_GPIOA
#define HALL_PORT          GPIOA
#define HALL_LEFT          GPIO_Pin_11
#define HALL_RIGHT          GPIO_Pin_12

#define WHEEL_RADIO  0.033   //m
#define WHEEL_ENCODER_HOLES 20

#define PI 3.1415

#define PID_TIME 200 //ms

typedef enum
{
    Dutyfactor_100 = 7200,
	Dutyfactor_90  = 6480,
	Dutyfactor_85  = 6120,
	Dutyfactor_80  = 5760,
	Dutyfactor_75  = 5400,
	Dutyfactor_70  = 5040,
	Dutyfactor_65  = 4680,
	Dutyfactor_60  = 4320,
	Dutyfactor_55  = 3960,
	Dutyfactor_50  = 3600,
	Dutyfactor_48  = 3456,
	Dutyfactor_46  = 3312,
	Dutyfactor_45  = 3240,
	Dutyfactor_40  = 2880,
	Dutyfactor_35  = 2520,
	Dutyfactor_30  = 2160,
	Dutyfactor_25  = 1800,
	Dutyfactor_20  = 1440,
	Dutyfactor_10  = 720,
	Dutyfactor_0   = 0,
}TIM_ZKB;

#define MIN_DUTY Dutyfactor_30
#define MIN_SPEED  0.1
#define MAX_SPEED  0.8


typedef enum
{
    FORWARD = 0,
	BACKWARD,
	STOP
}Wheel_Direction;

typedef enum
{
	 FRONT_LEFT = 0,
     FRONT_RIGHT,
}WHEEL_ID;

typedef struct 
{
    WHEEL_ID id;

    GPIO_TypeDef* port; //控制端口
    uint16_t pin[2];        //控制脚
   	Wheel_Direction direction;   //轮子转动方向

	float speed;       //设定的转动速度	 
	float real_speed;   //当前实际的转动速度 

	uint8_t  encoder_holes;//码盘孔数目
	float  radio;    //车轮半径
	float    distance; //当前行走路程

	float circles;
	
    uint8_t pre_io_state;
	uint8_t cur_io_state;

	uint32_t count;

	PID pid;
	uint16_t pid_time;
	uint8_t pid_enable;

}Wheel;


typedef struct 
{
    Wheel wheel[4];	 //4个轮子
}Car;



#define wheel_front_left car.wheel[0]
#define wheel_front_right car.wheel[1]
#define wheel_rear_left car.wheel[2]
#define wheel_rear_right car.wheel[3]


void Car_Init(void);
void Car_Go(void);	
void Car_Back(void);
void Car_Left(void);
void Car_Right(void);
void Car_Stop(void);


static void Car_GPIO_Init(void);
float Car_GetRunDistance(void);
void Car_RunCtl(void);
void Car_SpeedInc(Car* car,float speed);
void Car_SpeedDec(Car* car,float speed);
void Wheel_SetDirectionAndSpeed(Wheel* w,Wheel_Direction direction,uint16_t speed);

#endif
