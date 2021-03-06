/*
*
*   IDE: KEIL5
*   MCU: stm32
*   System clock is equal 72Mhz.
*   2014.2.28
*/
#include "tm1637.h"
//                  			0 1 2 3 4 5 6 7 8 9 A B C D E F - 灭 N  t L H r o

const u8 SegA[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x77, 0x7c, 0x39, 0x5e, 0x79,
    0x71, 0x40, 0x00, 0x37, 0x78,
    0x38, 0x76, 0x50, 0x5c, 0x73,
    0x3e, 0x5c, 0x0e
}; //段码

static void Delayus(u8 Nus)
{
    for(u8 i = 0; i<Nus; i++)
    {
        u8 j = 5;
        while(j--);
    }
    return;
}

void I2CStart(void)
{   //1637开始
    CLK_H;
    DO_H;
    Delayus( 2 );
    DO_L;
}

void I2Cask(void)
{
    u8 i = 255;
    CLK_L;
    Delayus( 5 );   //在第八个时钟下降沿之后延时5us
    while(DI)
    {
        i--;
        if(i == 0)
        {
            TM1637_Init(2);
            break;
        }
    }
    CLK_H;
    Delayus( 2 );
    CLK_L;
}


void I2CStop(void)
{
    CLK_L;
    Delayus( 2 );
    DO_L;
    Delayus( 2 );
    CLK_H;
    Delayus( 2 );
    DO_H;
}

void I2CWrByte(unsigned char oneByte)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        CLK_L;
        if(oneByte&0x01) //低位在前
        {
            DO_H;
        }
        else
        {
            DO_L;
        }
        Delayus( 3 );
        oneByte=oneByte>>1;
        CLK_H;
        Delayus( 3 );



    }
}

/*************************************************
Function:      TM1637_Init
Description:   adjust the brightness of LED
Input:      unsigned char    brightness
            input number 0~7 to adjust brightness

Output:         none
Return:         none
Others:         none
*************************************************/
void TM1637_Init(unsigned char  brightness)
{

    brightness|=0x88;
    I2CStart();
    I2CWrByte(brightness);  //开显示亮度
    I2Cask();
    I2CStop();
}



/*************************************************
Function:      Write_8LED
Description:   control the display of LED
Input:          uchar oneSecondsFlag
                uchar data1
                uchar data2
                uchar data3
                uchar data4
                uchar data5

Output:         none
Return:         none
Others:         none
*************************************************/
void Write_8LED(uchar sel ,uchar oneSecondsFlag,uchar data1,uchar data2,uchar data3,uchar	data4,uchar data5 )           //写显示寄存器
{

    I2CStart();
    I2CWrByte(0x40);         //40H地址自动加1模式，
    // I2CWrByte(0x44);         //44H固定地址模式。


    sel |= 0x20;//电源灯


    I2Cask();
    I2CStop();

    I2CStart();
    I2CWrByte(0xc0);         //设置首地址
    //地址自动加1，不必每次都写地址

    //add what you need !
    I2Cask();

    I2CWrByte(sel );  //发送数据    GR1

    I2Cask();

    I2CWrByte( SegA[data1] );  //发送数据    GR2

    I2Cask();

    I2CWrByte( oneSecondsFlag==2?SegA[data2]|0x80: SegA[data2] );  //发送数据    GR3

    I2Cask();

    I2CWrByte( oneSecondsFlag==1?SegA[data3]|0x80: SegA[data3] );  //发送数据  GR4

    I2Cask();

    I2CWrByte( oneSecondsFlag==4?SegA[data4]|0x80: SegA[data4]);  //发送数据   GR5

    I2Cask();

    I2CWrByte( SegA[data5] );   //发送数据   GR6


    I2Cask();


    I2CStop();



}

/*************************************************
Function:      Write_LED
Description:   control single LED light
Input:         witch led you want to control

Output:         none
Return:         none
Others:         none
*************************************************/
void	Write_LED(uchar	data)
{
    I2CStart();
    I2CWrByte(0x44);         //44H固定地址模式。
    I2Cask();
    I2CStop();
    I2CStart();
    I2CWrByte(0xc0);         //设置首地址

    I2Cask();
    //地址自动加1，不必每次都写地址
    I2CWrByte(data );  //发送数据    GR1
    I2Cask();
    I2CStop();
}



////==============================================
//unsigned char ScanKey(void)
// {
//  unsigned char rekey,i;
//  I2CStart();
//  I2CWrByte(0x42); //读按键命令
//   I2Cask();
//   DO = 1;
//   for(i=0;i<8;i++)
//    {
//     CLK=0;
//        rekey=rekey>>1;
//     Delay_us(30);
//     if(DI)
//      {
//       rekey=rekey|0x80;
//      }
//
//     Delay_us(30);
//
//    }
//     I2Cask();
//     I2CStop();
//
//
//    return (rekey);
// }
//