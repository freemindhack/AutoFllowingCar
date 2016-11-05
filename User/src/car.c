#include "car.h"
#include "usart.h"
#include "time.h"
#include "systick.h"
#include "time.h"

Car car;

SysTime tick_time;
SysTime pid_time;

void Parameter_Init(TIM_ZKB front_left_speed,TIM_ZKB front_right_speed);



static void Car_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	RCC_APB2PeriphClockCmd(WHEEL_RCC | HALL_RCC | RCC_APB2Periph_AFIO,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = WHEEL_FRONT_LEFT_1|WHEEL_FRONT_LEFT_2|WHEEL_FRONT_RIGHT_1|WHEEL_FRONT_RIGHT_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(WHEEL_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = HALL_LEFT | HALL_RIGHT;   //霍尔+码盘测速
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(HALL_PORT, &GPIO_InitStructure);			   
}


static void Init_PWM(void)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;					//定义一个通道输出结构

    TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//定义一个定时器结构体变量

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   	//使能定时器4，重要！！

	TIM_DeInit(TIM4);                              			//将IM2定时器初始化位复位值

	TIM_InternalClockConfig(TIM4); 							//配置 TIM4 内部时钟
	   
	TIM_BaseInitStructure.TIM_Period = 7200-1; 				//设置自动重载寄存器值为最大值	0~65535之间  1000000/1000=1000us=1ms													
															//TIM_Period（TIM1_ARR）=7200，计数器向上计数到7200后产生更新事件，
															//计数值归零 也就是 1MS产生更新事件一次
	TIM_BaseInitStructure.TIM_Prescaler = 0;  				//自定义预分频系数为0，即定时器的时钟频率为72M提供给定时器的时钟	0~65535之间
															//设置预分频器分频系数0
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割为0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM向上计数模式 从0开始向上计数，计数到1000后产生更新事件
	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure); 		//根据指定参数初始化TIM时间基数寄存器	
      
 	TIM_ARRPreloadConfig(TIM4, ENABLE);						//使能TIMx在 ARR 上的预装载寄存器 


	TIM_OCStructInit(&TIM_OCInitStructure);					//设置缺省值

	/*设置通道1*/

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	   	//PWM 模式 1 输出 	
	TIM_OCInitStructure.TIM_Pulse = 3600; 	//设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
															//PWM的输出频率为Fpwm=72M/7200=1Mhz；  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
															//TIM 输出比较极性高   	    
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
															//使能输出状态  需要PWM输出才需要这行代码
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);				//根据参数初始化PWM寄存器    TIM4_CH1

	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);	   	//使能 TIMx在 CCR2 上的预装载寄存器


	/*设置通道2*/																													//使能输出状态  需要PWM输出才需要这行代码
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);				//根据参数初始化PWM寄存器    TIM4_CH1
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);	   	//使能 TIMx在 CCR2 上的预装载寄存器


	/*设置通道3*/																													//使能输出状态  需要PWM输出才需要这行代码
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);				//根据参数初始化PWM寄存器    TIM4_CH1
	TIM_OCInitStructure.TIM_Pulse = 3600;
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);	   	//使能 TIMx在 CCR2 上的预装载寄存器


	/*设置通道4*/																													//使能输出状态  需要PWM输出才需要这行代码
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);				//根据参数初始化PWM寄存器    TIM4_CH1
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);	   	//使能 TIMx在 CCR2 上的预装载寄存器


	TIM_Cmd(TIM4, ENABLE); 		//TIM4总开关：开启 

    TIM_CtrlPWMOutputs(TIM4,ENABLE);  						//设置TIM4 的PWM 输出为使能  
}


static void Wheel_Hall_IO_Check(Wheel* w)
{
    w->cur_io_state = GPIO_ReadInputDataBit(HALL_PORT,HALL_LEFT);

	if(w->pre_io_state != w->cur_io_state)
	{
		w->count ++;
		w->pre_io_state = w->cur_io_state;
	}
}


static float Wheel_Hall_CaculateSpeed(Wheel* w,float k)  
{
	float speed = 2*PI*(w->radio)*((float)(w->count) * k/( (w->encoder_holes*2-1) ));

	w->circles += ((float)(w->count)/(w->encoder_holes*2-1));
//	printf("w->count : %d  w->circles ： %f\r\n",w->count,w->circles);
	w->count = 0;

	return speed;
}

