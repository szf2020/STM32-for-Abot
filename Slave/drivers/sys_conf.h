/**
 *@title Abot Firmware
 * Copyright: Copyright (c) 2019 Abot [https://github.com/tloinny/STM32-for-Abot]
 *
 *@created on 2019-1-08  
 *@author:tony-lin
 *@version 1.0.0 
 * 
 *@description: Abot slave firmware config file
 */
 
#ifndef _SYS_CONF_H_
#define _SYS_CONF_H_

/**
 *@description include  
 *�����ﶨ����������include��ͷ�ļ�
 */

#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "can.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "motor_gpio.h"
#include "motor_operate.h"
#include "can_protocol.h"
#include "exti.h"
#include "led.h"

/**
 *@description define 
 *�����������û������е�defineֵ
 */
 
/*
 *�������
 */
#define pi 3.141592654
 
/*
 *���ýڵ�
 */
#define SLAVE0 0
#define SLAVE1 0
#define SLAVE2 0
#define SLAVE3 1
 
/*
 *�ؽڲ�������
 */
#if SLAVE0
#define motor_type 200			/* ������ͣ�����200 pulse/r */
#define Micro_Step 2				/* ����ϸ���� ����1/2 */
#define ratio 10						/* ��е���ٱ� ����1/10 */
#define home_offset 0				/* ��λ���ز����� */
#endif
#if SLAVE1
#define motor_type 200			/* ������ͣ�����200 pulse/r */
#define Micro_Step 2				/* ����ϸ���� ����1/2 */
#define ratio 9							/* ��е���ٱ� ����1/9 */
#define home_offset 0				/* ��λ���ز����� */
#endif
#if SLAVE2
#define motor_type 200			/* ������ͣ�����200 pulse/r */
#define Micro_Step 2				/* ����ϸ���� ����1/2 */
#define ratio 16						/* ��е���ٱ� ����1/16 */
#define home_offset -1*pi		/* ��λ���ز����� */
#endif
#if SLAVE3
#define motor_type 400			/* ������ͣ�����400 pulse/r */
#define Micro_Step 2				/* ����ϸ���� ����1/2 */
#define ratio 12						/* ��е���ٱ� ����1/12 */
#define home_offset 0				/* ��λ���ز����� */
#endif

/*
 *��ʱ����Ƶ������ 
 *��ϵ������ļ�������
 */
#define psc_init 50
#define arr_init 0

/*
 *������������
 */
#define send_buf_size 6501
#define motion_buf_size 500
#define can_buf_size 8

/*
 *cmd
 */
#define C_CALL						'C'
#define C_READY						'R'
#define C_ACTION 					'A'
#define C_STOP 						'S'
#define C_HOME 						'H'
#define C_MOTOR_DISABLE		'D'
#define C_MOTOR_ENABLE		'E'

/*
 *feedback
 */
#if SLAVE0
#define c_motion_request		"Q0"
#define c_receive_call			"RC"
#define c_motor_home 				"H0"
#define c_motor_ready 			"R0"
#define c_motor_arrive			"AR0"
#define c_motor_action			"AC0"
#define c_motor_stop				"S0"
#define c_motor_disable			"D0"
#define c_motor_enable			"E0"
#define c_buf_full					"F"
#define c_buf_usefull				"U"
#endif

#if SLAVE1
#define c_motion_request		"Q1"
#define c_receive_call			"RC"
#define c_motor_home 				"H1"
#define c_motor_ready 			"R1"
#define c_motor_arrive			"AR1"
#define c_motor_action			"AC1"
#define c_motor_stop				"S1"
#define c_motor_disable			"D1"
#define c_motor_enable			"E1"
#define c_buf_full					"F"
#define c_buf_usefull				"U"
#endif

#if SLAVE2
#define c_motion_request		"Q2"
#define c_receive_call			"RC"
#define c_motor_home 				"H2"
#define c_motor_ready 			"R2"
#define c_motor_arrive			"AR2"
#define c_motor_action			"AC2"
#define c_motor_stop				"S2"
#define c_motor_disable			"D2"
#define c_motor_enable			"E2"
#define c_buf_full					"F"
#define c_buf_usefull				"U"
#endif

#if SLAVE3
#define c_motion_request		"Q3"
#define c_receive_call			"RC"
#define c_motor_home 				"H3"
#define c_motor_ready 			"R3"
#define c_motor_arrive			"AR3"
#define c_motor_action			"AC3"
#define c_motor_stop				"S3"
#define c_motor_disable			"D3"
#define c_motor_enable			"E3"
#define c_buf_full					"F"
#define c_buf_usefull				"U"
#endif

/*
 *ID
 */
#define master 			0x20000000	/* mask: 0x20000000 */
#define slave_0 		0x00200000	/* mask: 0x00200000 */
#define slave_1 		0x00400000	/* mask: 0x00400000 */
#define slave_2 		0x00800000	/* mask: 0x00800000 */
#define slave_3 		0x01000000	/* mask: 0x01000000 */
//#define slave_4			0x02000000	/* mask: 0x02000000 Ԥ���ڵ� */	
//#define slave_5			0x04000000	/* mask: 0x04000000 Ԥ���ڵ� */
#define slave_all 	0x07Ef0000

/*
 *Motor status
 */
#define m_moving 	0x01	/* ��������˶��������˶�״̬ */
#define m_stop		0x02	/* ����ո�ֹͣ�˶�������ֹͣ״̬ */
#define m_waiting 0x03	/* �������ֹͣ�˶������ڵȴ�״̬ */


#define DelayForRespond delay_ms(6);

/**
 *ͳһ��������
 */
extern u8 home_flag;
extern u8 can_rec_buf[];
extern u8 can_send_buf[];

extern const float step_angle;

extern u16 send_buf[];
extern int current_position;
extern u8 zeroed;

/* ����˶���Ϣ��ʽ */
struct motion_info
{
	float rad;
	u8 dir;
	u8 speed_max;
	u8 state;
};
typedef struct motion_info motion_info;
extern motion_info motion_buf[motion_buf_size];

extern u8 motion_buf_full;
extern u16 empty_flag;	/* ��λ����ʼֵΪmotion_buf_size */
extern u16 full_flag;		/* ��λ����ʼֵΪ0 */
extern int product_count;	/* motion_info �����߼��� */
extern int consum_count;	/* motion_info �����߼��� */


#endif
