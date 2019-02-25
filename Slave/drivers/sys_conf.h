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
 
/*�����������
 */
#define motor_type 400	/* ������ͣ�����200 pulse/r */
#define Micro_Step 2		/* ����ϸ���� ����1/2 */


/*
 *��ʱ����Ƶ������ 
 *��ϵ������ļ�������
 */
#define psc_init 719
#define arr_init 0

/*
 *������������
 */
#define send_buf_size 6401
#define motion_buf_size 200
#define CAN_buf_size 8

/*
 *�������
 */
#define pi 3.14

/*
 *��������
 */
#define ratio 12					/* ��е���ٱ� ����1/12 */

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
#define c_receive_call			"RC"
#define c_motor_home 				"H0"
#define c_motor_ready 			"R0"
#define c_motor_arrive			"AR0"
#define c_motor_action			"AC0"
#define c_motor_stop				"S0"
#define c_motor_disable			"D0"
#define c_motor_enable			"E0"

/*
 *ID
 */
#define master 			0x20000000	/* mask: 0x20000000 */
#define slave_0 		0x00200000	/* mask: 0x00200000 */
#define slave_1 		0x00400000	/* mask: 0x00400000 */
#define slave_2 		0x00800000	/* mask: 0x00800000 */
#define slave_3 		0x01000000	/* mask: 0x01000000 */
#define slave_4			0x02000000	/* mask: 0x02000000 */	
#define slave_5			0x04000000	/* mask: 0x04000000 */	
#define slave_all 	0x07Ef0000

/*
 *Motor status
 */
#define m_moving 	0x01	/* ��������˶��������˶�״̬ */
#define m_stop		0x02	/* ����ո�ֹͣ�˶�������ֹͣ״̬ */
#define m_waiting 0x03	/* �������ֹͣ�˶������ڵȴ�״̬ */

#endif
