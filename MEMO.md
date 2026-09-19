# 修改备忘录

> 规则：只新增条目，不回改旧条目；单条修改描述少于 40 汉字。
> 【我】= AI 修改，【你】= 用户修改。
> 编号：第 N 次为累计次数，括号内为本日第 M 次。
> 篇幅：满 30 条或 12000 字符即另起新 md，累计编号继续，本日编号按日重算。

## 2026-09-17 · 第 1 次（本日第 1 次）：INA226 电流读取接口对齐

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

## 2026-09-17 · 第 2 次（本日第 2 次）：仓库只留源代码与 hex

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

## 2026-09-17 · 第 3 次（本日第 3 次）：sensor 驱动重构完成度核对

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

## 2026-09-17 · 第 4 次（本日第 4 次）：sensor 重构收尾核对

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

## 2026-09-18 · 第 5 次（本日第 1 次）：传感器分层大调整核对

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

## 2026-09-18 · 第 6 次（本日第 2 次）：新增 I2C_sensor_dev_read_reg

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

## 2026-09-18 · 第 7 次（本日第 3 次）：sensors 收尾评估 + CubeMX 拆分外设

### 【我】sensors 结构评估（只读，未改代码）
- 三层已成形：服务层、器件驱动、传感器设备层。
- 服务层 sensors.c 已无 HAL 调用、无句柄，只剩 float 指针。
- TMP112A 与 INA226 各自独立成文件，错误枚举收成两层。
- 结论：分层合格，屏蔽还差 LSB 一环。

### 【我】本轮查出待修项
- sensors_dev.h 重定义 NULL，实测告警 47-D。
- INVALID_DATA_FLOAT 是死宏，赋给 float 也不是 NaN。
- 驱动层仍 extern hi2c1，反向依赖未解，锁拿不到。
- I2C 重试四份复制，且紧循环无延时。
- 0.01A 与逆流判废规则在重构中消失，待确认。
- I2C_Restart 仍只有声明没有实现。

### 【你】CubeMX 拆分外设初始化
- .ioc 已开 CoupleFile，生成 gpio/i2c/tim/usart 成对文件。
- 句柄随之外移：hi2c1 在 i2c.c，htim2 与 htim3 在 tim.c。
- stm32f1xx_hal_msp.c 相应瘦身 247 行。
- Keil 工程自动加入新文件，sensors 分组保住了。
- 编译器、优化等级与输出目录都没被重置。

### 【我】代价：应用代码被清掉 542 行
- 丢的是 buttomTask、Vlotage_pid、Callback01 三个任务体。
- 它们原本写在 main.c 的 USER CODE 区里。
- 但这三个函数归属 freertos.c，CubeMX 不认 main.c 的副本。
- 新 freertos.c 任务体已空，定时器只创建从未启动。
- 传感器采样回路目前没有调用者，旧代码可从 git 取回。

## 2026-09-18 · 第 8 次（本日第 4 次）：恢复任务体 + 总线注入 + Core/app

### 【我】Core/Src/freertos.c
- 从 git 取回 buttomTask、Vlotage_pid、Callback01 三段。
- 补回 osTimerStart，10ms 采样周期恢复。
- USER CODE 区补 sensors、powerMaster、tim、usart 头。
- 补 pid_calculate 原型与 integral 等外部声明。
- 新增弱钩子强定义，用 I2CAccess 锁总线。

### 【我】Core/sensors/sensors_dev.c 与 .h
- 新增 I2C_sensor_dev_init 注入总线句柄。
- 删掉手写 extern hi2c1，类型改由 i2c.h 提供。
- 读写与 I2C_Restart 全部加锁，Restart 加空句柄保护。
- 新增两个弱锁钩子，默认空实现不依赖 RTOS。

### 【我】Core/sensors/sensors.c 与 inc/sensors.h
- 实现 sensors_dev_init()，内部绑定 &hi2c1。
- 设备头加 stm32f1xx_hal.h，提供句柄类型。

### 【我】Core/app 迁移
- powerMaster.c/h 移到 Core/app 与 Core/app/inc。
- 同步 uvprojx、uvoptx 与 eide.yml 的路径与 include。
- 今后应用模块放这里，CubeMX 生成区不会再碰。

