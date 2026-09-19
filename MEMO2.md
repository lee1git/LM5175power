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
