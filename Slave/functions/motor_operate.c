#include "sys_conf.h"

extern u8 home_flag;

const float step_angle = 2 * pi/(motor_type*Micro_Step);
const float timer_frep = 72000000/(psc_init+1);
u8 Motor_status = 0;
int current_position;
u16 send_buf[send_buf_size];

/**
 *@function �����ʼ��
 *���Ӳ����ʼ��,��Ϊʵ�ֵ�����Ƽ���ʹ�õ���Դ���г�ʼ��
 *@param	
 *				DMA_CHx:�������ռ�õ�DMAͨ��
 *				cpar:�������ʹ�õĶ�ʱ��ARR�����ַ
 *				cmar:�洢����ַ
 *				cndtr:���ݴ�����
 *				arr:��ʱ���ĳ�ʼ�����װ��ֵ
 *				psc:��ʱ���ķ�Ƶ��
 *@return
 */
void motor_init(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr, u16 arr, u16 psc)
{
	motor_io_init();	/* ��ʼ��IO */
	DMA_Config(DMA_CHx, cpar, cmar, cndtr);	/* ��ʼ��DMA */
	TIM3_PWM_Init(arr, psc);	/* ��ʼ����ʱ�� */
	motor_enable();					/* ʹ�ܵ�� */
	Motor_status = m_stop;	/* ��ʼ�����״̬Ϊֹͣ״̬ */
}

/**
 *@function ���ʹ��
 *@param void
 *@return void
 */
void motor_enable()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);	
}

/**
 *@function ���ʧ��
 *@param void
 *@return void
 */
void motor_disable()
{
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}

/**
 *@function ���ת��
 *@param 
*				dir: 0��1,ӳ��Ϊ����˶�����
 *@return void
 */
void motor_dir(u8 dir)
{
	dir ? GPIO_SetBits(GPIOB, GPIO_Pin_3) : GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}

/**
 *@function ����˶�����
 *@param 
 *				steps:��������˶��Ĳ���
 *				dir:��������˶��ķ���
 *				speed_max:Ŀ���ٶ�
 *				speed_init:��ʼ�ٶ�
 *				acc_accel:���ٶ�
 *				acc_decel:���ٶ�
 *				S_buf:������������
 *@return void
 */
void motor_move_ready(float steps, u8 dir, float speed_max, float speed_init, float acc_accel, float acc_decel, u16 * S_buf)
{
	u16 max_steps_lim = (u16)ceil(((speed_max+speed_init)*(speed_max-speed_init))/ (2*step_angle*acc_accel));	/* the number of steps needed to accelerate to the desired speed */
	u16 acc_lim = (u16)ceil(steps * (acc_decel/(acc_accel+acc_decel)));		/* the number of steps before deceleration starts */
	u16 arr_max = (u16)(step_angle* timer_frep /speed_max);
	u16 accel_steps = 0;
	u16 *decel_steps = (u16*)malloc(sizeof(u16));
	u16 *temp = (u16*)malloc(sizeof(u16));
	u16 const_steps = 0;
	u16 i = 0;
	float compensation = 0;
		if(max_steps_lim <= acc_lim)	/* ������Լ��ٵ�����ٶ� */
		{
			accel_steps = max_steps_lim;
			*decel_steps = accel_steps * (acc_accel/acc_decel);
			const_steps = steps - (accel_steps + *decel_steps); 
			*temp = accel_steps + const_steps;
			free(decel_steps);
		}else	/* ����޷����ٵ�����ٶ� */
		{
			accel_steps = acc_lim;
			free(decel_steps);
		}
		while(!(i > steps))
		{
			if(i == 0) *(S_buf + i) = (u16)(timer_frep* sqrt(step_angle/acc_accel) * 0.676);
			if(i > 0 && i < accel_steps)	/* ���ٶ� */
			{
				*temp = (u16)ceil(*(S_buf + i - 1) - ((*(S_buf + i - 1) * 2 + compensation) / (4 * i - 1)));
				if(*temp != 0)
				*(S_buf + i) = (*temp);
				else *(S_buf + i) = 2;
				compensation = (float)fmod( *(S_buf + i - 1) * 2 + compensation, 4 * i - 1);	/* ���²��� */
			}
			if(i >= accel_steps && i < accel_steps + const_steps)	/* ���ٶ� */
			{
				*(S_buf + i) = arr_max;
			}
			if(i >= accel_steps + const_steps && i < steps)	/* ���ٶ� */
			{
				*temp = (u16)ceil (*(S_buf + (int)steps - i - 1) - (( *(S_buf + (int)steps - i - 1) * 2 + compensation) / (4 * i - 1)));
				if(*temp != 0)
				*(S_buf + i) = (*temp);
				else *(S_buf + i) = 2;
				compensation = (float)fmod( *(S_buf + (int)steps - i - 1) * 2 + compensation, 4 * i - 1);	/* ���²��� */	
			}
			if (i == steps)
			{
				*(S_buf + i) = 0 ;	/* ���� */
				free(temp);
			}
			++i;
		}
		motor_dir(dir);	/* ���õ���˶����� */
		DMA_SetCurrDataCounter(DMA1_Channel6,(u16)steps + 1);	/* ��ǰ����DMA�ķ���λ����������ʱ��ʹ��DMA */
		//DMA_Enable(DMA1_Channel6,(u16)steps + 1);
}