### 【我】验证与顺带发现
- armcc 单独编译七个 TU，除一条告警外全干净。
- freertos.c 原漏包含 sensors.h，已补，否则隐式声明。
- sensors.c 第 111 行枚举混用：设备码赋给状态码。
- sensors_outerdev_init 与声明名 sensors_dev_init 不一致。
- TMP112A 两处文件末尾缺换行，已补。

## 2026-09-18 · 第 9 次（本日第 5 次）：逆流与 LSB 定调 + 采样架构待办

### 【你】三条已定决策
- 驱动层不判逆流，负电流照实返回，交上层判断。
- LSB 暂保持应用传入，它与标定枚举绑定的固定值。
- 锁的适配之后再改，本轮不动。

### 【我】待办：控制带宽先要定
- INA226 配置 0x4127 是 AVG=128，更新周期约 159ms。
- 10ms 采样只是重复读同一笔数据，白占总线。
- 二选一：把 AVG 降到 4（约 9ms），或回路降到约 150ms。
- TMP112 十二位转换约 26ms，10ms 读一次同样没意义。

### 【我】待办：采样与控制回路
- 不要拆三个任务，同一条 I2C 总线只该有一个采样任务。
- 采样任务用 osDelayUntil 定周期，再按计数分频。
- 采样值写 PowerState，用 PowerStateAcssessHandle 保护。
- PID 任务用 osDelayUntil 定周期，短临界区拷贝后算 PID。
- 删掉 dataRead_TIM 软定时器，阻塞 I2C 不该待在 Tmr Svc。
- power_infoHandle 腾给 UART 上报结构体，msg_size 同改。

### 【我】待办：策略归属与收尾
- 逆流与 0.01A 判废放 Core/app，不要放器件层。
- LSB 与 CAL 值建议同处定义并注释，免得两处漂移。

## 2026-09-18 · 第 10 次（本日第 6 次）：锁语义与释放修复 + 按钮管线待办

### 【我】锁钩子改为返回状态
- I2C_sensor_dev_lock 改 int，0 持锁、非 0 未取到。
- 应用侧超时 10ms，内核启动前句柄为空直接放行。
- 没取到锁时设备层直接返回 ERR_BUSY，不碰总线。
- 读写与 I2C_Restart 三处都按结果判断。

### 【我】修掉无条件释放
- buttomTask 原先取锁失败也释放，改为只在 osOK 时释放。
- 同处把 GPIO 读取移出临界区，锁内只改状态。
- Vlotage_pid 同样只在 osOK 时释放。
- PowerState 弱函数原为空体 int，已补 return 0。
- PowerState 锁改 int 加 5ms 超时，超时宏放 PD 区。

### 【我】命名收口
- 总线绑定函数改名 Sensors_bus_init，与 restart 成对。
- 声明补进 sensors.h，消掉 main.c 的隐式声明。

### 【我】验证
- armcc 单独编译七个 TU，0 错 0 警。
- 顺带确认 outerdev_init 的枚举混用已修好。

### 【你】待办：按钮手势管线
- ISR 只负责唤醒，手势判定与动作执行分层。
- EXTI 现为仅下降沿，长按需周期 tick 或改双边沿。
- ISR 未带引脚信息，多键与双击需要每脚状态机。
- 手势逻辑放 Core/app，事件用队列传枚举给 buttomTask。

## 2026-09-18 · 第 11 次（本日第 7 次）：INA226 位域更正，第 9 次带宽待办作废

### 【你】手册 CONFIG(00h) 位域实证
- AVG 在 D11:D9，不在我原先以为的 D14:D12。
- VBUSCT 在 D8:D6，VSHCT 在 D5:D3。
- D14:D12 是保留位，POR 值为 100。
- 由表拼出 POR 即 0x4127，其 AVG 字段为 000。

### 【我】更正与撤回
- AVG=000 即 1 次平均，不是 128 次。
- 0x4127 两种边界都能凑出，故看默认值验不出。
- 正确周期为 1 乘 2.2ms 等于 2.2ms，约 455Hz。
- 结论：10ms 采样与回路匹配，配置无需改动。
- TMP112 也按手册 10ms 计，撤回 26ms 那句。

