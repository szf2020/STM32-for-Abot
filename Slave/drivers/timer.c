#include "timer.h"
#include "sys_conf.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 
/*
 *TIM3 PWM���ֳ�ʼ�� 
 *PWM�����ʼ��
 *arr���Զ���װֵ
 *psc��ʱ��Ԥ��Ƶ��
 */
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* ʹ��GPIO���� */   
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* ��ʼ��GPIO */
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
 
   /* ��ʼ��TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
	TIM_OCInitStructure.TIM_Pulse= 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
	TIM3->CR1 = 0x00000081;
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
}
