# 修改备忘录

> 规则：只新增条目，不回改旧条目；单条修改描述少于 40 汉字。
> 【我】= AI 修改，【你】= 用户修改。

## 2026-09-17 · 第 1 次：INA226 电流读取接口对齐

### 【我】Core/Src/sensors.c · INA226_readCuttent
- 返回改 sensor_fstate_t，形参加 hi2c 与 float* currentOut。
- 读寄存器改 HAL_I2C_Mem_Read，失败用 switch 转 SENSOR_ERR_*。
- 加空指针与 LSB 校验。

### 【我】Core/Inc/sensors.h
- 同步函数原型，增加 INA226_MAX_CURRENT 量程宏。

### 【你】Core/Src/sensors.c · INA226_readCuttent
- 删十安培上限校验，改小于 0.01A 即判非法（单向）。
- 保留死区：逆流与小电流都返回无效。

### 【你】Core/Inc/sensors.h
- 删掉 INA226_MAX_CURRENT 宏。

### 【你】Core/Src/main.c · Callback01
- 改按新签名传参，sensor_res 改 sensor_fstate_t。
- 注释掉 UART 上报段，改由队列上报。

## 待办
- UART 上位机通讯独立成任务，用队列传上报结构体统一上报。
- 逆流时直接关芯片保护，方案待定。
- 错误分拣方法待建立。
- 上报改用结构体时，osMessageQueueNew 的 msg_size 要同步改。
- 注释掉上报段后，temperature / current / res 暂成未使用变量（仅告警）。
