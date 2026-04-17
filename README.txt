# Smart Desk Clock

一款智能桌面时钟，融合 AI 美学与个性化交互，让时间显示成为桌面艺术。

## 特性

- **AI 风格设计师** — 描述你想要的风格，AI 自动生成独一无二的时钟主题
- **动态粒子特效** — 桃花飘落、数字雨、霓虹光点等环境特效
- **情绪感知语录** — 根据时段和天气，AI 推送双语哲理短句
- **多层窗口管理** — 置顶/普通/桌面底层，快捷键快速切换
- **智能天气显示** — 实时天气与诗意化表达结合
- **完全自定义** — 颜色、字体、透明度、特效随心调整

## 快捷键

| 按键 | 功能 |
|------|------|
| `L` | 锁定/解锁窗口位置 |
| `T` | 循环切换窗口层级 |
| `H` | 显示/隐藏窗口 |
| `S` | 打开样式选择器 |
| `R` | 刷新语录 |
| `Ctrl + +/-` | 缩放窗口大小 |
| `Q` | 退出应用 |

## 托盘菜单

右键点击托盘图标可访问：
- 锁定状态切换
- 窗口层级设置
- AI 风格生成（随机/描述生成）
- 样式切换
- 语录/天气刷新

## AI 配置

支持多个 AI 提供商：
- **智谱 GLM**（默认，免费额度）
- **Moonshot Kimi**
- **SiliconFlow**
- **本地 Ollama**

在托盘菜单中切换提供商并配置 API Key。

## 构建

### 依赖
- Qt 6.2+
- CMake 3.16+

### Linux
```bash
mkdir build && cd build
cmake ..
cmake --build .
./SmartDeskClock
```

### Windows
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
windeployqt SmartDeskClock.exe
```

## 配置存储

配置文件位于：
- Linux: `~/.config/SmartDeskClock/config.json`
- Windows: `%APPDATA%/SmartDeskClock/config.json`

## 许可证

MIT License