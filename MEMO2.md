# 修改备忘录（第二册）

> 规则：只新增条目，不回改旧条目；单条修改描述少于 40 汉字。
> 【我】= AI 修改，【你】= 用户修改。
> 编号：第 N 次为累计次数，括号内为本日第 M 次。
> 篇幅：满 30 条或 12000 字符即另起新 md，累计编号继续，本日编号按日重算。
> 本册接 MEMO.md：该册已 13742 字符，超 12000 上限，故按规则另起。

## 2026-09-19 · 第 16 次（本日第 4 次）：uv* 与 RTE_Components.h 归属核对

### 【我】uv* 各是什么（只读核对，未改文件）
- uvprojx 是工程本体：源文件清单、include、宏、输出设置。
- uvoptx 是调试选项：断点、下载算法、打开的文件标签。
- uvguix.* 是窗口布局，跟 Windows 用户走，已屏蔽。
- lst 是汇编列表，属编译产物，已屏蔽。

### 【我】RTE_Components.h 是什么
- uVision 的 RTE 组件管理器生成，文件头写 DO NOT EDIT。
- 本工程内容仅一行：CMSIS_device_header 指向 stm32f10x.h。
- EIDE 在 MDK-ARM 根另生成同名一份，内容不同。

### 【我】屏蔽无影响的证据
- 全仓无一处包含它，引用只在 CMSIS 模板例程中。
- freertos_os2.h 用的 CMSIS_device_header 另有来源。
- 该宏由 Core/Inc/FreeRTOSConfig.h 定义，指向 stm32f1xx.h。
- 那才是 HAL 工程该用的头，RTE 那份反而是标准库头。
- uvprojx 的 RTE 段为空壳，不引用该文件。
- uVision 打开工程会自动重建 RTE 目录。
- 结论：两份均已屏蔽且无影响，不必手工改它。

### 【我】关键机制：ignore 管不了已跟踪文件
- 当前 43 个跟踪文件，其中 RTE_Components.h 与 uvprojx、uvoptx 在列。
- 已跟踪文件不受屏蔽规则影响，故它们仍显示 M。
- 要清掉必须 git rm --cached 后提交。

### 【我】屏蔽 uvprojx 的代价（待你定，未动 .gitignore）
- 它记录 Core/app 与 Core/sensors 两个手加分组的文件清单。
- 也记录编译器 ARMCC V5.06、优化 O1、输出目录与 hex 开关。
- 屏蔽后仓库只剩源码与 .ioc，EIDE 工程同样已被屏蔽。
- CubeMX 重生成工程不会自动加回那两个自定义分组。

### 【我】建议
- uvoptx 可屏蔽：只丢断点与下载配置，uVision 会重建。
- uvprojx 建议保留，否则每次克隆都得手工重搭工程。

## 待你指令
- uvprojx / uvoptx 是否屏蔽，等你定。
- DebugConfig 与 EventRecorderStub.scvd 同属生成物，可选屏蔽。

## 2026-09-19 · 第 17 次（本日第 5 次）：uVision 无法 Download 排查

### 【我】已排除项（只读核对，未改文件）
- 编译没问题：日志末行 0 错 0 警，axf 与 hex 刚生成。
- 工具链 ARMCC V5.06u5 在位，路径 D:\Keil5\ARM\ARMCC。
- 调试驱动 CMSIS_AGDI.dll 1.33.24.0 在位，正是 DAP 用的那个。
- 器件包只装了 2.2.0，工程现绑 2.2.0，FLM 与 SVD 都在。
- Flash 算法串前后一致：128.FLM 于 08000000，长 020000。
- Utilities 页与算法选择前后完全一致。
- 固件没占 SWD：无 PA13/PA14，NOJTAG 保留 SW-DP。
- dbgconf 与 scvd 未被工程引用，属死文件。

### 【我】重新生成只改了这些
- uvoptx 的 nTsel 由 3 变成 2。
- 调试串改成新版写法，与 CW32_OSC 工程同款。
- 本机 nTsel 取 2 与取 3 都配 CMSIS_AGDI，故非病根。
- uvprojx 的器件包号由 2.4.1 改回 2.2.0。

