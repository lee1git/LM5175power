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

## 2026-09-17 · 第 2 次：仓库只留源代码与 hex

### 【我】.gitignore
- 整体屏蔽 /Drivers 与 /Middlewares，第三方库不进仓库。
- 增加系统与编辑器垃圾文件规则。

### 【我】MDK-ARM/.gitignore
- 屏蔽 .cmsis / .pack / .eide / .vscode 与 RTE 整个目录。
- 屏蔽 uvguix / lst / code-workspace 等杂项。
- 编译目录只留文件夹壳，额外放行 power_control.hex。
- 屏蔽两个 RTE_Components.h：uVision 与 EIDE 各自生成的。
- 模式行注释改为单独成行，避免被当成模式。

### 【我】待办
- 在 GitHub Desktop 里提交 1168 个文件的删除并 Push。

## 2026-09-17 · 第 3 次：sensor 驱动重构完成度核对

### 【我】已完成部分（只读核对，未改代码）
- 四个传感器函数统一为状态码加输出指针。
- 空指针、LSB 与 HAL 失败分支均已补齐。
- readCuttent 符合单向、0.01A 判废规则。
- main.c 三处调用与 INA226_init 已同步签名。

### 【我】未完成部分
- sensor_res 连赋三次未判错，错误分拣仍空。
- 队列 msg_size 仍为 sizeof(float)，非结构体。
- temperature 与 current 读完未用、也未上报。
- buf / sendLen / count / pulse 成死变量。
- sensors.h 缺 #define _SENSORS_H_，保护失效。
- Sensors_init 与 I2C_Restart 只有声明无实现。
- HIGH8IN16 / LOW8IN16 定义了却没用到。

### 【我】顺带查出两处真错
- INA226_init 用 (uint8_t*)&buf 写寄存器，小端反序。
- readVoltage 拿原始码值比 0.1f，量纲不对。

### 【我】告警说明
- 上次编译 0 错 13 警；temperature/current 不告警。

## 2026-09-17 · 第 4 次：sensor 重构收尾核对

### 【我】核对结论（只读，未改代码）
- 四个函数全部统一到新 API，驱动层可算收尾。
- 第 3 次报的两处真错已按新写法改掉。

### 【我】本轮确认已完成
- INA226_init 改 tx_data 双字节，大端次序对了。
- readVoltage 换 Mem_Read，改成输出指针式。
- sensors.h 补 #define _SENSORS_H_，保护生效。
- powerMaster.h 补保护宏与文件末尾换行。
- I2C_Restart 补 (void)，main.c 死变量清完。
- 编译由 13 警降到 1 警，只剩 sensor_res。

### 【我】剩下的驱动层遗留
- sensors.c 第 4 行 extern hi2c1 已无人使用。
- sensors.h 的 float.h 已无人用 FLT_MIN。
- readVoltage 死区只挡小于 -0.1V，负值会漏过。
- Sensors_init 与 I2C_Restart 仍只有声明无实现。

### 【我】按你要求延后，不计未完成
- 错误分拣：sensor_res 未判错，即唯一告警。
- UART 上报与队列结构体上报。

### 【我】顺带看到
- uvprojx 编译器由 ARMCLANG V6.24 换回 ARMCC V5.06。
- 优化等级由 -O2 改 -O1，与传感器逻辑无关。

## 2026-09-18 · 第 5 次：传感器分层大调整核对

### 【我】结构调整（只读核对，未改代码）
- sensors 拆到 Core/sensors 与 inc，工程已加入。
- 旧 Core/Inc/sensors.h 与 Core/Src/sensors.c 已删。
- 新增 sensors_dev 设备层，只实现了 write_reg。

### 【我】已完成
- I2C_sensor_dev_write_reg 映射完整，超时按忙处理。
- I2C2/I2C3 返回未实现，不冒充成功。
- INA226_dev_init 迁移完成，大端字节序正确。

### 【我】真错一：calibration 传错变量
- init 传给 dev_init 的是枚举 0，不是寄存器值。
- 告警 sensors.c(37) 已点名 calibration 未被使用。
- 后果：CAL 写 0，电流读数恒 0，永远判废。

### 【我】真错二：忙分支空转且假成功
- ERR_BUSY 分支只有空注释，随后落到 return OK。
- 初始化失败也会打印 INA:initOK。

### 【我】未完成
- 设备层没有 read_reg，三个读函数仍直连 HAL。
- 读函数收 hi2c 指针，init 却用 I2C_BUS，两套寻址。
- 三个状态枚举并存，init 把错误压成 ERR_DEVI。
- I2C_Restart 仍只有声明没有实现。

### 【我】重复头文件
- Core/Inc/sensors_dev.h 与 sensors/inc 同名同保护。
- Core/Inc 那份缺两个字节宏，且路径顺序先命中。
- .d 文件证实两副本都在用，建议删 Core/Inc 那份。

### 【我】小问题
- I2C_BUS 非 static，总线硬编码在 sensors.c。
- INA226AIDGSR.h 末尾缺换行，即那条告警。
- write_reg 未查 data 空指针与 len 为 0。
- 读函数失败时不写输出参数，调用方会拿到脏值。
- config 先于 calibration 写，有 CAL=0 窗口。

### 【我】延后项不计未完成
- sensor_res 未判错，UART 与队列上报。

## 2026-09-18 · 第 6 次：新增 I2C_sensor_dev_read_reg

### 【我】Core/sensors/sensors_dev.c
- 新增 I2C_sensor_dev_read_reg，读 8 位寄存器地址的数据。
- 形参与 write 齐平：总线枚举、addr、regaddr、缓冲、长度。
- 总线选择与超时按忙的错误映射与 write 完全一致。
- 多加了 data 空指针与 len 为 0 校验，读是输出缓冲。
- 只加这一个函数，write 与三个上层读函数都没动。

### 【我】Core/sensors/inc/sensors_dev.h
- 同步新增原型与注释，注明 16 位寄存器传 2 字节。
- 数据高位在前，与现有拼装方式一致。

### 【我】Core/Inc/sensors_dev.h
- 重复副本也同步了原型，免得调用方吃到旧副本。
- 该副本仍是隐患，建议删掉。

### 【我】验证方式
- 用 armcc V5.06u6 按工程宏与参数单独编译三个文件。
- 工程 include 顺序与前置 sensors/inc 顺序都 0 错 0 警。
- 目标文件里已确认生成该符号。
- 未跑工程整体编译，uVision 正在运行，避免动 hex。

## 待办
- UART 上位机通讯独立成任务，用队列传上报结构体统一上报。
- 逆流时直接关芯片保护，方案待定。
- 错误分拣方法待建立。
- 上报改用结构体时，osMessageQueueNew 的 msg_size 要同步改。
- 注释掉上报段后，temperature / current / res 暂成未使用变量（仅告警）。
