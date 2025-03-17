# :star:MESAFileType - 文件类型识别组件

一个功能强大的文件类型识别组件，支持多种识别方式和多线程处理



## :star:特点

- **多种识别方式**：
  - 基于 libmagic 的文件内容识别
  - 通过 URL 后缀名识别
  - 通过 HTTP 响应的 Content-Type 字段识别
  - 综合三种方式的识别结果
- **多线程支持**：提高大量文件或 URL 的识别效率
- **灵活的输入方式**：
  - 单个文件或 URL
  - 多个文件或 URL（通过文件存储）
  - 目录识别
- **便捷扩展**
  - 通过简单修改即可实现增加新的文件识别类型
- **进度条显示**：实时显示识别进度

## :star:目录结构



```
MESAFileType/
├── src/                      # 源代码文件
│   ├── MESA_file_type.h      # 头文件
│   ├── MESA_file_type.c      # libmagic实现文件
│   ├── frontend_identification.c # 前端识别实现文件
│ 	├── backend_identification.c # 后端识别实现文件
│   └── identification_core.c # 识别核心逻辑
├── test/                     # 测试文件
│   ├── single_file_test.c    # 单个文件测试
│   ├── single_url_test.c     # 单个URL测试
│   ├── directory_test.c      # 目录测试文件
│   ├── main_test.c           # 主测试文件
│   └── test.h                # 测试头文件

├── utils/                    # 工具文件
│   ├── progress_bar.c        # 进度条实现
│   └── utils.h               # 工具函数头文件
├── Makefile                  # 编译脚本
├── README.md                 # 项目说明
├── LICENSE                   # 许可证文件
└── docs/                     # 文档文件
    ├── design.md             # 设计文档
    └── usage.md              # 使用文档
```

## :gear:安装

### 依赖项

- libmagic 开发库
- pthread 库
- libcurl 库

在 Ubuntu/Debian 系统上安装依赖项：

bash

```bash
sudo apt-get install libmagic-dev libcurl4-openssl-dev
```

### 编译

使用 Makefile 编译项目：

bash

```bash
make
```

这将生成一个名为 `test` 的可执行测试文件。

## :star:使用方法

### 基本用法

bash

```bash
./test <file|url|dir|urls_file>
```

- **单个文件或 URL**：

  bash

  ```bash
  ./test example.txt
  ./test http://example.com/file.png
  ```

- **目录识别**：

  bash

  ```bash
  ./test /path/to/directory
  ```

- **URL 列表文件**：

  bash

  ```bash
  ./test urls.txt
  ```

### 示例输出

#### 单个文件或 URL



```
文件: example.txt
识别结果: document:txt

URL: http://example.com/file.png
识别结果: image:png
```

#### 多个文件或 URL



```
[====================] 100.00%
所识别文件路径或者URL   识别结果
example1.txt           document:txt
example2.jpg           image:png
example3.mp3           video:mp3
```

## :star:贡献

欢迎贡献！请提交问题或拉取请求来报告问题或添加新功能。