### 【我】真结论：仓库里那份工程文件包号是错的
- committed 的 uvprojx 写 2.4.1，本机并没有装。
- 若 git 还原它，uVision 找不到 FLM，下载必失败。
- 故清理 git 时别还原 MDK-ARM 的两个工程文件。

### 【我】待你给报错原文
- 日志里只有一次成功编译，没有下载记录。
- 缺报错文字，无法定位到具体哪一环。
- 若是找不到调试器：查 USB 口、驱动、是否被占用。
- 若是无法访问目标：Connect 改 under Reset 再试。
- 若报错来路不明：完全关闭 uVision 重开再试。

## 2026-09-19 · 第 18 次（本日第 6 次）：DAPLink 身份与 USB 通道核查

### 【我】适配器身份（只读核对，未改文件）
- E 盘卷标 DAPLINK，VID_0D28 加 PID_0204，是正版 mbed DAPLink。
- 接口固件版本 0253，属 2016 年产物，Local Mods 为 1。
- 自述支持 MSD、CDC、HID、WebUSB 四种接口。

### 【我】四个接口全部 Started，驱动均正常
- MSD 挂成 E 盘，CDC 是 COM18，另有 HID 与 WebUSB 各一条。
- HID 那条即 CMSIS-DAP v1，WebUSB 那条即 v2。
- 故不是线材、端口、驱动缺失或枚举失败所致。

### 【我】版本代差是最大嫌疑
- 适配器固件停在 2016，而 uVision 与驱动是 2025 年的。
- 拖 hex 只走 MSD，能烧只证明 SWD 物理链路通。
- 调试与下载走 HID 或 WinUSB，与 MSD 通道无关。

### 【我】首要试法：禁用 WebUSB 那条接口
- 设备管理器里禁用 MI_04 的 WebUSB: CMSIS-DAP。
- 逼 uVision 退回 HID 老通道，可逆，随时启用回来。
- 若随即能连，即坐实 v2 与老固件不合。

### 【我】次要试法
- Debug 设置里显式选中你这台，别停在 Any。
- 对照工程 BH-F103 存的是具体型号加序列号。
- Connect 改回 Normal，under Reset 反而连不上。

### 【我】待你回一个数据
- Debug 设置里 SW Device 显示的 IDCODE 是多少。
- 1BA01477 是正品 F103，别的值就要怀疑片子。
- 刷固件有风险，Bootloader 同为 0253，先别动手。

## 2026-09-19 · 第 19 次（本日第 7 次）：芯片没错，是工程里存的 ID 不匹配

### 【我】芯片真实身份（CubeProgrammer 只读连接）
- Device ID 0x410，中容量 F103，NVM 64KB，Cortex-M3。
- Revision 为 Rev A，电压 3.26V，正是工程选的那颗 C8。
- 故"芯片不匹配"不是芯片的问题，是 uVision 比对失败。

### 【我】换 ST-Link 也报同样的话，与适配器无关
- ST-Link 序列号 E1007200D0D2139393740544，固件 V2J37S7，连接正常。
- CubeProgrammer 能烧、DAPLink 拖 hex 能烧，硬件全通。
- 两个适配器都报同样的话，问题只能在 uVision 与工程。

### 【我】根因：uvoptx 里存的是别的板子的 ID
- 工程把期望的目标 ID 记在 uvoptx 的调试串里。
- DAP 那条存 1BA01477，ST-Link 那条存 2BA01477，两者不同。
- 说明两条是在不同芯片上记下的，拿去比这块片必然不符。

### 【我】修法（按顺序）
- 关 uVision，删或改名 power_control.uvoptx，再重开工程。
- 在 Debug 里选适配器并让它重新识别，然后 Download。
- 代价：只丢断点、窗口布局与调试参数，uvprojx 不动。
- 或取消 Debug 设置里的手动配置勾选，让 uVision 自动识别。

### 【我】顺带结论：uvoptx 不该进仓库
- 它记的是本机调试参数与芯片 ID，最易引发本类故障。
- 建议 git rm --cached 摘掉它，uvprojx 继续保留。

## 2026-09-19 · 第 20 次（本日第 8 次）：报错出处与 ST-Link 侧真相

