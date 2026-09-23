#include "pid.h"

#define my_fabs(x) (((x) >= 0.0f) ? (x) : -(x))

void PI_init(
    PI_data_typedef* PI_data,
    float Kp,
    float Ki,
    float integral_limit,
    float integral_deadzone
)
{
    PI_data->integral = 0;
    PI_data->Kp = Kp;
    PI_data->Ki = Ki;
    PI_data->integral_limit = integral_limit;
    PI_data->integral_deadzone = integral_deadzone;
}

void PI_clear_integral(PI_data_typedef* PI_data)
{
    PI_data->integral = 0;
}

float f_PI_calcu_keep(PI_data_typedef* PI_data, float target, float real)
{
    float err = target - real;
    if(my_fabs(err) > PI_data->integral_deadzone){
        PI_data->integral += err;

        if(my_fabs(PI_data->integral) > PI_data->integral_limit){
            PI_data->integral = PI_data->integral > 0.0f ? PI_data->integral_limit : -PI_data->integral_limit;
        }
    }

    return PI_data->Kp * err + PI_data->Ki * PI_data->integral;
}


// PID 计算函数
// 输入：设定电压 target_voltage，实际电压 actual_voltage
// 输出：PWM pulse 值
int i16_pid_calcu_addition(float target_voltage, float actual_voltage) {
    return 0;
}