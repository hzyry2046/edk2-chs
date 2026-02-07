# 中文显示支持测试程序

这是一个用于测试EFI固件是否支持中文字符显示的应用程序。

## 功能

- 检测EFI固件是否支持HII字体协议
- 显示包含中英文混合的测试字符串
- 验证中文字符是否能正确渲染
- 提供详细的测试结果报告

## 使用方法

1. 编译程序：
   ```bash
   # 设置EDK II环境
   . edksetup.sh
   
   # 编译应用程序
   build -p ChsDisplayTestApp/ChsDisplayTestApp.dsc -m ChsDisplayTestApp/ChsDisplayTestApp.inf -a X64
   ```

2. 将生成的EFI应用程序复制到EFI系统分区：
   ```bash
   cp Build/ChsDisplayTestApp/DEBUG_GCC5/X64/ChsDisplayTestApp.efi /path/to/esp/efi/boot/
   ```

3. 在UEFI Shell中运行：
   ```
   FS0:\> ChsDisplayTestApp.efi
   ```

## 测试输出解释

程序会显示如下信息：

- `[PASS] HII Font Protocol found.` - 表示找到了HII字体协议
- `[PASS] HII Database Protocol found.` - 表示找到了HII数据库协议
- 测试字符串 "Hello 你好 World 世界" - 如果能看到中文字符，则固件支持中文显示
- 最终结果会明确告知固件是否支持中文字符显示

## 结果判断

- 如果看到中文字符正常显示（而不是方框、问号或其他符号），则固件支持中文显示
- 如果看到方框或乱码，则固件不支持中文显示
- 程序最后会给出明确的结论

## 注意事项

- 此程序仅用于测试目的
- 需要支持HII字体协议的固件才能运行
- 在某些固件上可能无法显示中文字符，这取决于固件内置的字体支持情况