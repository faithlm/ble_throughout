/*
 * @Author: Liangmeng
 * @Date: 2022-08-06 00:05:02
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-06 00:05:13
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\ads129x.h
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#ifndef _ADS1298_H_
#define _ADS1298_H_

/*******************�ӿں궨�壬���������ӳ��ο�STM32Cube��������main.h�ļ�*****************/

// PWDN -------
#define ADS1299_PowerUp() HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_SET)
#define ADS1299_PowerDown() HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_RESET)

// RESET -------
#define ADS1299_ResetLow() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET)
#define ADS1299_ResetHigh() HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET)

// START --------
#define ADS1299_StartHigh() HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_SET)
#define ADS1299_StartLow() HAL_GPIO_WritePin(START_GPIO_Port, START_Pin, GPIO_PIN_RESET)

/********************************************************************************/

/**************ADS129x�����******************************/

/*ϵͳ����*/
#define WAKEUP 0x02	   //�Ӵ���ģʽ����
#define STANDBY 0x04   //�������ģʽ
#define ADS_RESET 0x06 //��λADS1292R
#define START 0x08	   //������ת��
#define STOP 0x0A	   //ֹͣת��
					   //#define OFFSETCAL	  0X1A	//ͨ��ƫ��У׼

/*���ݶ�ȡ����*/
#define RDATAC 0x10 //�������������ݶ�ȡģʽ,Ĭ��ʹ�ô�ģʽ
#define SDATAC 0x11 //ֹͣ���������ݶ�ȡģʽ
#define RDATA 0x12	//ͨ�������ȡ����;֧�ֶ��ֶ��أ�����ת��ģʽ

/*�Ĵ�����ȡ����*/
#define RREG 0x20 //��ȡ001r rrrr 000n nnnn   ��ʵ�ϸ߰�λ�У�r rrrr������ʼ�Ĵ�����ַ��ȫ���Ĵ�����ַ���5λ���ɱ�ʾ��
#define WREG 0x40 //д��010r rrrr 000n nnnn   �����n nnnnΪҪ��д�ļĴ���������ȥ1����˵����Ĵ�����дΪ0x00h
/*	r rrrr=Ҫ����д�ļĴ�����ַ
		n nnnn=Ҫ����д������*/

/*ADS129x�ڲ��Ĵ�����ַ����*/
#define ID 0x00		   // ID���ƼĴ���
#define CONFIG1 0x01   //���üĴ���1
#define CONFIG2 0x02   //���üĴ���2
#define CONFIG3 0x03   //���üĴ���3
#define LOFF 0x04	   //����������ƼĴ���
#define CH1SET 0x05	   //ͨ��1���üĴ���
#define CH2SET 0x06	   //ͨ��2���üĴ���
#define CH3SET 0x07	   //ͨ��3���üĴ���
#define CH4SET 0x08	   //ͨ��4���üĴ���
#define CH5SET 0x09	   //ͨ��5���üĴ���
#define CH6SET 0x0A	   //ͨ��6���üĴ���
#define CH7SET 0x0B	   //ͨ��7���üĴ���
#define CH8SET 0x0C	   //ͨ��8���üĴ���
#define RLD_SENSP 0x0D //
#define RLD_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11

#define LOFF_STATP 0x12
#define LOFF_STATN 0x13 //����״̬�Ĵ�����ַ��ֻ����

#define GPIO 0x14 // GPIO���ƼĴ���
#define PACE 0x15
#define RESP 0x16
#define CONFIG4 0x17
#define WCT1 0x18
#define WCT2 0x19

#endif
