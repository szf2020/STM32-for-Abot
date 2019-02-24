#include "spi.h"

/**
 *@function	SPI��ʼ��
 *@param void
 *@return void
 */
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	/* ʹ��PORTBʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);	/* ʹ��SPI2ʱ�� */		
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	/* PB13,PB14,PB15 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	/* ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	/* ����PB13,PB14,PB15 */

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	/* ����Ϊ˫��˫��ȫ˫�� */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	/* ����ģʽ */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 8λ֡�ṹ */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	/* Ӳ��Ƭѡ */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	/* �����ʷ�ƵֵΪ256 */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure); 
 
	SPI_Cmd(SPI2, ENABLE); /* ʹ������ */
	
	SPI2_ReadWriteByte(0xff);	/* �������� */
}

/**
 *@function	����SPI�ٶ�
 *@param		
 *					SPI_BaudRatePrescaler_2   2��Ƶ 
 *					SPI_BaudRatePrescaler_8   8��Ƶ  
 *					SPI_BaudRatePrescaler_16  16��Ƶ 
 *					SPI_BaudRatePrescaler_256 256��Ƶ
 *@return void
 */
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;
	SPI_Cmd(SPI2,ENABLE); 
} 

/**
 *@function	��дһ���ֽ�
 *@param	Ҫд����ֽ�
 *@return	��ȡ���ֽ�
 */
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)	/* ���ͻ���ձ�־λ */
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData);
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)	/* ���ܻ���ǿձ�־λ */
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2);    
}
