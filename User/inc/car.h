#ifndef __CAR_H_
#define __CAR_H_

#include "stm32f10x.h"

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
	uint8_t speed;       //设定的转动速度
	uint8_t real_speed;   //当前实际的转动速度
}Wheel;


typedef struct 
{
    uint8_t last_left_io_state;
    uint8_t cur_left_io_state; 

	uint8_t last_right_io_state;
	uint8_t cur_right_io_state;

	uint32_t left_count;
	uint32_t right_count;

	uint8_t left_speed;
	uint8_t right_speed;

}Car_Speed;


typedef struct 
{
    Wheel wheel[4];	 //4个轮子

	#define wheel_front_left car.wheel[0]
	#define wheel_front_right car.wheel[1]
	#define wheel_rear_left car.wheel[2]
	#define wheel_rear_right car.wheel[3]


	Car_Speed speed;  //速度
	#define car_speed car.speed

}Car;


void Car_Init(void);
void Car_Move(void);

#endif