### 【我】报错原文出自哪（二进制字符串取证）
- "Connection refused due to device mismatch!" 只在 CMSIS_AGDI.dll 里。
- 同句后半正是"设备与工程目标不同"，与你看到的一致。
- ULINK 与 ULINKpro 各有各的说法，ST-Link 驱动里没有这句。

### 【我】换 ST-Link 的真实报错（Error.log 实测）
- D:\Keil5\ARM\STLink\Error.log 今天 19:28 连记两条。
- 内容是 ST-Link is not in the DFU mode，请重启它。
- 故 ST-Link 那条路是另一个毛病：Keil 要动其固件却失败。
- 克隆 ST-Link 在这一步常过不去，与芯片匹配无关。

### 【我】新发现：MDK 5.43 装了两个 CMSIS-DAP 驱动
- BIN\CMSIS_AGDI.dll 是老驱动，走 HID，报错就出自它。
- BIN\CMSIS_DAP.dll 是新驱动，走 WinUSB，枚举方式完全不同。
- 故 Debug 下拉里应有两个 CMSIS-DAP 项，值得换一个试。

### 【我】uvoptx 已按建议删除
- MDK-ARM 下已无 power_control.uvoptx，uvprojx 被 uVision 重写过。
- 副作用：工程现在没有调试器选择，重开后必须重选一次。

### 【我】下一步
- 重开 uVision，Debug 页逐个试那两个 CMSIS-DAP 项。
- Settings 里确认 SW Device 那行被勾选，并取消手动配置。
- ST-Link 那条路要单独治，先升级它的固件再说。

## 2026-09-19 · 第 22 次（本日第 10 次）：工作模式定为 EIDE 为主

### 【你】工作方式决定
- 只借用 Keil 的器件包与编译器，日常不离开 VS Code 的 EIDE。
- uVision 不再是主线，仅作器件包与编译器的来源。

### 【我】核对：EIDE 侧条件已具备（只读，未改文件）
- eide.yml 的 toolchain 为 AC5，即 Keil 的 ARMCC 编译器。
- packDir 指向工程内 .pack 的 STM32F1xx_DFP 2.2.0，该目录已存在。
- outDir 为 build，器件名 STM32F103C8，与工程一致。
- Keil 侧编译器在 D:\Keil5\ARM\ARMCC\Bin，器件包根为 ARM\PACK。

### 【我】两处隐患（未改，待你定）
- EIDE 的 uploader 现为 OpenOCD，而本机并未安装它。
- 建议改用 STM32CubeProgrammer，其命令行已在且能连 ST-Link。
- .eide 目录现被 gitignore 整个屏蔽，主战场改了之后不妥。
- 建议只屏蔽 env.ini、log 与用户上下文，保留 eide.yml。
- .pack 那 87MB 器件包仍应继续屏蔽，EIDE 会自行准备。

## 2026-09-19 · 第 23 次（本日第 11 次）：.gitignore 按 EIDE 主线调整

### 【你】补充说明与后续计划
- EIDE 自带 OpenOCD，uploader 那条不用管。
- 本项目只是脱离 Keil 的实验。
- 下个项目由 CubeMX 出 GCC 等工程，全盘进 EIDE，不借 Keil。

### 【我】.gitignore 改动
- MDK-ARM 下放行 .eide 的 eide.yml、files.options.yml、env.ini。
- 其余 .eide 内容仍屏蔽，另屏蔽 .eide.usr.ctx.json。
- 新增 *.uvoptx 屏蔽，调试参数不再进仓库。
- 根 .gitignore 顶部说明改为 EIDE 主线、Keil 只供包与编译器。
- uvprojx 与 hex 放行，.pack、.cmsis、RTE、build 等照旧屏蔽。

### 【我】验证
- git ls-files -o --exclude-standard 只列出 .eide 那三个文件。
- 即三者已可提交，其余仍按老规则屏蔽。

## 2026-09-19 · 第 24 次（本日第 12 次）：堆瘦身与错误处理任务核对

### 【你】本次改动（提交 e66a75b）
- 因堆溢出，五个任务栈由 512 收到 256 字节。
- 空闲栈与定时器任务栈也各减半。
- 堆反而由 3072 提到 4096。
- 新增 sensor_err_handle，1 秒一轮的恢复任务。

