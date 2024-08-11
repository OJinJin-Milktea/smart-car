#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include <smartconfig.h>

//本巡线代码纯属无奈之举，流水线电机闭环控制硬件上真得加外挂拓展通道，真没法写闭环
//所以smartcar线程腰斩了，遥控线程也跟着腰斩了，巡线直接在主函数里写了
//巡线本身不是设计为往流水线上使用的，所以只能强行套用多电机驱动
//本来压根没想做巡线的
//哭了

#define RIGHT    GET_PIN(A, 5)
#define LEFT     GET_PIN(A, 4)

#define TURNSTOP     0
#define TURNRIGHT    1
#define TURNLEFT     2
#define TURNON       3

#define SLOW         150000
#define FAST         250000
#define QUIC         400000
#define WOCAO        500000


#define speedinit     FAST   

void sinit(signed int v,OUTSTRUCT *p)
{
        out3struct.out3_pulse=v;
        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);

        out2struct.out2_pulse=v;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p);

}

void speedsw(signed int v,OUTSTRUCT *p)
{
    if (p->signal==OUT3_SIGNAL)
    {
        out3struct.out3_pulse=v;
        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);
    }
    else if (p->signal==OUT2_SIGNAL)
    {
        out2struct.out2_pulse=v;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p); 
    }
    
}

void turn(uint8_t dir,OUTSTRUCT *p)
{
    if (dir==TURNRIGHT)
    {
        out3struct.out3_dir=STOP;
        //out3struct.out3_pulse=-WOCAO;
        out3struct.out3_pulse=0;

        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);

        out2struct.out2_dir=FORWARD;
        //out2struct.out2_pulse=WOCAO;
        out2struct.out2_pulse=speedinit+150000;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);

    }
    else if (dir==TURNLEFT)
    {
        out2struct.out2_dir=STOP;
        //out2struct.out2_pulse=-WOCAO;
        out2struct.out2_pulse=0;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);

        out3struct.out3_dir=FORWARD;
        //out3struct.out3_pulse=WOCAO;
        out3struct.out3_pulse=speedinit+150000;
        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);
    }
    else if (dir==TURNSTOP)
    {
        out2struct.out2_dir=STOP;
        out2struct.out2_pulse=0;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);

        out3struct.out3_dir=STOP;
        out3struct.out3_pulse=0;
        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);
    }
    else if (dir==TURNON)
    {
        out2struct.out2_dir=FORWARD;
        out2struct.out2_pulse=speedinit;
        DATASW(OUT2_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);

        out3struct.out3_dir=FORWARD;
        out3struct.out3_pulse=speedinit;
        DATASW(OUT3_SIGNAL,p);
        SETSPEED(p->signal,p);
        DIRSW(p);
    }
    

}
void xunji(OUTSTRUCT *p)
{
    if (rt_pin_read(RIGHT)==0 && rt_pin_read(LEFT)==1)
    {
        turn(TURNLEFT,p);
    }
    else if (rt_pin_read(RIGHT)==1 && rt_pin_read(LEFT)==0)
    {
        turn(TURNRIGHT,p);
    }
    else if (rt_pin_read(RIGHT)==1 && rt_pin_read(LEFT)==1)
    {
        turn(TURNON,p);
    }
    else if (rt_pin_read(RIGHT)==0 && rt_pin_read(LEFT)==0)
    {
        turn(TURNON,p);
    }
    
}