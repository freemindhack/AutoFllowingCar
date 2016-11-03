#include "car.h"
#include "usart.h"

uint8_t tick_1ms;  //pwm模拟控制量

uint16_t pid_1ms; //pid 积分时间控制量


Car car;

static void Car_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	RCC_APB2PeriphClockCmd(WHEEL_RCC,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = WHEEL_FRONT_LEFT_1|WHEEL_FRONT_LEFT_2|WHEEL_FRONT_RIGHT_1|WHEEL_FRONT_RIGHT_2|WHEEL_REAR_LEFT_1|WHEEL_REAR_LEFT_2|WHEEL_REAR_RIGHT_1|WHEEL_REAR_RIGHT_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(WHEEL_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = HALL_LEFT | HALL_RIGHT;   //霍尔+码盘测速
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(HALL_PORT, &GPIO_InitStructure);			   
}

void Wheel_Hall_IO_Check(Wheel* w)
{
    w->cur_io_state = GPIO_ReadInputDataBit(WHEEL_PORT,HALL_LEFT);

	if(w->pre_io_state != w->cur_io_state)
	{
		w->count ++;
		w->pre_io_state = w->cur_io_state;
	}

}

uint16_t Wheel_Hall_CaculateSpeed(Wheel* w,uint8_t k)  
{
    uint32_t distance = 0;

    w->real_speed =  w->count * k;

	w->count = 0;

	return w->real_speed;
}

void Wheel_SetDuty(Wheel* w,uint8_t v)
{
  	w->speed = v;
}

void TIM3_IRQHandler(void)   
{
    float next_left_speed = 0,next_right_speed = 0;
	uint16_t left_count = 0,right_count = 0;


	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //清除TIMx的中断标志位

		tick_1ms++;

		pid_1ms++;
    		
		Wheel_Hall_IO_Check(&wheel_front_left);
		Wheel_Hall_IO_Check(&wheel_front_right);

		if(tick_1ms > 100)
		{
		    tick_1ms = 0;
		}

		if(pid_1ms>2000) //200ms
		{
		    pid_1ms = 0;

			left_count = Wheel_Hall_CaculateSpeed(&wheel_front_left,5);

			if(left_count > 20)			//这里的20 需要根据实际情况进行修改，可能== 码盘孔数
			{
				wheel_front_left.real_speed = (left_count-20)/3;

				//next_left_speed = PID_SpeedAjust(&wheel_front_left.pid,wheel_front_left.real_speed);

			    //Wheel_SetDuty(&wheel_front_left,next_left_speed);
			}

			right_count = Wheel_Hall_CaculateSpeed(&wheel_front_right,5);
			if(right_count > 20 )
			{
				wheel_front_right.real_speed = (right_count-20)/3;

				//next_right_speed = PID_SpeedAjust(&wheel_front_right.pid,wheel_front_right.real_speed);

				//Wheel_SetDuty(&wheel_front_right,next_right_speed);
			}

		   	printf("before pid     left_pid_speed : %d   right_pid_speed : %d \r\n",wheel_front_left.real_speed,wheel_front_right.real_speed);

			//printf("after pid     left_pid_speed : %0.2f   right_pid_speed : %0.2f \r\n",next_left_speed,next_right_speed);
		}			   

		Car_Move();

	}

}

void Wheel_PWMEmulate(Wheel* w,uint8_t duty)
{
    if(w->direction == STOP)
	{ 
	    GPIO_ResetBits(w->port,w->pin[0]);
		GPIO_ResetBits(w->port,w->pin[1]);
	    return;
	}

    if( duty > w->speed)
	{
	   	if(w->direction == FORWARD)
		{
		     
		    GPIO_ResetBits(w->port,w->pin[0]);
		}
		else
		{
		   	GPIO_ResetBits(w->port,w->pin[1]);
		}
	}
	else
	{
	    if(w->direction == FORWARD)
		{
		    GPIO_SetBits(w->port,w->pin[0]);
		}
		else
		{
		   	GPIO_SetBits(w->port,w->pin[1]);
		}
	} 
}


void Wheel_SpeedInc(Wheel* w,uint8_t val)
{
   	(w->speed + val) < 100 ? (w->speed += val) : (w->speed = 10); 
}


void Wheel_SpeedDec(Wheel* w,uint8_t val)
{
    w->speed - val > 0 ? (w->speed -= val) : (w->speed = 0);
}

void Wheel_SetDirection(Wheel* w,Wheel_Direction direction)
{
     w->direction = direction;

     if(w->direction == FORWARD)
	 {
	     GPIO_SetBits(w->port,w->pin[0]);
		 GPIO_ResetBits(w->port,w->pin[1]);  
	 }
	 else
	 {
	     GPIO_ResetBits(w->port,w->pin[0]);
		 GPIO_SetBits(w->port,w->pin[1]);  
	 }
}

