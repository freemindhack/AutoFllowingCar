#include "car.h"

uint8_t tick_1ms;
uint16_t speed_1ms;

Car car;

static void Car_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//����һ��GPIO�ṹ�����

	RCC_APB2PeriphClockCmd(WHEEL_RCC,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = WHEEL_FRONT_LEFT_1|WHEEL_FRONT_LEFT_2|WHEEL_FRONT_RIGHT_1|WHEEL_FRONT_RIGHT_2|WHEEL_REAR_LEFT_1|WHEEL_REAR_LEFT_2|WHEEL_REAR_RIGHT_1|WHEEL_REAR_RIGHT_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(WHEEL_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = HALL_LEFT | HALL_RIGHT;   //����+���̲���
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	   		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
  	GPIO_Init(HALL_PORT, &GPIO_InitStructure);			   
}

static void Init_TIMER(u16 Period)  //���ﶨʱʱ����period*1ms
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = Period; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������N*10ΪNms
	TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM3
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	/*�����ж����ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����							 
}


void Hall_IO_Check(Car_Speed* ws)
{
    ws->cur_left_io_state = GPIO_ReadInputDataBit(WHEEL_PORT,HALL_LEFT);

	if(ws->last_left_io_state != ws->cur_left_io_state)
	{
		ws->left_count ++;
	}

	ws->cur_right_io_state = GPIO_ReadInputDataBit(WHEEL_PORT,HALL_RIGHT);

	if(ws->last_right_io_state != ws->cur_right_io_state)
	{
		ws->right_count ++;
	}
}

void Hall_CaculateSpeed(void)
{
    
}

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //���TIMx���жϱ�־λ

		tick_1ms++;

        		
		Hall_IO_Check(&car_speed);

		if(tick_1ms > 100)
		{
		    tick_1ms = 0;

		
		}

		Car_Move();

	}

}

void Wheel_PWMEmulate(Wheel* w,uint8_t duty)
{
    if(w->direction == STOP) return;

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

void Car_Go(void)	//��ǰ
{
	 Wheel_SetDirection(&wheel_front_left,FORWARD);

	 Wheel_SetDirection(&wheel_front_right,FORWARD);

	 Wheel_SetDirection(&wheel_rear_left,FORWARD);

	 Wheel_SetDirection(&wheel_rear_right,FORWARD);
}

void Car_Back(void) //���
{
	 Wheel_SetDirection(&wheel_front_left,BACKWARD);

	 Wheel_SetDirection(&wheel_front_right,BACKWARD);

	 Wheel_SetDirection(&wheel_rear_left,BACKWARD);

	 Wheel_SetDirection(&wheel_rear_right,BACKWARD);
}

void Car_Left(void)//����
{
	Wheel_SetDirection(&wheel_front_left,BACKWARD);
	Wheel_SetDirection(&wheel_rear_left,BACKWARD);

	Wheel_SetDirection(&wheel_front_right,FORWARD);

	Wheel_SpeedInc(&wheel_rear_right,30);
	Wheel_SetDirection(&wheel_rear_right,FORWARD);
}


void Car_Right(void) //����
{
	Wheel_SpeedDec(&wheel_front_right,0);
	Wheel_SetDirection(&wheel_front_right,BACKWARD);
	Wheel_SpeedDec(&wheel_rear_right,0);
	Wheel_SetDirection(&wheel_rear_right,BACKWARD);

	Wheel_SetDirection(&wheel_front_left,FORWARD);

	Wheel_SpeedInc(&wheel_rear_right,50);
	Wheel_SetDirection(&wheel_rear_left,FORWARD);
}

void Car_Stop(void)  //ֹͣ
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
}

void Parameter_Init(void)
{
   	Wheel_ParameterInit(&wheel_front_left,GPIOA,WHEEL_FRONT_LEFT_1,WHEEL_FRONT_LEFT_2,FORWARD,20);
	Wheel_ParameterInit(&wheel_front_right,GPIOA,WHEEL_FRONT_RIGHT_1,WHEEL_FRONT_RIGHT_2,FORWARD,20);
	Wheel_ParameterInit(&wheel_rear_left,GPIOA,WHEEL_REAR_LEFT_1,WHEEL_REAR_LEFT_2,FORWARD,20);
	Wheel_ParameterInit(&wheel_rear_right,GPIOA,WHEEL_REAR_RIGHT_1,WHEEL_REAR_RIGHT_2,FORWARD,20);

    car_speed.last_left_io_state = 0;
	car_speed.last_right_io_state = 0;

	car_speed.left_speed = 0;
	car_speed.right_speed = 0;

}

void Car_Init(void)
{
    Car_GPIO_Init();
	Parameter_Init();

	Car_Go();
	Init_TIMER(1);

}