### 【我】第 9 次作废范围
- 整组"控制带宽先要定"的待办作废。
- 降 AVG 或降回路周期的建议一并撤回。

### 【我】留档：AVG 对照
- AVG 1 用 0x4127，4 用 0x4327，16 用 0x4527。
- 64 用 0x4727，128 用 0x4927，周期随之翻倍。
- 若日后电流抖动偏大，可加平均，代价是更新变慢。

## 2026-09-18 · 第 12 次（本日第 8 次）：goto 收尾范式定为硬性规定 + 新回路核对

### 【你】硬性规定：goto 只跳到尾部
- goto 一律跳到尾部收尾标签，不用一层层 if 包裹主体。
- 尾部标签之后只放周期延时，收尾与延时同处出口。
- 提前退出与跳过分支全部走同一条尾部出口。
- 三个任务体现已如此，今后新代码同样照此办。

### 【我】范式的配套纪律（随硬性规定一并生效）
- 跳到尾部与自然落入尾部是两条路径，收尾只写在跳转前必漏。
- 每条 goto 分支的解锁、状态回写要与自然落入处一字不差。
- 收尾若有多处重复，宁可重复也不能少写，缺一处就是死锁。

### 【我】真错：Vlotage_pid 成功路径漏解锁
- 三条 goto 分支都调了 PowerState_unlock，自然落入那条没调。
- PowerStateAcssessHandle 是非递归互斥量，本任务再取必超时。
- 后果：PID 第二轮起只走跳过分支，采样任务再也拿不到锁。
- 修法：在尾部标签前补一次 PowerState_unlock()。

### 【我】真错：当前代码编译不过（armcc 实测，14 错 3 警）
- powerMaster.h 用了 bool 却未包含 stdbool.h，三个 TU 共 9 错。
- 同一头文件用 TickType_t 却未带 FreeRTOS 头，powerMaster.c 三处错。
- PowerState 字段实名 now_voltaga，freertos.c 写成 now_voltage，两处错。
- 三个未使用变量告警：voltage_change、osRes、last_vlotage。

### 【我】偏差：delayUntil 的基准被循环内重置
- lastWakeTime 赋在循环体开头，等于每轮重新取当前节拍。
- 周期因此变成 20ms 加执行时间，还带抖动，不是严格 20ms。
- 修法：初始化移到 for(;;) 之前，基准只在循环外取一次。

### 【我】偏差：INA226 错误计数自相矛盾
- current_err_count 到阈值后从不复位，电压那路只复位自己。
- 同一轮先判离线，随后任一成功又写回在线，离线判定被覆盖。
- 净效果：INA226 的阈值机制等于失效，只有 TMP112 那路自洽。

### 【我】留档：PID 与临界区现状
- pid_calculate 返回相对基准的增量，now_pulse=500 是基准非累加值。
- 建议 now_pulse 改名 PWM_BASE 并加 const，免被读成增量式。
- 积分项无上限也不抗饱和，长时间偏离会绕死，本轮只记录。
- 建议 powerMaster.h 自带 FreeRTOS.h 或 bool 头，免得靠包含顺序。

## 2026-09-19 · 第 13 次（本日第 1 次）：采样与 PID 回路落地核对

### 【你】任务体重构
- 新建 sensorTask 承接 20ms 采样，Callback01 已清空。
- PID 与采样均改 vTaskDelayUntil 定周期，PID 加 check_over 闸门。
- PID 新增电压时间戳新鲜度检查，超 1s 未更新即复位积分。
- PowerState 字段拼写统一 now_voltage，头文件补 cmsis_os.h。
- 三个 bool 字段改 char，绕开 stdbool.h。
- 死变量 voltage_change、osRes、last_vlotage 已删。
- Vlotage_pid 尾部补回 PowerState_unlock，锁泄漏已修。

### 【我】本轮修改（共三处）
- 两个任务的 lastWakeTime 初始化移出循环体，全程只取一次。
- 否则周期等于 20ms 加执行时间，拍长在漂，PID 参数跟着漂。
- check_over 判据反了：检查全过时不跑 PID，故障时反而跑一次。
- 已改为每轮先置 1，任一故障清 0，判据改 if(check_over == 1)。

