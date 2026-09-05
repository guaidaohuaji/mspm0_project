#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "ti_msp_dl_config.h"

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240	
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif



//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() DL_GPIO_clearPins(GPIO_LCD_PIN_SCLK_PORT,GPIO_LCD_PIN_SCLK_PIN)//SCL=SCLK
#define LCD_SCLK_Set() DL_GPIO_setPins(GPIO_LCD_PIN_SCLK_PORT,GPIO_LCD_PIN_SCLK_PIN)

#define LCD_MOSI_Clr() DL_GPIO_clearPins(GPIO_LCD_PIN_MOSI_PORT,GPIO_LCD_PIN_MOSI_PIN)//SDA=MOSI
#define LCD_MOSI_Set() DL_GPIO_setPins(GPIO_LCD_PIN_MOSI_PORT,GPIO_LCD_PIN_MOSI_PIN)

#define LCD_RES_Clr()  DL_GPIO_clearPins(GPIO_LCD_PIN_RES_PORT,GPIO_LCD_PIN_RES_PIN)//RES
#define LCD_RES_Set()  DL_GPIO_setPins(GPIO_LCD_PIN_RES_PORT,GPIO_LCD_PIN_RES_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(GPIO_LCD_PIN_DC_PORT,GPIO_LCD_PIN_DC_PIN)//DC
#define LCD_DC_Set()   DL_GPIO_setPins(GPIO_LCD_PIN_DC_PORT,GPIO_LCD_PIN_DC_PIN)
 		     
#define LCD_CS_Clr()   DL_GPIO_clearPins(GPIO_LCD_PIN_CS_PORT,GPIO_LCD_PIN_CS_PIN)//CS
#define LCD_CS_Set()   DL_GPIO_setPins(GPIO_LCD_PIN_CS_PORT,GPIO_LCD_PIN_CS_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(GPIO_LCD_PIN_BLK_PORT,GPIO_LCD_PIN_BLK_PIN)//BLK
#define LCD_BLK_Set()  DL_GPIO_setPins(GPIO_LCD_PIN_BLK_PORT,GPIO_LCD_PIN_BLK_PIN)



void lcd_delay_ms(uint32_t ms); //延时函数
void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节
void LCD_WR_REG(uint8_t dat);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




