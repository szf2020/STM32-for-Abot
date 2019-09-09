/**
 *@title Abot Firmware
 * Copyright: Copyright (c) 2019 Abot [https://github.com/tloinny/STM32-for-Abot]
 *
 *@date on 2019-1-08  
 *@author:tony-lin
 *@version 1.0.0 
 * 
 *@description: Abot slave firmware
 */
 
#include "sys_conf.h"

/* ����˶���Ϣ�Ķ�д����ʹ���ˡ�������������ģʽ�� */
/* ������������Ķ���������������ģʽ��Ҫʹ�õ����� */
motion_info motion_buf[motion_buf_size];	/* ����˶���Ϣ������ */
u8 motion_buf_full = 0;
u16 empty_flag = motion_buf_size;	/* ��λ����ʼֵΪmotion_buf_size */
u16 full_flag = 0;		/* ��λ����ʼֵΪ0 */
int product_count = 1;	/* motion_info �����߼��� */
int consum_count = 1;	/* motion_info �����߼��� */
float delta_rad = 0;

u8 key = 0;	/* CAN���շ��ر�־λ */

int main(void)
{
	/* ��ʼ���ṹ������ĵ�һλ */
	motion_buf[0].rad = 0;
	motion_buf[0].dir = 0;
	motion_buf[0].speed_max = 0;
	
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	LED_Init();
	LED0 = 0;
	motor_init(DMA1_Channel6, (u32)&TIM3->ARR, (u32)send_buf, send_buf_size,arr_init, psc_init);	/* ��ʼ����� */
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);	/* ��ʼ��CAN���� */
	EXTIX_Init();	/* ��ʼ���ⲿ�ж� */
		while(1)
		{
			DelayForRespond	/* CPU���𣬵ȴ������Ļظ� */
			key = Can_Receive_Msg(can_rec_buf);		/* ����CAN������Ϣ */
			if(key != 0 && *(can_rec_buf+3) == 0 && empty_flag != 0 && full_flag < motion_buf_size)	/* ������յ�����Ϣ����������Ϣ���������������ߵ��������� */
			{
				/* ��������Ϊ */
				
				motion_buf[product_count].rad = *(can_rec_buf+1)*254+*(can_rec_buf);
				(motion_buf[product_count].rad-motion_buf[product_count - 1].rad)>0 ? (motion_buf[product_count].dir = 1) : (motion_buf[product_count].dir = 0);	/* �ж��˶����� */
				motion_buf[product_count].speed_max = *(can_rec_buf+5)*254+*(can_rec_buf+4); /* �����˶��ٶ� */
				motion_buf[product_count].state = *(can_rec_buf+7);
				
				(product_count == motion_buf_size)?(product_count = 1):(++product_count);	/* �����߼��� */
				
				--empty_flag;	/* ��ȡһ����λ */
				++full_flag;	/* �ͷ�һ����λ */
				CAN_send_feedback(c_motion_request);	/* �Ѿ����յ�һ���˶���Ϣ�������뻺����������������һ���˶����� */
			}else	if(key != 0 && *(can_rec_buf+3) != 0)/* ������յ�����Ϣ��������Ϣ */
			{
				switch(*(can_rec_buf+3))	/* ƥ������ */
				{
					case C_READY:	/* READY���Ԥ�����ú�send_buf���ȴ�ACTION���� */
						if(empty_flag < motion_buf_size && full_flag != 0 && MotorStatus() != m_moving && zeroed != 0)	/* ���˶���Ϣ���������ö��ҵ�������˶�״̬ʱ��������send_buf */
						{ 
							/* ��������Ϊ */
							
							/* ��������һ��λ�õ�deltaֵ���������õ���˶����� */
							delta_rad = fabs((motion_buf[consum_count].rad - motion_buf[consum_count-1].rad)/1000);
							
							#if SLAVE0||SLAVE1||SLAVE2
							motor_point_movement_ready(motor_type*Micro_Step*ratio*(delta_rad/pi/2), motion_buf[consum_count].dir, motor_type*Micro_Step*ratio*(delta_rad/pi/2)*0.003*pi, 0.1*pi, 0.5, send_buf);
							#endif
							
							#if SLAVE3
							motor_point_movement_ready(motor_type*Micro_Step*ratio*(delta_rad/pi/2), motion_buf[consum_count].dir, motor_type*Micro_Step*ratio*(delta_rad/pi/2)*0.006*pi, 1*pi, 2, send_buf);
							#endif
							/* ����������һλ������ */
							if(consum_count - 1 > 0)
							{
								motion_buf[consum_count - 1].rad = 0;	
								motion_buf[consum_count - 1].dir = 0;
								motion_buf[consum_count - 1].speed_max = 0;	
							}
							
							(consum_count == motion_buf_size)?(consum_count = 1):(++consum_count);	/* �����߼��� */
							
							++empty_flag;	/* �ͷ�һ����λ */
							--full_flag;	/* ��ȡһ����λ */
						}
						CAN_send_feedback(c_motor_ready);	/* ֪ͨ�����Ѿ����һ��׼�����������Խ���ACTION������ */
						break;
					case C_ACTION:	/* ACTION�������DMA�Ͷ�ʱ�����������send_buf���������� */
						if(motor_run()==1)
						{
							CAN_send_feedback(c_motor_action);	/* ֪ͨ�����Ѿ���ʼһ��ACTION */
						}
						break;
					case C_STOP:	/* STOP����������ֹͣ�˶� */
						motor_stop();
						CAN_send_feedback(c_motor_stop);	/* ֪ͨ��������Ѿ�ֹͣ */
						break;
					case C_HOME:	/* HOME��������λ */
						motor_home();
						CAN_send_feedback(c_motor_home);	/* ֪ͨ��������Ѿ����ﵽԭ�� */
						break;
					case C_MOTOR_DISABLE:	/* DISABLE������ʧ�� */
						motor_disable();
						CAN_send_feedback(c_motor_disable);	/* ֪ͨ��������Ѿ�ʧ�� */
						break;
					case C_MOTOR_ENABLE:	/* ENABLE������ʹ�� */
						motor_enable();
						CAN_send_feedback(c_motor_enable);	/* ֪ͨ��������Ѿ�ʹ�� */
						break;
					case C_CALL:					/* CALL������������ĺ��� */
						CAN_send_feedback(c_receive_call);	/* ��Ӧ�������У�֪ͨ�������ڵ���� */
						break;
				}
			}else if(key == 0 && empty_flag < motion_buf_size && full_flag != 0 && MotorStatus() != m_moving)	/* �������û�з�����Ϣ����������Ǵӻ��Ļ������������˶���Ϣ��δִ�У��������������˶����� */
			{
				CAN_send_feedback(c_motion_request);
			}
			if(empty_flag == 0 && motion_buf_full == 0)	/* ���������û�п�λ������Ϊmotion_buf������֪ͨ������Ҫ�ٷ�������Ϣ�ݴ��������ڴ��� */
			{
				CAN_send_feedback(c_buf_full);
				motion_buf_full = 1;
			}else if(empty_flag > 50 && motion_buf_full == 1)	/* ����������д���50����λ������Ϊmotion_buf���ã�֪ͨ�������Լ������� */
			{
				CAN_send_feedback(c_buf_usefull);
				motion_buf_full = 0;				
			}
			MotorStatus();	/* ���Ը��µ��״̬ */
		}
}