### 【我】复验
- armcc 按工程参数单独编译七个 TU，0 错 0 警。
- uVision 在跑，未做工程整体编译，未动 hex。

### 【我】待你定：器件状态位不再回到在线
- 数据回写段已删掉 DEVICE_ONLINE，只剩三处离线判定。
- PowerState 是全局量，零初始化即离线，故两位开机就是离线。
- 现状：状态位只能表达离线，在线与否改由时间戳新鲜度表达。
- 若有意如此，建议两位改名故障锁存位并加注释写明语义。

### 【我】留档：剩余小项
- dataRead_TIM 仍按 10ms 起跑且回调为空，建议退役。
- now_pulse 语义是基准值不是累加值，建议改名 PWM_BASE 加 const。
- 积分项无上限，1s 时间戳检查兜了一部分，抗饱和仍缺。

## 2026-09-19 · 第 14 次（本日第 2 次）：默认在线初始化 + 断电后检查缺口

### 【你】日志篇幅改口径
- 上限由 4000 词与 50 条，改为 30 条或 12000 字符。

### 【你】PowerState 初始化改默认在线
- 三个在线位在 PowerStateInit 里显式置 DEVICE_ONLINE。
- 入参改 assert 前置校验，越界即停，不再兜底改默认值。
- 错误处理尚未做，故语义只能是开局在线、运行中降级。

### 【我】断电后检查逻辑缺口（只读核对）
- Sensors_bus_restart 与 I2C_Restart 只有定义，零调用者。
- 掉电时从机若拉住 SDA，首次通讯必失败，无人复位总线。
- 失败只累加计数，三拍后置离线，器件不再尝试恢复。
- 开机无器件存在性自检，INA226 的 ID 寄存器闲置。
- 复位原因未记录，掉电重启与看门狗复位无法区分。
- pgood_statu 只在 init 里写 1，之后无人更新。
- .ioc 未接 PGOOD，LM5175 输出异常 MCU 看不见。
- 输出关只跳过 PID，CCR 停在上次值，重开无软启动。
- I2C1_state 无人降级，总线错也只记到两个器件位上。

### 【我】留档：时间戳判据本身没问题
- init 清 0 加 tick 从 0 起，开机 1s 内不会误判过期。
- tick 为 32 位 ms，满 49.7 天回绕，差值写法照样成立。

## 2026-09-19 · 第 15 次（本日第 3 次）：assert 漏头文件，链接必失败

### 【我】真错：新增断言缺 assert.h
- powerMaster.c 三处与 sensors_dev.c 两处新增 assert。
- assert 只是宏，缺头文件就退化成一次普通函数调用。
- 编译期只报 223-D 隐式声明，链接期才是致命的。
- 已实测复现：单独链接报 L6218E Undefined symbol assert。
- 修法：两文件各补 #include <assert.h>，复验回 0 错 0 警。

### 【我】留档：断言在裸机上的后果
- 标准实现落到 __aeabi_assert，打印后即挂死。
- 无调试器时可能停在 BKPT，等于现场静默停机。
- 若不想要这行为，可定义 NDEBUG 或换成自己的处理。

## 待办
- 删掉 dataRead_TIM 软定时器，Callback01 已空，留着只白跑节拍。
- 器件在线位语义待定：一旦离线不再回在线，见第 13 次。
- 电源回路 PID 是否加积分限幅与抗饱和，待定。
- UART 上位机通讯独立成任务，用队列传上报结构体统一上报。
- 逆流时直接关芯片保护，方案待定。
- 错误分拣方法待建立。
- 上报改用结构体时，osMessageQueueNew 的 msg_size 要同步改。
- 注释掉上报段后，temperature / current / res 暂成未使用变量（仅告警）。
- 上电做一次 INA226 的 ID 自检，失败即置离线。
- 错误计数触发时调 Sensors_bus_restart 并清计数，限次重试。
- 记录 RCC 复位标志，用于区分掉电与看门狗复位。
- 输出关闭时把 CCR 归到安全值，并考虑加软启动。
- 断言失败怎么办待定：定义 NDEBUG，或换自定义上报。
