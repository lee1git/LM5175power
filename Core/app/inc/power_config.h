#ifndef _POWER_CONFIG_H
#define _POWER_CONFIG_H

#include "powerMaster.h"


//limit
#define POWER_MAX_VOLTAGE           (15.0f)
#define POWER_MIN_VOLTAGE           (1.0f)
#define PWM_VLOTAGE_TOP             (324U)     //when pluse is 324,zhe Vout is 15V
#define PWM_VLOTAGE_buttom          (758U)     //when pluse is 758,zhe Vout is 1V
#define PWM_VLOTAGE_SLACK           (10U)
#define PWM_VOLTAGE_PULSE_MAX       (PWM_VLOTAGE_buttom + PWM_VLOTAGE_SLACK)
#define PWM_VOLTAGE_PULSE_MIN       (PWM_VLOTAGE_TOP - PWM_VLOTAGE_SLACK)

//step
#define POWER_VOLTAGE_UP_STEP       (1.0f)
#define POWER_VOLTAGE_DOWN_STEP     (-1.0f)

//setup
#define POWER_SETUP_OUT_STATE       (PWR_EN_OFF)
#define POWER_SETUP_VOLTAGE_SET     (6.0f)
#define POWER_SETUP_CURRENT_LIMIT   (10.0f)

//wdog
#define IWDOG_FEED_TIME_MS          (300)   //300ms

/* 传感器错误计数上限 */
#define SENSOR_ERROR_THRESHOLD  5
/* I2C 总线锁最长等待：约两倍最坏持锁时间（一次事务 5ms HAL 超时） */
#define I2C_LOCK_TIMEOUT_TICKS    (pdMS_TO_TICKS(10))
/* PowerState 锁最长等待：临界区只有几次字段读写，5ms 足够宽松 */
#define POWERSTATE_LOCK_TIMEOUT_TICKS  (pdMS_TO_TICKS(5))


//calculate
#define VOLTAGE_TO_PWM_PULSE(_vol)  \
    (PWM_VLOTAGE_buttom - (uint32_t)((float)(PWM_VLOTAGE_buttom-PWM_VLOTAGE_TOP)/(POWER_MAX_VOLTAGE-POWER_MIN_VOLTAGE)*((float)_vol-1.0f)))    //_vol must between 1.0f - 15.0f

#endif // !_POWER_CONFIG_H