### 【我】内存账（估算，含 heap_4 每块 8 字节头）
- 栈共 2048，TCB 约 728，队列与互斥约 350。
- 合计约 3.2KB，堆 4KB，余约 0.9KB，够但不宽。
- 实测静态 RAM 只占 8.6KB 共 20KB，提堆比削栈划算。

### 【我】两处纯浪费
- configUSE_TIMERS 为 1，全仓却无任何软件定时器。
- 关掉可省栈 512 加 TCB 与定时器队列约 340，约 850。
- 错误处理任务只为 1 秒一查，可折进 sensorRead 省约 360。

### 【我】真错四条
- 总线离线时三个结果变量未赋值，会写回旧值并刷新时间戳。
- 三个状态局部量未初始化，首次取锁失败即读栈垃圾。
- 两处 lastWakeTime 写在延时后，周期变成 20ms 加执行时间。
- 恢复任务读未加锁实时结构、回写旧快照，会覆盖他人更新。

### 【我】设计与小问题
- I2C1_state 无人置离线，故总线恢复分支是死代码。
- 恢复只要一次成功、判废要五次，抖动器件会来回跳。
- 判离线时错误计数不清零，恢复后易立刻再判废。
- 重试是紧循环无退避，DEBUG 里串口长度写死会截断。

## 待办（第 24 次）
- 先测再调：读 xPortGetMinimumEverFreeHeapSize 与五个任务的栈水位。
- 打开 configCHECK_FOR_STACK_OVERFLOW 为 2，并实现溢出钩子。
- 开启 malloc 失败钩子，或在 MX_FREERTOS_Init 里逐个判空句柄。
- 循环开头把三个 state_res 置 SENSOR_SKIP，堵住旧值写回。
- 三个状态局部量声明即初始化为 DEVICE_OFFLINE。
- 删掉 sensorRead 与 sensor_err_handle 里延时后的 lastWakeTime 重取。
- 恢复任务改读快照判定，只回写真正恢复成功的字段。
- 给总线一个降级判据，否则恢复分支永远不会跑到。
- 恢复要求连续 N 次成功，判离线时把三个错误计数清零。
- 余量不足时优先把堆提到 6144 或 8192，而不是继续削栈。
- 关掉未使用的软件定时器，可回收约 850 字节堆。

## 2026-09-19 · 第 25 次（本日第 13 次）：待提交的 18 个文件是纯换行符差异

### 【我】发现（只读核对，未改文件）
- 工作区 18 个文件与 HEAD 逐行等量增删，合计 1355 对 1355。
- 再加 --ignore-cr-at-eol 与 -w 之后，差异为零。
- 即内容没变，只是工作区 CRLF、仓库 LF。
- autocrlf 在本地、全局、系统三处都未设置，也没有 .gitattributes。

### 【我】影响与修法
- 照原样提交会把 18 个文件的空改动写进历史。
- 建议新增 .gitattributes，内容写 * text=auto eol=crlf。
- 再执行 git add --renormalize . 后，这些文件即回到干净。
- 这也解释了本日开头"git 里修改很多"的现象。

### 【我】待办
- 加 .gitattributes 并 renormalize，避免换行符噪音再进提交。

## 2026-09-20 · 第 26 次（本日第 1 次）：AC6 切换与 u8g2 移植核对

### 【你】本轮改动（提交 8fbecf9 / d61a49c / ad95641）
- 修正传感器恢复状态与任务计时，补上总线降级判据。
- PID 与电压控制流程微调。
- 接入 OLED 屏幕任务与 I2C2，移植 u8g2。
- 编译器由 AC5 换到 AC6，即 armclang V6.24。

### 【我】AC6 切换正确，且有数据支持
- 新构建 ROM 共 41112 字节，超 32KB，正是 AC5 的授权上限。
- 换 AC6 后 0 错 8 警，ROM 40.12KB，ZI 10304 字节。
- RAM 用了 10336 字节共 20480，约五成，还有余量。

### 【我】八个告警是同一个真错
- freertos.c 里写成 char* buffer[20]，那是指针数组。
- sprintf 目标应为 char buffer[20]，编译器点名八处。
- 现在侥幸不炸，只因 80 字节恰好大于 13 字节的串。