float Wheel_PwmToSpeed(uint16_t pwm)
{
    if(pwm < MIN_DUTY)	 return 0;

	return ((pwm - MIN_DUTY)/360*0.05 + MIN_SPEED); 
}

uint16_t Wheel_SpeedToPwm(float speed)
{
    if( speed < MIN_SPEED ) return MIN_DUTY;

	return ((speed - MIN_SPEED)*20*360 + MIN_DUTY);
}

void Wheel_SpeedInc(Wheel* w,float speed)
{
    if( w->speed + speed > MAX_SPEED )
	{
	    w->speed = MAX_SPEED;
	}
	else
	{
	    w->speed += speed; 
	}

	w->pid.set_speed = w->speed;

	Wheel_SetDirectionAndSpeed(w,w->direction,Wheel_SpeedToPwm(w->speed));

}


void Wheel_SpeedDec(Wheel* w,float speed)
{
	if( w->speed - speed < MIN_SPEED )
	{
	   	w->speed = MIN_SPEED;
	}
	else
	{
	    w->speed -= speed;
	}

	w->pid.set_speed = w->speed;

	Wheel_SetDirectionAndSpeed(w,w->direction,Wheel_SpeedToPwm(w->speed));

}


void Car_SpeedInc(Car* car,float speed)
{
   	Wheel_SpeedInc(&car->wheel[0],speed);

    PID_ParameterInit(&car->wheel[0].pid,car->wheel[0].speed);

	Wheel_SpeedInc(&car->wheel[1],speed);
	PID_ParameterInit(&car->wheel[1].pid,car->wheel[1].speed);

}

void Car_SpeedDec(Car* car,float speed)
{
   	Wheel_SpeedDec(&car->wheel[0],speed);
	PID_ParameterInit(&car->wheel[0].pid,car->wheel[0].speed);

	Wheel_SpeedDec(&car->wheel[1],speed);
	PID_ParameterInit(&car->wheel[1].pid,car->wheel[1].speed);
}

void Wheel_SetDirectionAndSpeed(Wheel* w,Wheel_Direction direction,uint16_t speed)
{
    w->speed = Wheel_PwmToSpeed(speed);

    if(w->id == FRONT_LEFT)	 //左前
	{
	   if(direction == FORWARD)	  //向前
	   {
	       	TIM_SetCompare1(TIM4,speed);
			TIM_SetCompare2(TIM4,0);
	   }
	   else
	   {
	        TIM_SetCompare1(TIM4,0);
			TIM_SetCompare2(TIM4,speed);
	   }
	}
	else if(w->id == FRONT_RIGHT) //右前
	{
	   if(direction == FORWARD)	  //向前
	   {
	       	TIM_SetCompare3(TIM4,speed);
			TIM_SetCompare4(TIM4,0);
	   }
	   else
	   {
	        TIM_SetCompare3(TIM4,0);
			TIM_SetCompare4(TIM4,speed);
	   }
	}
}




void Car_Go(void)	//向前
{

	 Wheel_SetDirectionAndSpeed(&wheel_front_left,FORWARD,Wheel_SpeedToPwm(wheel_front_left.speed));
	 Wheel_SetDirectionAndSpeed(&wheel_front_right,FORWARD,Wheel_SpeedToPwm(wheel_front_right.speed));

}  


void Car_Back(void) //向后
{
	 Wheel_SetDirectionAndSpeed(&wheel_front_left,BACKWARD,Wheel_SpeedToPwm(wheel_front_left.speed));
	 Wheel_SetDirectionAndSpeed(&wheel_front_right,BACKWARD,Wheel_SpeedToPwm(wheel_front_right.speed));
}

void Car_Left(void)//向左
{
	Wheel_SetDirectionAndSpeed(&wheel_front_left,BACKWARD,Dutyfactor_35);
	Wheel_SetDirectionAndSpeed(&wheel_front_right,FORWARD,Dutyfactor_50);
}

void Car_Right(void) //向右
{
	Wheel_SetDirectionAndSpeed(&wheel_front_left,FORWARD,Dutyfactor_50);
	Wheel_SetDirectionAndSpeed(&wheel_front_right,BACKWARD,Dutyfactor_30);
}

