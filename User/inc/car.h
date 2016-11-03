#ifndef __CAR_H_
#define __CAR_H_

#include "stm32f10x.h"
#include "pid.h"

#define WHEEL_RCC           RCC_APB2Periph_GPIOA
#define WHEEL_PORT          GPIOA

#define WHEEL_FRONT_LEFT_1  GPIO_Pin_0 	  //左前
#define WHEEL_FRONT_LEFT_2  GPIO_Pin_1 

#define WHEEL_FRONT_RIGHT_1 GPIO_Pin_2	 //右前
#define WHEEL_FRONT_RIGHT_2 GPIO_Pin_3

#define WHEEL_REAR_LEFT_1 GPIO_Pin_4	//左后
#define WHEEL_REAR_LEFT_2 GPIO_Pin_5

#define WHEEL_REAR_RIGHT_1 GPIO_Pin_6	 //右后
#define WHEEL_REAR_RIGHT_2 GPIO_Pin_7

#define HALL_PORT          GPIOA
#define HALL_LEFT          GPIO_Pin_11
#define HALL_RIGHT          GPIO_Pin_12

#define WHEEL_RADIO  33   //mm
#define WHEEL_ENCODER_HOLES 20

#define PI 314

#define PID_TIME 200 //ms

typedef enum
{
    FORWARD = 0,
	BACKWARD,
	STOP
}Wheel_Direction;

typedef struct 
{
    GPIO_TypeDef* port; //控制端口
    uint16_t pin[2];        //控制脚
   	Wheel_Direction direction;   //轮子转动方向
	uint16_t speed;       //设定的转动速度	 pwm
	uint16_t real_speed;   //当前实际的转动速度 pwm

	uint8_t  encoder_holes;//码盘孔数目
	uint8_t  radio;    //车轮半径
	uint32_t    distance; //当前行走路程
	uint32_t    phy_speed; //轮子当前实际速度
	
    uint8_t pre_io_state;
	uint8_t cur_io_state;

	uint32_t count;

	PID pid;

	uint16_t pid_time;

}Wheel;


typedef struct 
{
    Wheel wheel[4];	 //4个轮子

	#define wheel_front_left car.wheel[0]
	#define wheel_front_right car.wheel[1]
	#define wheel_rear_left car.wheel[2]
	#define wheel_rear_right car.wheel[3]


}Car;


void Car_Init(void);
void Car_Move(void);

void Car_Go(void);	
void Car_Back(void);
void Car_Left(void);
void Car_Right(void);
void Car_Stop(void);

void Parameter_Init(uint8_t front_left_speed,uint8_t front_right_speed,uint8_t rear_left_speed,uint8_t rear_right_speed);
float Car_GetRunDistance(void);

#endif