### 【我】u8g2 配置与调用不匹配
- 用的是 sh1106 全缓冲档 _f，缓冲占 1024 字节。
- 全缓冲应 ClearBuffer 加 SendBuffer，代码里没有 Clear。
- 还在 SendBuffer 之后调 NextPage，那是页模式接口。
- 后果：残影，字符串变短时会留下旧字符。

### 【我】堆又快见底（估算）
- 新增 screen 任务占 512 栈加约 104 的 TCB 与块头。
- 估合计约 3.9KB 共 4096，余量只剩约 200 字节。
- 建议直接把堆提到 6144 或 8192，RAM 尚有六成可让。

### 【我】u8g2 源码在仓库之外
- 源码在 D:\Keil5\3rdParty\，比工程根还高两级。
- eide.yml 里写作 ../../../../3rdParty/...。
- 后果：克隆仓库后无法编译 OLED 部分。
- 建议把用到的 sh1106csrc 与包装搬进仓库。

### 【我】OLED 成本与遗留
- 全缓冲 1KB 走 100kHz，每次约 90 到 100 毫秒阻塞。
- 建议 I2C2 提到 400kHz 或改页模式，压住刷新耗时。
- u8g2Init 里两次 HAL_Delay(1000) 让开机多等两秒。
- 上轮第 3、5 条已修，第 4 条基本修好。
- 上轮第 1、2 条仍未修：总线离线时结果变量未赋值。

## 待办（第 26 次）
- 立刻把 configTOTAL_HEAP_SIZE 提到 6144 或 8192。
- 关掉未使用的软件定时器，可省约 850 字节堆。
- 把 char* buffer[20] 改正，并改用 snprintf 带上限。
- 全缓冲补 u8g2_ClearBuffer，删掉多余的 NextPage。
- 把 u8g2 用到的那份源码搬进仓库，并改两处路径。
- 实测堆峰值与八个任务的栈水位，再定各栈大小。
- 开栈溢出与 malloc 失败钩子，创建句柄逐个判空。
- 循环开头把三个 state_res 置 SENSOR_SKIP，堵旧值回写。
- 三个器件状态局部量声明即初始化为 DEVICE_OFFLINE。
- I2C2 提到 400kHz 或改页模式，压 OLED 刷新耗时。
- 去掉 u8g2Init 里两次 1 秒延时。
- 恢复回写只写真正恢复成功的字段，别整份回写。
- 加 .gitattributes 并 renormalize，清掉换行符噪音。

## 2026-09-21 · 第 27 次（本日第 1 次）：待提交改动核对与英文 commit 文案

### 【你】本轮待提交改动
- 把 u8g2 源码搬进仓库 3rdParty/u8g2，两个工程都改指它。
- 新增 power_config.h，集中电压限值、步进与开机设定。
- 按键直接改设定值并限幅，去掉两个 CMSIS 队列。
- 堆由 4096 提到 8192，HAL 时基由 TIM4 改回 SysTick。
- 屏幕任务改 char buffer 并补 ClearBuffer，删掉 NextPage。

### 【我】核对结论
- 最新构建 0 错 0 警，ROM 40.0KB，RAM 14424 共 20480，七成。
- 两个队列已无任何引用，删除干净。
- sh1106 设备符号在 u8x8_d_ssd1306_128x64_noname.c 内，能链上。
- 那八个指针类型告警已随 buffer 改正而消失。

### 【我】新发现
- u8g2_fonts.c 单文件 37.93MB，占 3rdParty 全部 41.3MB 的九成。
- 实际只用了 u8g2_font_6x10_tr 一个字体，其余全是白背。
- 屏幕改为每 200ms 无条件全屏重刷，1KB 走 100kHz 约 90ms。
- PowerState_copy_last 与 voltage_change 已成只写不读的死变量。

### 【我】产出
- 已给出英文 summary 与 description，供本次提交直接使用。

## 待办（第 27 次）
- 把 u8g2_fonts.c 精简到只留用到的字体，或改用单字体文件。
- I2C2 提到 400kHz，或恢复只在数值变化时刷屏。
- 删掉 PowerState_copy_last 与 voltage_change 两个死变量。
- 上轮遗留：循环开头置 SENSOR_SKIP，状态局部量初始化。
- 上轮遗留：八个任务的栈水位与堆峰值实测。
- 上轮遗留：.gitattributes 与 renormalize，清行尾噪音。