/**
 *@function �������
 *@param void
 *@return 
 * 				1:�ɹ�����
 *				0:DMAæµ
 */
u8 motor_run()
{
	if(Motor_status == m_stop || Motor_status == m_waiting)	/* ���DMA�Ѿ��������������ݣ�����Կ�ʼ��һ�η��� */
	{
		TIM3->ARR = 2;	/* �������һ����0������������ʱ��ARR�ᱻ���㣬������һ��������Ч��*/
		DMA_Cmd(DMA1_Channel6, ENABLE);
		TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
		TIM3->EGR = 0x00000001;
		Motor_status = m_moving;
		return 1;
	}
	return 0;
}

/**
 *@function ���ֹͣ
 *@param void
 *@return void
 */
void motor_stop()
{
	TIM_Cmd(TIM3,DISABLE);
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA1_Channel6->CNDTR = 0;
	Motor_status = m_stop;
}

/**
 *@function �������
 *@param void
 *@return void
 */
void motor_restart()
{
	DMA_Enable(DMA1_Channel6, send_buf_size);
	home_flag = 0;
}

/**
 *@function �����λ
 *@param void
 *@return void
 */
void motor_home()
{
	while(home_flag == 0)	/* ����λ����û�б����� */
	{
		/* ����ؽ�ԭ�㿿�� */
		motor_move_ready(1, 0, pi, pi, 1, 1, send_buf);
		motor_run();
		while(isMotorStatus() != m_moving);	/* ���������Ǵ����˶�״̬������Լ����������� */
	}
	/* ��λ���ر����������ֹͣ */
	current_position = 0;
	motor_stop();
}

/**
 *@function �жϵ��������״̬
 *@param void
 *@return 
 * 				m_moving 	0x01
 * 				m_stop		0x02
 * 				m_waiting 0x03
 */
u8 isMotorStatus()
{
	if(DMA_send_feedback(DMA1_Channel6) == 0 && Motor_status == m_moving)	/* ���DMA�Ѿ����������ݣ����ҵ����Ȼ��������״̬ */
	{
		Motor_status = m_stop;	/* ���״̬�л���ֹͣ */
		return m_stop;	/* ��Ϊ����ոյ���ָ��λ�� */
	}
	if(DMA_send_feedback(DMA1_Channel6) == 0 && Motor_status == m_stop)
	{
		Motor_status = m_waiting;
		return m_waiting;	/* ��Ϊ������ѵ���ָ��λ�� */
	}
	return m_moving;	/* ������Ϊ�����δ����ָ��λ�� */
}
