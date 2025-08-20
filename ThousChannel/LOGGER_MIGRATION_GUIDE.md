# 日志系统迁移指南

## 概述
将旧的 `CLogger` 系统迁移到现代的 `ModernLogger` 系统。

## 迁移步骤

### 1. 更新头文件包含
将所有文件中的：
```cpp
#include "Logger.h"
```
替换为：
```cpp
#include "ModernLogger.h"
```

### 2. 宏定义映射

#### 旧宏 → 新宏
| 旧宏 | 新宏 | 说明 |
|------|------|------|
| `LOG_DEBUG(msg)` | `LOG_DEBUG("{}", msg)` | 简单字符串 |
| `LOG_INFO(msg)` | `LOG_INFO("{}", msg)` | 简单字符串 |
| `LOG_WARNING(msg)` | `LOG_WARN("{}", msg)` | 简单字符串 |
| `LOG_ERROR(msg)` | `LOG_ERROR("{}", msg)` | 简单字符串 |
| `LOG_FATAL(msg)` | `LOG_FATAL("{}", msg)` | 简单字符串 |

#### 格式化宏映射
| 旧宏 | 新宏 | 说明 |
|------|------|------|
| `LOG_DEBUG_FMT(fmt, ...)` | `LOG_DEBUG(fmt, ...)` | 移除 _FMT 后缀 |
| `LOG_INFO(fmt, ...)` | `LOG_INFO(fmt, ...)` | 移除 _FMT 后缀 |
| `LOG_WARNING_FMT(fmt, ...)` | `LOG_WARN(fmt, ...)` | 移除 _FMT 后缀 |
| `LOG_ERROR(fmt, ...)` | `LOG_ERROR(fmt, ...)` | 移除 _FMT 后缀 |
| `LOG_FATAL_FMT(fmt, ...)` | `LOG_FATAL(fmt, ...)` | 移除 _FMT 后缀 |

#### 字符串宏映射
| 旧宏 | 新宏 | 说明 |
|------|------|------|
| `LOG_INFO(msg)` | `LOG_INFO("{}", msg)` | 手动替换为格式化调用 |
| `LOG_WARNING(msg)` | `LOG_WARN("{}", msg)` | 手动替换为格式化调用 |
| `LOG_ERROR(msg)` | `LOG_ERROR("{}", msg)` | 手动替换为格式化调用 |

### 3. 格式化字符串更新

#### C风格 → C++20风格
| 旧格式 | 新格式 | 说明 |
|--------|--------|------|
| `%s` | `{}` | 字符串 |
| `%d` | `{}` | 整数 |
| `%f` | `{:.2f}` | 浮点数（保留2位小数） |
| `%u` | `{}` | 无符号整数 |

### 4. 代码示例

#### 旧代码：
```cpp
LOG_INFO("User %s (ID: %d) logged in", userName.c_str(), userId);
LOG_ERROR("Connection failed: %s", errorMsg.c_str());
LOG_INFO("Operation completed");
```

#### 新代码：
```cpp
LOG_INFO("User {} (ID: {}) logged in", userName, userId);
LOG_ERROR("Connection failed: {}", errorMsg);
LOG_INFO("Operation completed");
```

**注意**：`LOG_INFO_STR` 等宏需要手动替换为对应的格式化调用。

### 5. 移除不必要的 .c_str() 调用
由于新系统使用 `std::format`，不再需要 `.c_str()` 调用：

```cpp
// 旧代码
LOG_INFO("String: %s", myString.c_str());

// 新代码
LOG_INFO("String: {}", myString);
```

### 6. 初始化新日志系统
在应用程序启动时添加：

```cpp
// 配置日志系统
auto& logger = ModernLogger::instance();
logger.setLogLevel(LogLevel::Debug);  // 或 Info, Warn, Error
logger.setLogFile("logs/application.log");
```

### 7. 删除旧文件
迁移完成后，可以删除：
- `Logger.h`
- `Logger.cpp`

### 8. 更新构建系统
确保 `ModernLogger.cpp` 被包含在构建中。

## 优势

### 性能提升
- **零编码转换开销**：不再需要 `_T()` 到 UTF-8 的转换
- **编译时优化**：Release模式下日志调用可能被完全优化掉
- **更少的锁竞争**：使用 `std::lock_guard` 替代 `CRITICAL_SECTION`

### 类型安全
- **编译时类型检查**：`std::format` 提供类型安全
- **自动源位置**：`std::source_location` 自动捕获调用位置
- **更好的错误信息**：编译时错误而不是运行时错误

### 现代C++特性
- **C++20 std::format**：类型安全的格式化
- **模板元编程**：编译时优化
- **RAII**：自动资源管理

## 注意事项

1. **C++20要求**：新系统需要C++20编译器支持
2. **向后兼容**：兼容性宏确保现有代码继续工作
3. **性能测试**：建议在迁移后进行性能测试
4. **日志格式**：新系统的日志格式略有不同，包含源位置信息

## 迁移检查清单

- [ ] 更新所有 `#include "Logger.h"` 为 `#include "ModernLogger.h"`
- [ ] 替换所有 `LOG_*_FMT` 宏为 `LOG_*`
- [ ] 更新格式化字符串从 `%s` 到 `{}`
- [ ] 移除不必要的 `.c_str()` 调用
- [ ] 添加日志系统初始化代码
- [ ] 更新构建系统
- [ ] 测试所有日志功能
- [ ] 删除旧日志文件