void Car_Stop(void)  //停止
{
	Wheel_SetDirectionAndSpeed(&wheel_front_left,STOP,Dutyfactor_0);
	GPIO_ResetBits(wheel_front_left.port,wheel_front_left.pin[0]);
	GPIO_ResetBits(wheel_front_left.port,wheel_front_left.pin[1]);

	Wheel_SetDirectionAndSpeed(&wheel_front_right,STOP,Dutyfactor_0);
	GPIO_ResetBits(wheel_front_right.port,wheel_front_right.pin[0]);
	GPIO_ResetBits(wheel_front_right.port,wheel_front_right.pin[1]);	
}

void Wheel_PidEnable(Wheel* w,uint8_t enable)
{
     w->pid_enable = enable;
}

static void Wheel_ParameterInit(Wheel* w,WHEEL_ID id,GPIO_TypeDef* port,uint16_t pin1,uint16_t pin2,Wheel_Direction direction,TIM_ZKB speed)
{
     w->id = id;
	 w->port = port;
	 w->pin[0] = pin1;
	 w->pin[1] = pin2;
	 w->direction = direction;
	 w->speed = Wheel_PwmToSpeed(speed);

	 w->real_speed = speed;
	 w->pre_io_state = w->cur_io_state = 0;
	 w->count = 0;

	 w->encoder_holes = WHEEL_ENCODER_HOLES; 
	 w->radio = WHEEL_RADIO;
	 w->distance = 0;

	 w->circles = 0;
	 
	 w->pid_time = PID_TIME;

	 w->pid_enable = 1;

}  


static void Parameter_Init(TIM_ZKB front_left_speed,TIM_ZKB front_right_speed)
{
   	Wheel_ParameterInit(&wheel_front_left,FRONT_LEFT,GPIOA,WHEEL_FRONT_LEFT_1,WHEEL_FRONT_LEFT_2,FORWARD,front_left_speed);
	Wheel_ParameterInit(&wheel_front_right,FRONT_RIGHT,GPIOA,WHEEL_FRONT_RIGHT_1,WHEEL_FRONT_RIGHT_2,FORWARD,front_right_speed);

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

	Init_PWM();
	Parameter_Init(Dutyfactor_40,Dutyfactor_40);

	Car_Go();

	UserTime_Reset(&pid_time);
}

void Car_RunCtl(void)
{
    float next_left_speed = 0,next_right_speed = 0;

	Wheel_Hall_IO_Check(&wheel_front_left);
	Wheel_Hall_IO_Check(&wheel_front_right);

    if(T_1ms(pid_time,200))  //定时1ms
	{
	   UserTime_Reset(&pid_time); 

	   if( !(wheel_front_left.direction == STOP && wheel_front_right.direction == STOP) )
	   {
	       if( wheel_front_left.count > 0)
		   {
		   		wheel_front_left.real_speed = Wheel_Hall_CaculateSpeed(&wheel_front_left,5);

				if(wheel_front_left.pid_enable)
				{
					next_left_speed = PID_SpeedAjust(&wheel_front_left.pid,wheel_front_left.real_speed);
					//Wheel_SetDirectionAndSpeed(&wheel_front_left,wheel_front_left.direction,Wheel_SpeedToPwm(next_left_speed));
				}


				wheel_front_left.distance += wheel_front_left.real_speed*0.2;

				printf("distancce : %0.4f\r\n",Car_GetRunDistance());
		   }

		   if( wheel_front_right.count >0 )
		   {
		   		wheel_front_right.real_speed = Wheel_Hall_CaculateSpeed(&wheel_front_right,5);

				if( wheel_front_right.pid_enable )
				{
					next_right_speed = PID_SpeedAjust(&wheel_front_right.pid,wheel_front_right.real_speed);
					//Wheel_SetDirectionAndSpeed(&wheel_front_right,wheel_front_right.direction,Wheel_SpeedToPwm(next_right_speed));
				}
		   }
	
		   printf("left_speed : %0.2f  right_speed : %0.2f\r\n",wheel_front_left.real_speed,wheel_front_right.real_speed);
//		   printf("next_left_speed : %0.2f  next_right_speed : %0.2f\r\n",next_left_speed,next_right_speed);
	   }

	}  

}
