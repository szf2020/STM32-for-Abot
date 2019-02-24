#include "w25qxx.h"

u16 W25QXX_TYPE=W25Q80;	/* ����SPI FLASH�ͺ� */
					
/**
 *@function	��ʼ��SPI FLASH IO
 *@param void
 *@return void
 */					
void W25QXX_Init(void)
{	
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  /* Ƭѡ�� */
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* ������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
 
  W25QXX_CS=1;	/* Ƭѡ���ߣ�ȡ��ѡ�� */				
	SPI2_Init();	/* ��ʼ��SPI */	  
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);	/* 18Mʱ�� */
	W25QXX_TYPE=W25QXX_ReadID();	/* ��ȡFLASH ID */
}  

/**
 *@function	��ȡ״̬�Ĵ���
 *@param void
 *@return �Ĵ���ֵ
 * BIT 7   6  5  4   3   2   1   0
 *     SPR RV TB BP2 BP1 BP0 WEL BUSY
 * SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
 * TB,BP2,BP1,BP0:FLASH����д��������
 * WEL:дʹ������
 * BUSY:æ���λ(1:æ;0:����)
 * Ĭ��ֵ:0x00
 */
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;	/* Ƭѡ���ͣ�ʹ��w25q */                            
	SPI2_ReadWriteByte(W25X_ReadStatusReg);	/* ���Ͷ�ȡ�Ĵ������� */
	byte=SPI2_ReadWriteByte(0Xff);	/* ��ȡһ���ֽ� */          
	W25QXX_CS=1;	/* Ƭѡ���ߣ�ȡ��ѡ�� */                           
	return byte;   
} 

/**
 *@function д״̬�Ĵ���
 *@parma Ҫд���ֵ
 *@return void
 */
void W25QXX_Write_SR(u8 sr)   
{   
	W25QXX_CS=0;                             
	SPI2_ReadWriteByte(W25X_WriteStatusReg);   
	SPI2_ReadWriteByte(sr);               	
	W25QXX_CS=1;                               	      
}   
   
/**
 *@function дʹ��
 *��λWEL	
 *@param void
 *@return void
 */
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                          	
    SPI2_ReadWriteByte(W25X_WriteEnable); 	  
	W25QXX_CS=1;                               	      
} 

/**
 *@function д��ֹ
 *����WEL	
 *@param void
 *@return void
 */
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            
    SPI2_ReadWriteByte(W25X_WriteDisable);     
	W25QXX_CS=1;                                
} 		
	
/**
 *@function ��ȡоƬID
 *@param void
 *@return оƬID
 *0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
 *0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
 *0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
 *0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
 *0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	
 */
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI2_ReadWriteByte(0x90);
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return Temp;
}   		    

/**
 *@function ��ȡSPI FLASH
 *��ָ����ַ��ʼ��ȡָ�����ȵ����ݣ��������ݴ��浽���ݴ洢��
 *@param
 *				pBuffer ���ݴ洢��
 *				ReadAddr ��ȡ����ʼ��ַ(24bit)
 *				NumByteToRead Ҫ��ȡ���ֽ���(max:65535)
 *@return void
 */
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS=0;                            
	SPI2_ReadWriteByte(W25X_ReadData); 	/* ���Ͷ�ȡ���� */        	
	SPI2_ReadWriteByte((u8)((ReadAddr)>>16));	/* ����24bit��ַ */
  SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   /* ѭ����ȡ��ֱ����ȡ��Ŀ���ֽ��� */
    }
	W25QXX_CS=1;  				    	      
}  

/**
 *@function ��һҳ��д������256���ֽ�
 *��ָ����ַ��ʼд��ָ�����ȵ����ݣ����ȷ����ַ��Խ��
 *@param
 *				pBuffer ���ݴ洢��
 *				ReadAddr д�����ʼ��ַ(24bit)
 *				NumByteToRead Ҫд����ֽ���(max:256)
 *@return void
 */
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  W25QXX_Write_Enable();                  	
	W25QXX_CS=0;                            	
  SPI2_ReadWriteByte(W25X_PageProgram);	/* ����дҳ���� */      	
  SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); 	/* ����24bit��ַ */
  SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++) SPI2_ReadWriteByte(pBuffer[i]);	/* ѭ��д�룬ֱ��д��Ŀ���ֽ��� */
	W25QXX_CS=1;                      
	W25QXX_Wait_Busy();		/* �ȴ�д����� */				   		
} 

/**
 *@function �޼���дSPI FLASH
 *��ָ����ַ��ʼд��ָ�����ȵ����ݣ����ȷ����ַ��Խ�磬���Զ���ҳ����
 *����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0xff,�����ڷ�0xff��λ�ý�д��ʧ��
 *@param
 *				pBuffer ���ݴ洢��
 *				ReadAddr д�����ʼ��ַ(24bit)
 *				NumByteToRead Ҫд����ֽ���(max:256)
 *@return void
 */
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; 	 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  
			if(NumByteToWrite>256)pageremain=256; 
			else pageremain=NumByteToWrite; 	 
		}
	};	    
} 
  
/**
 *@function дSPI FLASH
 *��ָ����ַ��ʼд��ָ�����ȵ����ݣ��ú������в�������
 *@param
 *				pBuffer ���ݴ洢��
 *				ReadAddr д�����ʼ��ַ(24bit)
 *				NumByteToRead Ҫд����ֽ���(max:256)
 *@return void
 */
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;  
	secoff=WriteAddr%4096;
	secremain=4096-secoff;
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
		for(i=0;i<secremain;i++)
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;  
		}
		if(i<secremain)
		{
			W25QXX_Erase_Sector(secpos);
			for(i=0;i<secremain;i++)
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);	   
		if(NumByteToWrite==secremain)break;
		else
		{
			secpos++;
			secoff=0;

		   	pBuffer+=secremain;
				WriteAddr+=secremain;
		   	NumByteToWrite-=secremain;
			if(NumByteToWrite>4096)secremain=4096;
			else secremain=NumByteToWrite;
		}	 
	};	 
}

/**
 *@function ��������оƬ
 *@param void
 *@return void
 */
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                        
    SPI2_ReadWriteByte(W25X_ChipErase);     
		W25QXX_CS=1;                          
		W25QXX_Wait_Busy();   				   	
}   

/**
 *@function ����һ������
 *@param 
 * 				Dst Addr:������ַ������ʵ����������
 *@return void
 */
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
 
 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  
    W25QXX_Wait_Busy();   
  	W25QXX_CS=0;                            	
    SPI2_ReadWriteByte(W25X_SectorErase);      	
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  	
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
		W25QXX_CS=1;                            	     
    W25QXX_Wait_Busy();   				   		
}  

/**
 *@function �ȴ�����
 *@param void
 *@return void
 */
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);  		
}  

/**
 *@function �������ģʽ
 *@param void
 *@return void
 */
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS=0;                           	 	
    SPI2_ReadWriteByte(W25X_PowerDown);      
		W25QXX_CS=1;                            
    rt_hw_us_delay(3);                            
}   

/**
 *@function ����оƬ
 *@param void
 *@return void
 */
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS=0;                            
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);	
		W25QXX_CS=1;                         
    rt_hw_us_delay(3);                         
}   

