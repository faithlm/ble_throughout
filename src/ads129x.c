/******************************************************************************
 * @file    ads129x.c
 * @author  ST_RR
 * @version V1.5
 * @date    20-June-2020
 * @brief   This file contains ADC initialization process.
 ******************************************************************************/

#include "ads129x.h"

/***********************************************��дads129x�Ĵ��� ********************************************************************/
uint8_t ADS_REG(uint8_t command, uint8_t data)
{
	unsigned char data_return;

	SPI_ReadWriteByte(command);
	HAL_Delay(1);

	SPI_ReadWriteByte(0x00); //�����Ĵ���WREG��RREG��Ϊ���ֽ�ָ��ڶ���Ϊ��д�Ĵ�������-1������ο�ADS129x.h�ļ�����WREG��RREG������
	HAL_Delay(1);

	if ((command & 0x20) == 0x20) //�ж��Ƿ�Ϊ���Ĵ���ָ����Ƿ�0ȡ�ؼĴ���ֵ
	{
		data_return = SPI_ReadWriteByte(0x00);
	}

	if ((command & 0x40) == 0x40) //�ж��Ƿ�Ϊд�Ĵ���ָ����Ƿ����������ݣ����Է���ֵ
	{
		data_return = SPI_ReadWriteByte(data);
	}
	return (data_return);
}

/******��ȡ27���ֽ�����216λ 1100+ LOFF_STATP + LOFF_STATN + GPIO[4:7]+ 24*8ͨ�� =216 λ *****************/
void ADS_Read(unsigned char *data)
{
	unsigned char i;
	for (i = 0; i < 27; i++)
	{
		*data = SPI_ReadWriteByte(0x00); //���ص�������char���ͣ�8λ��
		data++;
	}
}

/*********************************�ϵ��ʼ�� ***************************************************************/
void ADS1299_PowerOnInit(void)
{

	ADS1299_StartLow(); //�Ȳ�Ҫ��ʼת������

	ADS1299_PowerUp(); // PWDN �øߵ�ƽ

	ADS1299_ResetHigh(); // �����λRESET
	HAL_Delay(150);		 //��ʱ�ȴ���Դ�ȶ�
}

/*****************������ǿ�Ƹ�λ�˿ڣ������ڲ����ֲ��ֽ����쳣״̬******************************************/
void POR_Reset_ADS1x9x(void)
{

	ADS1299_ResetHigh();
	HAL_Delay(50);
	ADS1299_ResetLow();
	HAL_Delay(1);
	ADS1299_ResetHigh();
}

/*************************ʹ��start�źſ�ʼת����ͬ������ADCоƬ******************************************/
void Enable_ADS1x9x_Conversion(void)
{

	ADS1299_StartLow();
	HAL_Delay(2);
	ADS1299_StartHigh();
	HAL_Delay(18);
}

/******************************* ���������� �������************************************************

	   CONFIG 1                              CHnSET

	���λ1 HR; 0 LP

 C0    32KHZ    40  16KHz                    05      Gain 6
 C1    16KHZ    41  8KHz                     15      Gain 1
 C2    8kHZ     42  4KHz                     25      Gain 2
 C3    4KHZ     43  2KHz                     35      Gain 3
 C4    2KHZ     44  1KHz                     45      Gain 4
 C5    1kHZ     45  500Hz                    55      Gain 8
 C6    500      46  250Hz                    65      Gain 12

****************************************************************************************************/
/*****************************����ģʽ���� ����Ϊ�ɼ��ڲ������ź�ģʽ******************************/
void ADS1299_Test_Mode(void)
{

	POR_Reset_ADS1x9x();	   //��λ
	SPI_ReadWriteByte(SDATAC); //ֹͣת��ģʽ
	HAL_Delay(5);

	ADS_REG(WREG | CONFIG3, 0xC0); //ʹ���ڲ��ο���ѹ��BIASREFʹ���ڲ�������AVDD+AVSS��/2
	HAL_Delay(10);				   //�ȴ��ڲ��ο���ѹ�ȶ�

	ADS_REG(WREG | CONFIG1, 0x46); //  ������C6,�͹���46
	HAL_Delay(1);

	ADS_REG(WREG | CONFIG2, 0X34); //���ò��Է����ź�Ƶ�ʷ�ֵ��
	HAL_Delay(1);

	ADS_REG(WREG | RLD_SENSP, 0x00); //Ĭ��ֵ
	ADS_REG(WREG | RLD_SENSN, 0x00);
	HAL_Delay(10);

	ADS_REG(WREG | CONFIG4, 0x00);
	HAL_Delay(1);

	ADS_REG(WREG | CH1SET, 0x65); // amplified x12
	HAL_Delay(1);
	ADS_REG(WREG | CH2SET, 0x55); // amplified x8
	HAL_Delay(1);
	ADS_REG(WREG | CH3SET, 0x45); // amplified x4
	HAL_Delay(1);
	ADS_REG(WREG | CH4SET, 0x35); // amplified x3
	HAL_Delay(1);
	ADS_REG(WREG | CH5SET, 0x25); // amplified x2
	HAL_Delay(1);
	ADS_REG(WREG | CH6SET, 0x15); // amplified x1
	HAL_Delay(1);
	ADS_REG(WREG | CH7SET, 0x65); // amplified x12
	HAL_Delay(1);
	ADS_REG(WREG | CH8SET, 0x65);
	HAL_Delay(1);

	ADS_REG(WREG | LOFF, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | LOFF_SENSP, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | LOFF_SENSN, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | LOFF_FLIP, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | LOFF_STATP, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | LOFF_STATN, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | GPIO, 0x0F);
	HAL_Delay(1);
	ADS_REG(WREG | PACE, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | RESP, 0x20);
	HAL_Delay(1);
	ADS_REG(WREG | WCT1, 0x00);
	HAL_Delay(1);
	ADS_REG(WREG | WCT2, 0x00);
	HAL_Delay(1);

	SPI_ReadWriteByte(RDATAC); //ִ��RDATAC�����н����Բ����Ĵ������� Enable continuous conversion mode
	HAL_Delay(1);
}
