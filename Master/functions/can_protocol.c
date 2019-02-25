#include "sys_conf.h"

u8 can_buf[CAN_buf_size] = {0};
u8 can_rec_buf[CAN_buf_size] = {0};
u32 slave[slave_num_max] = {slave_0,slave_1,slave_2,slave_3,slave_4,slave_5};
u8 slave_num = 0;

/**
 *@function CAN��ӻ������ٶ���Ϣ�ͻ����ƵĽǶ���Ϣ
 *@param 
 *			rad:�Ƕ���Ϣ
 *			speed:�ٶ���Ϣ	
 *				ID:�ӻ���ַ	��:slave_0|slave_1,��ʾ���͸��ӻ�0�ʹӻ�1
 *@return 
 *				0----�ɹ�
 *				����----ʧ��
 */
u8 CAN_send_motion_info(float rad, float speed, u32 ID)
{
	u8 result = 0;
	u16 rad_temp = (u16)(rad * 1000);	/* ����1000���������ȷ��0.001 */
	u16 speed_temp = (u16)(speed * 10);
	*(can_buf) = (u8)(rad_temp%254);
	*(can_buf + 1) = (u8)(rad_temp/254);
	*(can_buf + 3) = 0;	/* '/0'��Ϊ�ָ��� */
	*(can_buf + 4) = (u8)(speed_temp%254);
	*(can_buf + 5) = (u8)(speed_temp/254);
	result = Can_Send_Msg(can_buf, CAN_buf_size, ID);
	return result;
}

/**
 *@function CAN��ӻ�����ָ��
 *@param 
 *				cmd:����
 *				ID:�ӻ���ַ	��:slave_0|slave_1,��ʾ���͸��ӻ�0�ʹӻ�1
 *@return 
 *				0----�ɹ�
 *				����----ʧ��
 */
u8 CAN_send_cmd(u8 cmd, u32 ID)
{
	u16 i;
	u8 result;
	for(i = 0; i < CAN_buf_size; ++i) *(can_buf + i) = cmd;
	result = Can_Send_Msg(can_buf, CAN_buf_size, ID);
	clean_can_buf();
	return result;
}

/**
 *@function CAN��ӻ��������ݷַ�
 *@param 
 *				buf:����Դ
 *				len:����Դ����
 *@return 
 *				0----�ɹ�
 *				����----ʧ��
 */
u8 CAN_distribute(u8 * buf, u8 len)
{
	int i = 1;
	u8 result=0;
	for(;i<len-2;i+=3)	/* ������Ϊ3����buf */
	{
		*can_buf=*(buf+i);
		*(can_buf+1)=*(buf+i+1);
		*(can_buf+3)=0;
		*(can_buf+4)=*(buf+i+2);	/* ����buf����can_buf�ı�Ҫλ */
		if(slave[(i-1)/3] != 0)	/* ����ڵ���ڣ���ַ����� */
		{
			result += Can_Send_Msg(can_buf, CAN_buf_size, slave[(i-1)/3]);	/* �ַ����� */
			DEBUG_USART_DMA_Tx_Start(can_buf, CAN_buf_size);	/* ����λ������ */
		}
		DelayForRespond;
		clean_can_buf();
	}
	return result;
}

void CAN_Call()
{
	u32 time_out = 10;
	u8 temp_buf[8]={0};
	u32 count = 0;
	int i = 0;
	for(;i<slave_num_max;++i)
	{
		CAN_send_cmd(C_CALL,slave[i]);
		for(;(Can_Receive_Msg(temp_buf) == 0) && count < time_out; ++count,delay_ms(6));
		if(count >= time_out || !(temp_buf[0]== 'R'&&temp_buf[1]== 'C')) /* ����ȴ���ʱ���߷�����������Ϊ�ýڵ㲻���� */
		{
			slave[i] = 0;
		}else
		{
			++slave_num;
		}
	}
}

void clean_can_buf()
{
	int i = 0;
	for(;i<CAN_buf_size;++i)
	{
		can_buf[i] = 0;
	}
}

void clean_can_rec_buf()
{
	int i = 0;
	for(;i<CAN_buf_size;++i)
	{
		can_rec_buf[i] = 0;
	}
}

u8 home_all()
{
	u8 count;
	u8 i;
	u32 rec_history[slave_num_max] = {0};	/* ��¼��Щ�ڵ��Ѿ����͹���Ϣ,�����ظ����͵���� */
	u8 temp_buf[8]={0};
	CAN_send_cmd(C_HOME,slave_all);
	for(count=0;count<slave_num;)
	{
		if(Can_Receive_Msg(temp_buf))
		{
			if(temp_buf[0] == 'H' && slave[(temp_buf[1]-'0')] != 0)
			{
				if(rec_history[(temp_buf[1]-'0')] == 0)
				{
					rec_history[(temp_buf[1]-'0')] = 1;
					++count;
				}
			}	
		}			
	}
	for(i=0;i<slave_num_max;++i)
	{
		if(!(slave[i]*rec_history[i] == slave[i]))
		{
			clean_can_rec_buf();
			return 0;
		}
	}
	clean_can_rec_buf();
	return 1;
}
