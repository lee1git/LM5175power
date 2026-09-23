# 修改备忘录（第三册）

> 规则：只新增条目，不回改旧条目；单条修改描述少于 40 汉字。
> 【我】= AI 修改，【你】= 用户修改。
> 编号：第 N 次为累计次数，括号内为本日第 M 次。
> 篇幅：满 30 条或 12000 字符即另起新 md，累计编号继续，本日编号按日重算。
> 本册接 MEMO2.md：该册已 17566 字符，超 12000 上限，故按规则另起。

## 2026-09-23 · 第 34 次（本日第 1 次）：PI 化与看门狗落地后的全项目筛查

### 【你】本轮改动
- PID 简化为 PI，独立成 Core/algorithm/pid.c 与 pid.h。
- 新增 IWDG 初始化与 iwdog_feed 任务，任务总数到 7。
- .ioc 同步登记看门狗任务、优先级与二参数。
- 已提交 819b46f，含 .ioc、pid 两文件与看门狗。

### 【我】筛查口径
- 通读 Core 全部自研源码、构建配置、构建日志与 map。
- 结论逐条查调用点，不猜；都带文件与行号。
- git diff -w --ignore-cr-at-eol 为空：无真实未提交改动。
- 38 条 M 全是行尾差异，与第 28 次结论一致。
- EIDE 结论按清单推断，未实际跑构建复现。

### 【我】确认的真错（必修）
- INA226 校准寄存器从未写入：init 只在离线分支被调。
- 故开机电流恒为 0，电流限制形同虚设，需上板确认。
- Vlotage_pid 首次读 new_voltage 时该量未初始化。
- sensorRead 总线离线分支三个 state_res 未初始化。
- 该分支也不置 SENSOR_SKIP，可能写回旧值并刷时间戳。
- PWM 初值不一致：寄存器 324，PI 内部 now_pulse 500。
- 首轮按 now_voltage=0 闭环，使能瞬间跳到约 11V。
- 限幅 300/800 与标定 324/758 不符，可越出 1 至 15V。
- now_pulse 为无符号，先加后限幅会朝反方向跑。
- IWDG 超时约 1s 而喂狗 800ms，最坏 0.67s 会复位。
- 喂狗任务无条件刷新，低优先级任务卡死查不出。
- 未冻结调试期 IWDG，断点停超 1s 即复位。
- I2C2 全程无锁，屏任务与恢复任务互相破坏总线。
- uart_dev 的 uart 指针未初始化，非 UART2 即野指针。

### 【我】风险与工程债
- main.c 残留旧 PID：pid_calculate 与 7 个全局量已死。
- pid.c 的 i16_pid_calcu_addition 是空壳，头文件仍声明。
- I2C_dev_lock 与 unlock 两层都无人调用。
- 该锁的 I2C2 分支直接返回 -1，等于该总线无保护。
- sensors_outerdev_init 无任何调用者。
- TMP112 上限判据 200 度不可达，超 125 度仍算有效。
- I2C 恢复持锁约 18ms，超过 10ms 取锁超时，误判离线。
- 未开栈溢出与 malloc 失败钩子，任务句柄未判空。
- DEV_UART_DEBUG 未定义，水位与打印都是死代码。
- run_count 只声明不累加，打印恒为 0。
- 死变量剩 PowerState_copy_last；voltage_change 已不存在。
- eide.yml 重复登记 powerMaster.c，产物出现 _1 后缀。
- 无 .gitattributes，工作区 30 条纯行尾噪音。
- 备份目录 .bak 已被忽略不入库，建议清理。
- NDEBUG 未定义，assert 在发布构建里仍生效。
- EIDE 源清单漏 HAL 的 iwdg 驱动，主线链接不出固件。
- EIDE 的 excludeList 前缀过时，42 条排除全部失效。
- 两套构建宏不一致，EIDE 多一个 STM32F10X_MD。
- 被 RTE 规则排除的 RTE_Components.h 其实已入库。
- DebugConfig 与 .scvd 已入库，与 README 口径相悖。
- .git 已 42MB、1706 个松散对象，从未 gc。
- picture 占已跟踪体积的 66%，别再加大图进去。

### 【我】文档口径
- README 尺寸 43.1/14.4KB 与 map 的 41.31/14.09KB 不符。
- README 任务表 PIDv 20ms、screen 200ms 与代码不符。
- README 未列 iwdog_feed，也未提看门狗与 PI。
- README 称增量式 PID，实现是 PI 加累加，需改口径。
- README 写有新鲜度检查，但代码无人读时间戳。
- README 称离线即闭锁 PWM，实际是冻结比较值。

## 待办（第 34 次）
- 上板确认开机 Iout 是否为 0，定 INA226 初始化时机。
- IWDG 重载或喂狗周期收到最坏超时的三分之一以内。
- 喂狗改成按任务心跳，而非无条件刷新。
- 冻结调试期 IWDG，避免断点即复位。
- state_res 与三个器件状态量改在循环开头初始化。
- 总线离线分支置 SENSOR_SKIP，不写回不刷时间戳。
- PWM 初值改读寄存器，首轮等有效采样再闭环。
- 限幅改用 324/758，now_pulse 改有符号且先限后加。
- 给 I2C2 加锁，或让屏恢复走同一把总线锁。
- uart_dev 补 else 与参数校验，返回未实现错误码。
- 删掉 main.c 旧 PID 与空壳函数，清理死 extern。
- PB13 开关的空分支补上，或明确不做。
- 开栈溢出与 malloc 失败钩子，实测水位与堆峰值。
- 在两处构建定义 DEV_UART_DEBUG，并给计数器累加。
- eide.yml 去掉重复登记，确认 _1 产物消失。
- EIDE 清单补 stm32f1xx_hal_iwdg.c，让主线能链接。
- 修 excludeList 前缀，真正排掉不用的 u8g2 文件。
- 统一两套构建的宏，去掉多余的 STM32F10X_MD。
- 处理已入库的 RTE_Components.h 与 DebugConfig。
- 跑一次 git gc，并理清 hex 该由哪套构建产出。
- 加 .gitattributes 并 renormalize，清掉行尾噪音。
- README 尺寸、任务表与控制口径按代码重写。
- 12 小时考核跑完回填最终结果。
- 推送前删掉 8.7MB 原图，只留 web 版。
