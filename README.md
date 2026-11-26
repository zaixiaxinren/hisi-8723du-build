# 8723du 驱动编译包（Hi3798MV200H / Android 7.0 / 3.18.24）

## 这个包是干什么的

给海思 Hi3798MV200H 盒子的 **Android 7.0 系统**编译 RTL8723DU WiFi 驱动。

目标内核：`3.18.24_hi3798mv2x`

编译成功后要得到的关键特征：

```
vermagic=3.18.24_hi3798mv2x SMP mod_unload ARMv7 p2v8
```

注意里面**没有 `preempt`**，也**没有 `modversions`**。这两个细节错了，模块就加载不上。

---

## 目录结构

```
.
├── .github/workflows/build.yml   ← 编译脚本（必须放到这个路径）
├── 8723du/                       ← Realtek 驱动源码 v5.2.15.2
└── README.md
```

---

## 操作步骤

### 1. 建一个 GitHub 仓库

打开 https://github.com/new

- Repository name 随便填，比如 `8723du-hisilicon`
- 选 **Public**（Private 也可以，但 Actions 有免费额度限制）
- **不要**勾选 "Add a README file"
- 点 Create repository

### 2. 上传驱动源码

在新建的空仓库页面：

1. 点 **"uploading an existing file"** 链接
2. 把本文件夹里的 `8723du` **整个文件夹**拖进去
3. 底部点 **Commit changes**

### 3. 创建编译脚本（关键一步）

`.github` 这种以点开头的目录，用网页拖拽上传经常会被忽略，所以单独手动建：

1. 回到仓库首页，点 **Add file** → **Create new file**
2. 在文件名框里输入（要包含斜杠，GitHub 会自动建目录）：

```
.github/workflows/build.yml
```

3. 把本包里的 `.github/workflows/build.yml` 文件内容**全部复制粘贴**进去
4. 点 **Commit changes**

### 4. 等编译

提交后自动开始：

1. 点仓库顶部的 **Actions** 标签
2. 会看到一个叫 **Build 8723du for Hi3798MV200H (3.18.24)** 的任务在跑
3. 点进去可以看实时日志
4. 大约 **6~10 分钟**跑完

### 5. 下载结果

编译完成后：

1. 点进那次运行
2. 页面底部 **Artifacts** 区域
3. 下载 **8723du-build-result.zip**
4. 解压，里面应该有 `8723du.ko`

---

## 成功长什么样

Artifacts 里的日志会有这一段：

```
=== ko files ===
./8723du.ko

=== vermagic ===
./8723du.ko:
vermagic=3.18.24_hi3798mv2x SMP mod_unload ARMv7 p2v8
```

**vermagic 这一行必须一字不差**。如果显示的是：

- `... SMP preempt mod_unload ...` → 说明 PREEMPT 没关掉
- `... modversions ...` → 说明 MODVERSIONS 没关掉
- `... 3.18.24 SMP ...` → 说明 LOCALVERSION 没生效

这三种情况告诉我，我改配置。

---

## 如果失败了

**正常，第一次大概率要调。**

Realtek 驱动是 2018 年的代码，Linux 3.18 是 2014 年的内核，两边 API 有差异，通常要打几个补丁。

失败时请把 **Actions 运行日志最后 100 行**贴给我，特别是这些行：

```
error: 
warning: implicit declaration of
No such file or directory
```

我看到具体报错会改代码，然后你重新上传或我再给一份新包。

---

## 技术备注（给你看的，不用操作）

### 为什么只要编一个 .ko

7.0 的 system 分区里自带：

- `cfg80211.ko`（371504 字节，vermagic 和内核匹配）
- `wpa_supplicant`（253 处引用）
- `hostapd`（240 处）
- `libnl`（85 处）

所以 WiFi 框架和用户态都是全的，**只缺 8723du 这一个驱动**（原包只有 8723bu，B 系列和 D 系列芯片不同）。

### 为什么不需要内核源码的 .config

内核没开 `CONFIG_MODVERSIONS`，模块加载时不校验符号 CRC，只看 vermagic。所以只要编出的 vermagic 字符串对，就能加载。

### 为什么用标准 Linux 3.18.24 而不是海思源码

海思不开源内核。但编译**外部模块**只需要：

- 内核头文件（`include/`、`arch/arm/include/`）
- `scripts/` 里的构建工具
- 正确的 `.config`（影响 vermagic 和条件编译）

这些都来自标准 3.18.24。驱动只调用标准 net/usb/cfg80211 API，不碰海思私有接口。

### 编出来之后怎么用

1. `8723du.ko` 推到盒子 `/system/lib/modules/`
2. 先 `insmod cfg80211.ko`
3. 再 `insmod 8723du.ko`
4. 固件 `rtl8723d_fw.bin` 需要放到 `/system/etc/firmware/` 或 `/vendor/firmware/`

（驱动源码里 `CONFIG_EMBEDDED_FWIMG` 是开的，固件可能已经内嵌在 .ko 里，到时候看情况）
