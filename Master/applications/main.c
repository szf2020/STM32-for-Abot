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
				CAN_distribute(DEBUG_Rx_Buff, DEBUG_Receive_length);
				DEBUG_Receive_length = 0;
				DEBUG_RX_Start;	/* ������һ�ν��� */
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