void Car_Go(void)	//向前
{
	 Wheel_SetDirection(&wheel_front_left,FORWARD);

	 Wheel_SetDirection(&wheel_front_right,FORWARD);

	 Wheel_SetDirection(&wheel_rear_left,FORWARD);

	 Wheel_SetDirection(&wheel_rear_right,FORWARD);
}

void Car_Back(void) //向后
{
	 Wheel_SetDirection(&wheel_front_left,BACKWARD);

	 Wheel_SetDirection(&wheel_front_right,BACKWARD);

	 Wheel_SetDirection(&wheel_rear_left,BACKWARD);

	 Wheel_SetDirection(&wheel_rear_right,BACKWARD);
}

void Car_Left(void)//向左
{
    Wheel_SpeedDec(&wheel_front_left,10);
	Wheel_SetDirection(&wheel_front_left,BACKWARD);
	Wheel_SetDirection(&wheel_rear_left,BACKWARD);

	Wheel_SpeedInc(&wheel_front_right,40);
	Wheel_SetDirection(&wheel_front_right,FORWARD);

	Wheel_SpeedInc(&wheel_rear_right,30);
	Wheel_SetDirection(&wheel_rear_right,FORWARD);
}


void Car_Right(void) //向右
{
	Wheel_SetDirection(&wheel_front_right,BACKWARD);
	Wheel_SetDirection(&wheel_rear_right,BACKWARD);

	Wheel_SetDirection(&wheel_front_left,FORWARD);

	Wheel_SpeedInc(&wheel_rear_right,50);
	Wheel_SetDirection(&wheel_rear_left,FORWARD);
}

void Car_Stop(void)  //停止
{
    wheel_front_left.speed = 0;

	wheel_front_left.direction = STOP;

    wheel_front_right.speed = 0;
	wheel_front_right.direction = STOP;

	wheel_rear_left.speed = 0;
	wheel_rear_left.direction = STOP;

    wheel_rear_right.speed = 0;
	wheel_rear_right.direction = STOP;
}


void Car_Move(void)	  
{ 
     Wheel_PWMEmulate(&wheel_front_left,tick_1ms);
	 Wheel_PWMEmulate(&wheel_front_right,tick_1ms);

	 Wheel_PWMEmulate(&wheel_rear_left,tick_1ms);
	 Wheel_PWMEmulate(&wheel_rear_right,tick_1ms);
}

void Wheel_ParameterInit(Wheel* w,GPIO_TypeDef* port,uint16_t pin1,uint16_t pin2,Wheel_Direction direction,uint8_t speed)
{
	 w->port = port;
	 w->pin[0] = pin1;
	 w->pin[1] = pin2;
	 w->direction = direction;
	 w->speed = speed;

	 w->real_speed = speed;
	 w->pre_io_state = w->cur_io_state = 0;
	 w->count = 0;

	 w->encoder_holes = WHEEL_ENCODER_HOLES; 
	 w->radio = WHEEL_RADIO;
	 w->distance = 0;
	 w->phy_speed = 0;
	 
	 w->pid_time = PID_TIME;
}

void Parameter_Init(uint8_t front_left_speed,uint8_t front_right_speed,uint8_t rear_left_speed,uint8_t rear_right_speed)
{
   	Wheel_ParameterInit(&wheel_front_left,GPIOA,WHEEL_FRONT_LEFT_1,WHEEL_FRONT_LEFT_2,FORWARD,front_left_speed);
	Wheel_ParameterInit(&wheel_front_right,GPIOA,WHEEL_FRONT_RIGHT_1,WHEEL_FRONT_RIGHT_2,FORWARD,front_right_speed);


	Wheel_ParameterInit(&wheel_rear_left,GPIOA,WHEEL_REAR_LEFT_1,WHEEL_REAR_LEFT_2,FORWARD,rear_left_speed);
	Wheel_ParameterInit(&wheel_rear_right,GPIOA,WHEEL_REAR_RIGHT_1,WHEEL_REAR_RIGHT_2,FORWARD,rear_right_speed);

	//pid 调控左前  右前 轮子速度
	PID_ParameterInit(&wheel_front_left.pid,wheel_front_left.speed);
	PID_ParameterInit(&wheel_front_right.pid,wheel_front_right.speed);

}

float Car_GetRunDistance(void)
{
   
	return 	wheel_front_left.distance;
}

void Car_Init(void)
{
    Car_GPIO_Init();
	Parameter_Init(40,40,0,0);

	Car_Go();
	Init_TIMER(1);	 //0.1 ms

}
