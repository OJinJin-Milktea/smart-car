#ifndef __SMARTCONFIG_H
#define __SMARTCONFIG_H


#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "drv_config.h"
#include "drv_tim.h"

#include <drivers/rt_drv_pwm.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pid.h>

 /*--------------------------------------- POMD引脚配置情况 -----------------------------------------------*/
//
//                        | MODE        |    GPIOS    |    GPIOS    | MODE        |   Config    |
//                        | :---------: | :---------: | :---------: | :---------: | :---------: |
//                        |             | PF4         | PD12        |   TIM4_CH1  |             |
//                        |             | PF5         | PB11        |   TIM2_CH4  |  TIM2_CH4   |
//                        |             | PF6         | PB10        |   TIM2_CH3  |  TIM2_CH3   |
//                        |             | PF7         | PD11        |             |             |
//
//                        | MODE        |    GPIOS    |    GPIOS    | MODE        |   Config    |
//                        | :---------: | :---------: | :---------: | :---------: | :---------: |
//                        | channel_4   | PE4         | PA5         |             |  光感右侧    |
//                        | channel_3   | PE3         | PA6         |             |  光感左侧    |
//                        | channel_2   | PE5         | PA7         | TIM3_CH2    |             |
//                        | channel_1   | PE2         | PA4         |             |             |
//
 /*-------------------------------------------------------------------------------------------------------*/


//驱动器通道设置
#define CHANNEL1    GET_PIN(E, 2)
#define CHANNEL2    GET_PIN(E, 5)
#define CHANNEL3    GET_PIN(E, 3)
#define CHANNEL4    GET_PIN(E, 4)

#define FORWARD     2                 //OUT2前进
#define BACKWARD    1                 //OUT2后退
#define STOP        0                 //OUT2停止

//PWM通道设置
#define OUT2_DEV           "pwm2"                /* 电机2 PWM设备名称 */
#define OUT2_CHANNEL        3                    /* PWM通道 */
#define OUT2_SIGNAL         1                    /* 电机2识别号 */

#define OUT3_DEV           "pwm2"                /* 电机3  PWM设备名称 */
#define OUT3_CHANNEL        4                    /* PWM通道 */
#define OUT3_SIGNAL         2                    /* 电机3识别号 */

//正交编码器设置（未使用）
#define OUT2_ENC           "pulse2"              //电机2 计数器名称
#define OUT3_ENC           "pulse4"              //电机3 计数器名称

//车辆模型数据
#define ReductionRatio      1.0f			     //电机减速比
#define EncoderLine  		20				     //编码器线数
#define DiameterWheel		0.068f	 		     //轮子直径：mm

//软件速控预设
#define PWM_Period          500000                //PWM周期
#define MOTOR_PWM_MAX   	400000		          //禁止满占空比输出，造成MOS损坏
#define MOTOR_PWM_MIN	   -400000		          //
#define MOTOR_SPEED_MAX		10.0f	 	          //电机最大转速(m/s) (0.017,8.04)
#define PI					3.141593f             //π
#define MOTOR_CONTROL_CYCLE	0.01f    	          //电机控制周期T：10ms

//PID通用计算包请去pid.h修改预设值

//设备2数据保存结构体
typedef struct
{
    uint8_t out2_signal;                 //结构体识别码
    const char* out2_dev;                //pwm设备
    const char* out2_enc;                //enc设备
    uint8_t out2_channel;                //通道
    uint8_t out2_dir;                    //方向
    signed int out2_pulse;               //脉宽数据

}OUT2STRUCT;
//OUT2STRUCT out2struct;
extern OUT2STRUCT out2struct;

//设备3数据保存结构体
typedef struct
{
    uint8_t out3_signal;                 //结构体识别码
    const char* out3_dev;                //pwm设备
    const char* out3_enc;                //enc设备
    uint8_t out3_channel;                //通道
    uint8_t out3_dir;                    //方向
    signed int out3_pulse;               //脉宽数据

}OUT3STRUCT;
//OUT3STRUCT out3struct;
extern OUT3STRUCT out3struct;

//设备控制结构体
typedef struct
{
    uint8_t signal;                 //结构体识别码
    const char* dev;                //pwm设备
    const char* enc;                //enc设备
    uint8_t channel;                //通道
    uint8_t dir;                    //方向
    signed int pulse;               //脉宽数据

}OUTSTRUCT;
extern OUTSTRUCT outstruct;

void DATASW(uint8_t s,OUTSTRUCT *p);                     //结构体通道选择器 
void DATASAVE(uint8_t s,OUTSTRUCT *p);                   //结构体通道保存器 
void DIRINIT(uint8_t s);                                 //LN298N驱动器通道初始化
void DIRSW(OUTSTRUCT *p);                                //LN298N驱动器控制
void MOTORINIT(uint8_t s,OUTSTRUCT *p);                  //电机初始化（一次只对一侧电机初始化方便使用单电机模式。）
void SETSPEED(uint8_t s,OUTSTRUCT *p);                   //速度设定
void CONTROLLOOP(uint8_t s,OUTSTRUCT *p,float speed);    //闭环速度控制
void CARTIM(uint8_t s,OUTSTRUCT *p,float speed);         //线程控制函数

void xunji(OUTSTRUCT *p);

//void ENCODERINIT(const char* dev);                           //脉冲计数器初始化
//void ENCODER_RevSample(void);                                //脉冲计数器采样





#endif