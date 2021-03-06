/********************************************
 **--------------文件信息---------------------
 ** 文 件 名: Delay.c
 ** 创 建 人: 张豪杰
 ** 创建日期: 2014年02月15日
 ** 描    述: 配置系统中使用的延时函数
 **
 *******************************************/

#include "delay.h"	

static u8 fac_us = 0; //us延时倍乘数
static u16 fac_ms = 0; //ms延时倍乘数,在ucos下,代表每个节拍的ms数

//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(u8 SYSCLK)
{
	u32 reload;

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us = SYSCLK / 8;		//不论是否使用ucos,fac_us都需要使用

	reload = SYSCLK / 8;		//每秒钟的计数次数 单位为K
	reload *= 1000000 / OS_TICKS_PER_SEC;		//根据OS_TICKS_PER_SEC设定溢出时间
	//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右
	fac_ms = 1000 / OS_TICKS_PER_SEC;						//代表ucos可以延时的最少单位
	SysTick->CTRL |= (1ul << 1);//SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD = reload; 	//每1/OS_TICKS_PER_SEC秒中断一次
	SysTick->CTRL |= (1ul << 0);   	//开启SYSTICK
}

//延时nus
//nus为要延时的us数.
void Delay_us(u32 nus)
{
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD;	//LOAD的值
	ticks = nus * fac_us; 			//需要的节拍数
	tcnt = 0;
	OSSchedLock();				//阻止ucos调度，防止打断us延时
	told = SysTick->VAL;        	//刚进入时的计数器值
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			if (tnow < told)
				tcnt += told - tnow;        	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break;        	//时间超过/等于要延迟的时间,则退出.
		}
	};
	OSSchedUnlock();			//开启ucos调度
}
//延时nms
//nms:要延时的ms数
void Delay_ms(u16 nms)
{
	if (OSRunning == TRUE)			//如果os已经在跑了
	{
		if (nms >= fac_ms)			//延时的时间大于ucos的最少时间周期
		{
			OSTimeDly(nms / fac_ms);			//ucos延时
		}
		nms %= fac_ms;			//ucos已经无法提供这么小的延时了,采用普通方式延时
	}
	Delay_us((u32)(nms * 1000));	//普通方式延时
}

