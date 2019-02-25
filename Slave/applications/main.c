#include "sys_conf.h"

typedef struct motion_info
{
	float rad;
	u8 dir;
	float speed_max;
} motion_info;

motion_info motion_buf[motion_buf_size];	/* ����˶����������� */
extern u16 send_buf[send_buf_size]; /* ���巢�ͻ����� */ 

int key = 0;
int product_count = 1;	/* motion_info �����߼��� */
int consum_count = 1;	/* motion_info �����߼��� */
extern int current_position;
float delta_rad = 0; 
	
u8 empty_flag = motion_buf_size;	/* ��λ����ʼֵΪmotion_buf_size */
u8 full_flag = 0;		/* ��λ����ʼֵΪ0 */

u8 rec_buf[8] = {0};
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
	EXTIX_Init();
		while(1)
		{
			key = Can_Receive_Msg(rec_buf);		/* ����CAN������Ϣ */
			if(key != 0 && *(rec_buf+3) == 0 && empty_flag != 0 && full_flag < motion_buf_size)	/* ������յ�����Ϣ����������Ϣ���������������ߵ��������� */
			{
				/* ��������Ϊ */
				motion_buf[product_count].rad = *(rec_buf+1)*254+*(rec_buf);	/* ����ؽ�ת�ǻ���ֵ */
				((motion_buf[product_count].rad-motion_buf[product_count - 1].rad)>0)?(motion_buf[product_count].dir = 1):(motion_buf[product_count].dir = 0);	/* �ж��˶����� */
				motion_buf[product_count].speed_max = *(rec_buf+5)*254+*(rec_buf+4); /* �����˶��ٶ� */
				printf("-----------\r\n");
				printf("rad:%f\r\n",motion_buf[product_count].rad);
				printf("dir:%d\r\n",motion_buf[product_count].dir);
				printf("speed:%f\r\n",motion_buf[product_count].speed_max);
				(product_count == motion_buf_size)?(product_count = 1):(++product_count);
				--empty_flag;	/* ��ȡһ����λ */
				++full_flag;	/* �ͷ�һ����λ */
			}else	if(key != 0 && *(rec_buf+3) != 0)/* ������յ�����Ϣ��������Ϣ */
			{
				switch(*(rec_buf+3))	/* ƥ������ */
				{
					case C_READY:	/* READY���Ԥ�����ú�send_buf���ȴ�ACTION���� */
						printf("recieve r \r\n");
						if(empty_flag < motion_buf_size && full_flag != 0)
						{
							/* ��������Ϊ */
							
							/* ��������һ��λ�õ�deltaֵ */
							delta_rad = (motion_buf[consum_count].rad - motion_buf[consum_count-1].rad)/1000;
							motor_move_ready(motor_type*Micro_Step*ratio*delta_rad, motion_buf[consum_count].dir, motion_buf[consum_count].speed_max, pi, 5, 5, send_buf);
							/* ����������һλ������ */
							if(consum_count - 1 > 0)
							{
								motion_buf[consum_count - 1].rad = 0;	
								motion_buf[consum_count - 1].dir = 0;
								motion_buf[consum_count - 1].speed_max = 0;	
							}
							
							(consum_count == motion_buf_size)?(consum_count = 1):(++consum_count);
							++empty_flag;	/* �ͷ�һ����λ */
							--full_flag;	/* ��ȡһ����λ */
						}
						CAN_send_feedback(c_motor_ready);	/* ֪ͨ�����Ѿ����һ��׼�����������Խ���ACTION������ */
						break;
					case C_ACTION:	/* ACTION�������DMA�Ͷ�ʱ����������̸���send_buf���������� */
						printf("recieve a\r\n");
						if(motor_run())
						CAN_send_feedback(c_motor_action);	/* ֪ͨ�����Ѿ���ʼһ��ACTION */
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
			}
			if(isMotorStatus() == m_stop)
			{
				CAN_send_feedback(c_motor_arrive);	/* ֪ͨ��������Ѿ�����ָ��λ�� */
			}
		}
}