## 2026-09-21 · 第 28 次（本日第 2 次）：README 与实测指标留档

### 【你】提交与实测数据
- 提交 a297d40 采用了上一轮给的英文 summary 与 description。
- 实测：负载与设定值突变 500ms 内恢复稳定。
- 实测：6V/10A 满载跌落 0.02V，负载调整率 0.3%。
- 实测：120W 连续输出，最高温度 65 度。
- 短路由 LM5175 限流与打嗝保护自动闭锁，解除后自恢复。

### 【我】更正与核对
- 更正：第 27 次写的"时基改 SysTick"未落地，现仍为 TIM4。
- SysTick 留给 FreeRTOS、TIM4 供 HAL，属推荐做法，无需再改。
- 最新构建 armclang V6.24，0 错 0 警，ROM 39.4KB，RAM 14.4KB。
- 工作区 30 条改动经核对仍是纯行尾差异，内容无变化。

### 【我】本轮产出
- 仓库根新增 README.md：指标、硬件、任务表、分层、构建与已知限制。

## 待办（第 28 次）
- u8g2_fonts.c 38MB 只有一个字体在用，待精简。
- I2C2 提到 400kHz，或恢复按变化刷屏。
- 循环开头置三个 state_res 为 SENSOR_SKIP，器件状态量初始化。
- 开栈溢出与 malloc 失败钩子，任务句柄逐个判空。
- 实测各任务栈水位与堆峰值。
- 清掉 PowerState_copy_last 与 voltage_change 两个死变量。
- 加 .gitattributes 与 renormalize，清掉 30 条行尾噪音。
- 未用软件定时器，configUSE_TIMERS 可关，省约 850 字节堆。

## 2026-09-22 · 第 29 次（本日第 1 次）：字体瘦身与 I2C 加速核对

### 【你】本轮改动
- u8g2_fonts.c 由 37.93MB 精简到 0.03MB，只留四个字体。
- 3rdParty 合计由 41.3MB 降到 2.42MB。
- I2C2 由 100kHz 提到 400kHz，模式改为 Fast。
- TIM1 预分频由 17 改 71，延时基准变成 1 微秒。

### 【我】核对结论
- u8g2_font_6x10_tr 定义在精简后文件第 13 行，能链上。
- 最新构建 09-22 10:13，0 错 0 警，ROM 39.4KB，RAM 14.4KB。
- 屏幕单帧刷新由约 90ms 降到约 23ms。
- I2C2 与 TIM1 这两处改动尚未提交，其余已入库。

### 【我】仍未修项
- state_res 三个变量与三个器件状态量仍未初始化。
- 总线离线分支仍未把结果变量置 SENSOR_SKIP。
- 无栈溢出与 malloc 失败检测，任务句柄未判空。
- 栈水位与堆峰值未实测。
- voltage_change 与 PowerState_copy_last 仍是死变量。
- configUSE_TIMERS 仍为 1，无软件定时器在用。
- 无 .gitattributes，工作区仍有行尾噪音。
- u8x8_fonts.c 仍有 1.53MB，未用可再精简。

### 【我】产出
- README 已按本轮结果更新：指标表加刷新耗时，硬件补 I2C 速率，待办重写。

## 待办（第 29 次）
- 提交 I2C2 400kHz 与 TIM1 预分频这两处改动。
- 循环开头置三个 state_res 为 SENSOR_SKIP，状态量初始化。
- 开栈溢出与 malloc 失败钩子，任务句柄逐个判空。
- 实测各任务栈水位与堆峰值。
- 清掉 PowerState_copy_last 与 voltage_change 两个死变量。
- 加 .gitattributes 与 renormalize，清掉行尾噪音。
- 未用软件定时器，configUSE_TIMERS 可关，省约 850 字节堆。
- u8x8_fonts.c 还有 1.53MB 未用，可继续精简。
- I2C1 传感器总线也可考虑提到 400kHz（可选）。

## 2026-09-22 · 第 30 次（本日第 2 次）：代码量与项目规模统计

