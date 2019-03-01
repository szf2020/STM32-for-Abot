/**
 *@title Abot Firmware
 * Copyright: Copyright (c) 2019 Abot [https://github.com/tloinny/STM32-for-Abot]
 *
 *@created on 2019-1-08  
 *@author:tony-lin
 *@version 1.0.0 
 * 
 *@description: Abot master firmware
 */
 
#include "sys_conf.h"

u8 usart_buf[usart_buf_size][14];
u16 empty_flag = usart_buf_size;	/* ��λ����ʼֵΪmotion_buf_size */
u16 full_flag = 0;		/* ��λ����ʼֵΪ0 */
int product_count = 0;	/* �����߼��� */
int consum_count = 0;	/* �����߼��� */

int main(void)
{
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	DEBUG_USARTx_DMA_Config();
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);
	LED0 = 1;
	delay_ms(1000);
	CAN_Call();	/* CAN�㲥һ�Σ��鿴����������Щ�ڵ� */
	delay_ms(1000);
	home_all();	/* �������йؽ�Ѱ��ԭ�� */
	
	while(1)
		{
			if(DEBUG_Receive_length > 0) /* ������һ֡����,�������ݷַ� */
			{
				if(DEBUG_Rx_Buff[0]==255 && DEBUG_Rx_Buff[DEBUG_Receive_length-1]==255 && DEBUG_Receive_length == 14)
				{
					if(slave_buf_available == 1 && empty_flag == usart_buf_size && full_flag == 0)	/* ����ӻ��Ļ��������ã������������������Ѿ�û�����ݣ������ֱ�ӽ������ݷַ� */
					{
						CAN_distribute(DEBUG_Rx_Buff,DEBUG_Receive_length);
					}else if(slave_buf_available == 0 && empty_flag != 0 && full_flag <usart_buf_size)	/* ����ӻ��Ļ����������ã������������������ã�����Ҫ�����������湦�� */
					{
						int i;
						for(i=0;i<DEBUG_Receive_length;++i)
						{
							usart_buf[product_count][i] = DEBUG_Rx_Buff[i];
						}
						(product_count == usart_buf_size)?(product_count = 0):(++product_count);	/* �����߼��� */
						--empty_flag;	/* ��ȡһ����λ */
						++full_flag;	/* �ͷ�һ����λ */
					}
				}				
				DEBUG_Receive_length = 0;
				DEBUG_RX_Start;	/* ������һ�ν��� */
			}
			
			if(slave_buf_available == 1 && empty_flag != usart_buf_size && full_flag != 0 )
			{
				CAN_distribute(usart_buf[consum_count],14);
				
				(consum_count == usart_buf_size)?(consum_count = 1):(++consum_count);	/* �����߼��� */
							
				++empty_flag;	/* �ͷ�һ����λ */
				--full_flag;	/* ��ȡһ����λ */				
			}
			
			/* CPU���𣬵ȴ������ϵ����� */
			DelayForRespond
			
			/* ����յ����Դӻ��ķ��أ�����з���ƥ�� */
			if(Can_Receive_Msg(can_rec_buf) != 0)
			{
				match_feedback(can_rec_buf);				
			}
		}
}

