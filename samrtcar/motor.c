#include <smartconfig.h>

struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
rt_device_t pulse_encoder_dev;   /* 脉冲编码器设备句柄 */
static uint16_t counter = 0;

OUT2STRUCT out2struct;
OUT3STRUCT out3struct;
OUTSTRUCT outstruct;

//结构体通道选择器 
void DATASW(uint8_t s,OUTSTRUCT *p)
{
   if (s==OUT2_SIGNAL)
   {
    p->dev=out2struct.out2_dev;
    p->enc=out2struct.out2_enc;
    p->channel=out2struct.out2_channel;
    p->signal=out2struct.out2_signal;
    p->dir=out2struct.out2_dir;
    p->pulse=out2struct.out2_pulse;

   }
    if (s==OUT3_SIGNAL)
   {
    p->dev=out3struct.out3_dev;
    p->enc=out3struct.out3_enc;
    p->channel=out3struct.out3_channel;
    p->signal=out3struct.out3_signal;
    p->dir=out3struct.out3_dir;
    p->pulse=out3struct.out3_pulse;
   }   
}
//结构体通道保存器 
void DATASAVE(uint8_t s,OUTSTRUCT *p)
{
   if (s==OUT2_SIGNAL)
   {
//    out2struct.out2_dev=p->dev;
//    out2struct.out2_enc=p->enc;
//    out2struct.out2_channel=p->channel;
//    out2struct.out2_signal=p->signal;
    out2struct.out2_dir=p->dir;
    out2struct.out2_pulse=p->pulse;

   }
    if (s==OUT3_SIGNAL)
   {
//    out3struct.out3_dev=p->dev;
//    out3struct.out3_enc=p->enc;
//    out3struct.out3_channel=p->channel;
//    out3struct.out3_signal=p->signal;
    out3struct.out3_dir=p->dir;
    out3struct.out3_pulse=p->pulse;
   }   
}

//LN298N通道初始化
void DIRINIT(uint8_t s)
{
    if (s==OUT2_SIGNAL)//OUT2通道初始化
   {
    rt_pin_mode(CHANNEL1, PIN_MODE_OUTPUT);
    rt_pin_mode(CHANNEL2, PIN_MODE_OUTPUT);
    rt_pin_write(CHANNEL1,PIN_LOW);
    rt_pin_write(CHANNEL2,PIN_LOW);
   }
    if (s==OUT3_SIGNAL)//OUT3通道初始化
   {
    rt_pin_mode(CHANNEL3, PIN_MODE_OUTPUT);
    rt_pin_mode(CHANNEL4, PIN_MODE_OUTPUT);
    rt_pin_write(CHANNEL3,PIN_LOW);
    rt_pin_write(CHANNEL4,PIN_LOW);
   } 
}

//LN298N方向控制
void DIRSW(OUTSTRUCT *p)
{
    if (p->signal==OUT2_SIGNAL)
    {
        if (p->dir == FORWARD)//前进
        {
            rt_pin_write(CHANNEL1,PIN_HIGH);
            rt_pin_write(CHANNEL2,PIN_LOW);
        }
        else if (p->dir == BACKWARD)//后退
        {
            rt_pin_write(CHANNEL2,PIN_HIGH);
            rt_pin_write(CHANNEL1,PIN_LOW);
        }
        else if (p->dir == STOP)//停止
        {
            rt_pin_write(CHANNEL2,PIN_LOW);
            rt_pin_write(CHANNEL1,PIN_LOW);
        } 
    }
    else if (p->signal==OUT3_SIGNAL)
    {
        if (p->dir == FORWARD)//前进
        {
            rt_pin_write(CHANNEL3,PIN_HIGH);
            rt_pin_write(CHANNEL4,PIN_LOW);
        }
        else if (p->dir == BACKWARD)//后退
        {
            rt_pin_write(CHANNEL4,PIN_HIGH);
            rt_pin_write(CHANNEL3,PIN_LOW);
        }
        else if (p->dir == STOP)//停止
        {
            rt_pin_write(CHANNEL4,PIN_LOW);
            rt_pin_write(CHANNEL3,PIN_LOW);
        }
    }
}