### 【我】自研代码（去注释后的代码行）
- 应用层 Core/app 共 7 文件 148 行。
- 传感器层 Core/sensors 共 8 文件 369 行。
- CubeMX 文件内 USER CODE 共 458 行，其中 freertos.c 占 395。
- 自研合计约 975 行，注释另计约两成。

### 【我】生成与第三方
- CubeMX 生成 Core/Src 与 Inc 共 1510 行，其中约 458 行是自研片段。
- ST HAL 驱动包 788 文件 34.98 万行，多数未参与编译。
- FreeRTOS 39 文件 1.40 万行，实际链接约十个文件。
- u8g2 48 文件 3.44 万行，其中 u8x8_fonts.c 占大半且未使用。
- 日志与 README 三个文件约 610 行。

### 【我】项目规模估计
- 固件最终 39.4KB ROM 与 14.4KB RAM，属中等规模应用。
- 依赖体量大但自研聚焦：RTOS 架构、三层驱动、PID、显示、容错。
- 硬件含原理图、PCB、焊接与调试，与固件合计约 1.5 到 2.5 人月。
- 时间跨度按 git 与简历记 2026.8 起，约六周单人投入。

### 【我】产出
- README 增加代码规模小节，日志补本轮统计。

## 待办（第 30 次）
- 提交 I2C2 400kHz 与 TIM1 预分频这两处改动。
- 循环开头置三个 state_res 为 SENSOR_SKIP，状态量初始化。
- 开栈溢出与 malloc 失败钩子，任务句柄逐个判空。
- 实测各任务栈水位与堆峰值。
- 清掉 PowerState_copy_last 与 voltage_change 两个死变量。
- 加 .gitattributes 与 renormalize，清掉行尾噪音。
- 未用软件定时器，configUSE_TIMERS 可关，省约 850 字节堆。
- u8x8_fonts.c 与未编译的 HAL 文件可再精简仓库体积。

## 2026-09-22 · 第 31 次（本日第 3 次）：UART 上报与栈水位机制

### 【你】本轮新增
- 新增 uart_dev 与 uart_debug 两文件，即 UART 设备层与打印层。
- 打印层用 vsnprintf 限长 128 字节，再交设备层发出。
- 设备层带弱锁钩子与错误码，取不到锁按忙返回。
- 四个任务周期打印各自栈水位，即水位查看机制。
- 六个任务栈随之调大，最大 192 字，合计 3840 字节。
- 新增第三个互斥量 dev_uart1，供串口发送加锁。
- 屏幕刷新增加 Vout、Iout 与温度三行。

### 【我】真错一处（必修）
- uart_debug 传 DEV_UART1，设备层却只映射 DEV_UART2。
- 于是 uart 指针未初始化就交给 HAL，一调用即可能异常。
- 修法二选一：调用处改传 DEV_UART2，或设备层补上映射。
- 名字也要统一：本板实际是 USART2，枚举却叫 DEV_UART1。

### 【我】其它核对
- 水位只打印不比较，确实没有硬控制，低水位不触发动作。
- 堆峰值未纳入上报，未用 xPortGetMinimumEverFreeHeapSize。
- buttom 与 defaultTask 两个任务尚未打印水位。
- DEV_UART_DEBUG 未在两处构建里定义，开机那句被编译掉。
- 最新构建 18:07，0 错 0 警，ROM 43.1KB，RAM 14.4KB。

### 【我】产出
- 日志记录本轮机制与那处真错，待办已更新。

## 待办（第 31 次）
- 修 DEV_UART1 与 DEV_UART2 不一致，否则一调用就异常。
- 水位加阈值判断与动作，或至少低水位时打印告警。
- 堆峰值一并上报，用 xPortGetMinimumEverFreeHeapSize。
- buttom 与 defaultTask 补上水位打印。
- 若要开机打印，在 EIDE 与 uvprojx 里定义 DEV_UART_DEBUG。
- 串口锁超时宏沿用了电源锁的名字，建议另立一个。
- 提交 I2C2 400kHz 与 TIM1 预分频那两处旧改动。
- 循环开头置三个 state_res 为 SENSOR_SKIP，状态量初始化。
- 开栈溢出与 malloc 失败钩子，任务句柄逐个判空。
- 加 .gitattributes 与 renormalize，清掉行尾噪音。
