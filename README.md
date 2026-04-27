# raylib playground

一个用于收集、构建和测试 raylib C examples 的小型 playground。

- 构建系统：Zig `0.16.0`
- 示例语言：C
- 图形库：raylib `6.0`
- 依赖管理：`build.zig.zon`

项目会自动扫描 `src/bin/*.c`，为每个 C 文件生成一个同名可执行程序，并通过 Zig dependency 链接 raylib。

## Requirements

- Zig `0.16.0`

## Quick start

构建所有 examples：

```bash
zig build examples
```

运行默认 example：

```bash
zig build run
```

运行单个 example：

```bash
zig build core_basic_window
zig build shapes_basic_shapes
zig build shapes_math_sine_cosine
zig build textures_logo_raylib
```

指定 raylib platform backend：

```bash
zig build examples -Dplatform=glfw
```

## Current examples

- `core_basic_window`
- `shapes_basic_shapes`
- `shapes_math_sine_cosine`
- `textures_logo_raylib`

## Project layout

```text
.
├── assets/            # 示例运行时需要的资源
├── src/bin/           # raylib C examples，每个文件对应一个可执行程序
├── target/bin/        # 构建输出
├── build.zig          # 构建逻辑
└── build.zig.zon      # 项目元信息与依赖声明
```

## Add a new example

1. 新建 `src/bin/<example_name>.c`
2. 如果需要资源，把少量必要文件放到 `assets/`
3. 将源码中的资源路径写成 `assets/<filename>`
4. 验证构建：

```bash
zig build examples
```

## Notes

- 不要把项目改成 CMake。
- 不要手动 vendor 第三方源码或预编译二进制。
- raylib 应始终由 Zig dependency 管理。
- 运行时工作目录是项目根目录，所以资源路径应相对项目根目录。