//通道电控初始化
void MOTORINIT(uint8_t s,OUTSTRUCT *p)
{
     if (s==OUT2_SIGNAL)
     {
        //电机2设备数据结构体初始化
        out2struct.out2_dev=OUT2_DEV;
        out2struct.out2_enc=OUT2_ENC;
        out2struct.out2_channel=OUT2_CHANNEL;
        out2struct.out2_dir=STOP;
        out2struct.out2_signal=OUT2_SIGNAL;
        out2struct.out2_pulse=0;
     }

     if (s==OUT3_SIGNAL)
     {
        //电机3设备数据结构体初始化
        out3struct.out3_dev=OUT3_DEV;
        out3struct.out3_enc=OUT3_ENC;
        out3struct.out3_channel=OUT3_CHANNEL;
        out3struct.out3_dir=STOP;
        out3struct.out3_signal=OUT3_SIGNAL;
        out3struct.out3_pulse=0;
     }
    
    

    DIRINIT(s); //LN298N对应通道初始化
    DATASW(s,p);//识别电机通道切换对应数据
    DIRSW(p);   //读取通道信息设置通道方向

    //查找pwm设备
    pwm_dev = (struct rt_device_pwm *)rt_device_find(p->dev);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", p->dev);   
    }
    
/*
    //查找enc设备,闭环控制时可以启用，该设备也是rtt的驱动，适用于增量式编码器
    pulse_encoder_dev = rt_device_find(p->enc);
    if (pulse_encoder_dev == RT_NULL)
    {
        rt_kprintf("pulse encoder dev run failed! can't find %s device!\n", p->enc);
        
    }
    // 使能enc设备
    rt_device_open(pulse_encoder_dev, RT_DEVICE_OFLAG_RDONLY);
*/

    // 设置PWM周期和脉冲宽度默认值 
    rt_pwm_set(pwm_dev, p->channel, PWM_Period, p->pulse);
    // 使能pwm设备 
    rt_pwm_enable(pwm_dev, p->channel);

}

//速度设定
void SETSPEED(uint8_t s,OUTSTRUCT *p)
{
    DATASW(s,&outstruct);//识别电机通道切换对应数据
     /* 查找设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(p->dev);

    p->pulse = -(p->pulse);//参数反转
    if (p->pulse>=0)
    {
        p->dir=FORWARD;
        DIRSW(&outstruct);
        if (p->pulse>MOTOR_PWM_MAX)
        {
             p->pulse = MOTOR_PWM_MAX;
        }
        rt_pwm_set(pwm_dev, p->channel, PWM_Period, p->pulse);
        //DATASAVE(s,&outstruct);
    }
    else if (p->pulse<0)
    {
        p->dir=BACKWARD;
        DIRSW(&outstruct);
        if (p->pulse<MOTOR_PWM_MIN)
        {
             p->pulse = MOTOR_PWM_MIN;
        }
        p->pulse = -(p->pulse);
        rt_pwm_set(pwm_dev, p->channel, PWM_Period, p->pulse);
       // DATASAVE(s,&outstruct);
    }
    
}



//闭环速度控制
void CONTROLLOOP(uint8_t s,OUTSTRUCT *p,float speed)
{
    rt_int32_t encounter;
    float SpeedFeedback;
    DATASW(s,p);//识别电机通道切换对应数据

   /*
    //闭环编码器采样段
    pulse_encoder_dev = rt_device_find(p->enc);
    rt_device_read(pulse_encoder_dev, 0, &encounter, 1);//脉冲计数器采样
    rt_device_control(pulse_encoder_dev, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
    
    pidStr.vi_FeedBack=encounter;
    SpeedFeedback=(float)(encounter * PI * DiameterWheel)/ MOTOR_CONTROL_CYCLE / EncoderLine / 2.0f / ReductionRatio; //  m/s
    */

    rt_kprintf("speed is %2f\n", SpeedFeedback);

    if(speed > MOTOR_SPEED_MAX)
        speed = MOTOR_SPEED_MAX;   
    else if(speed < -MOTOR_SPEED_MAX)
        speed = -MOTOR_SPEED_MAX;

    pidStr.vi_Ref = (float)(speed*MOTOR_CONTROL_CYCLE / DiameterWheel / PI * EncoderLine * 2.0f * ReductionRatio);
    
    p->pulse=PID_MoveCalculate(&pidStr);

    DATASAVE(s,&outstruct);
    SETSPEED(s,p);
}


//线程控制函数
void CARTIM(uint8_t s,OUTSTRUCT *p,float speed)
{
    DATASW(s,p);//识别电机通道切换对应数据
    counter++;
    if (counter>=10)
    { 
        CONTROLLOOP(s,p,speed);
        counter=0;
    }
    
}






















