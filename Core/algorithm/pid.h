#ifndef _PID_ALGORITHM_H_
#define _PID_ALGORITHM_H_

typedef struct
{
    //pid 
    float Kp;       //比例系数
    float Ki;       //积分系数

    //pid limit
    float integral_limit;      //积分限幅，防止积分饱和(>0)
    float integral_deadzone;    //积分死区，防止积分饱和(>0)

    // PID 状态
    float integral;
}PI_data_typedef;

void PI_init(
    PI_data_typedef* PI_data,
    float Kp,
    float Ki,
    float integral_limit,
    float integral_deadzone
);

void PI_clear_integral(PI_data_typedef* PI_data);
//(return type)_(name of algorithm)_(method)_(method when integral is outof control)[_(extra description)]
float f_PI_calcu_keep(PI_data_typedef* PI_data, float target, float real);

int i16_pid_calcu_addition(float target_voltage, float actual_voltage);

#endif // !_PID_ALGORITHM_H_