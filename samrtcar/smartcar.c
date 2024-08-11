#include <smartconfig.h>
#include <rtthread.h>


#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5



/* 定时器的控制块 */
static struct rt_timer timer1;
static int cnt = 0;


/* 定时器 1 超时函数 */
static void timeout1(void* parameter)
{
    
}

void TIMINIT(void)
{
    /* 初始化定时器 */
    rt_timer_init(&timer1, "timer1",  /* 定时器名字是 timer1 */
                    timeout1, /* 超时时回调的处理函数 */
                    RT_NULL, /* 超时函数的入口参数 */
                    10, /* 定时长度，以 OS Tick 为单位，即 10 个 OS Tick */
                    RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */
    
    /* 启动定时器 */
    //rt_timer_start(&timer1);
}



rt_align(RT_ALIGN_SIZE)
static char car_symtem_stack[1024];
static struct rt_thread car_symtem;

rt_align(RT_ALIGN_SIZE)
static char carsport_stack[1024];
static struct rt_thread carsport;




/* 子线程电控 入口 */
static void carsport_entry(void *param)
{
    //电机2初始化
    DIRINIT(OUT2_SIGNAL);
    MOTORINIT(OUT2_SIGNAL,&outstruct);
    //电机3初始化
    DIRINIT(OUT3_SIGNAL);
    MOTORINIT(OUT3_SIGNAL,&outstruct);
    
    while (1)
    {
        xunji(&outstruct);       
    }
    
}

/* 系统线程 入口 */
static void car_symtem_entry(void *param)
{
     rt_thread_init(&carsport,
                   "carsport",
                   carsport_entry,
                   RT_NULL,
                   &carsport_stack[0],
                   sizeof(carsport_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
                   
    rt_thread_startup(&carsport);

}



int carsymtem(void)
{

    /* 初始化线程 2，名称是 thread2，入口是 thread2_entry */
    rt_thread_init(&car_symtem,
                   "car_symtem",
                   car_symtem_entry,
                   RT_NULL,
                   &car_symtem_stack[0],
                   sizeof(car_symtem_stack),
                   THREAD_PRIORITY , THREAD_TIMESLICE);

    rt_thread_startup(&car_symtem);

    return 0;
}



/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(carsymtem, car symtem);