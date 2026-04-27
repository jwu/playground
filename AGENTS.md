# AGENTS.md

## Project

这是一个用于收集、构建和测试 raylib C examples 的小型 playground。

- 构建系统：Zig `0.16.0`
- 包管理：`build.zig.zon`，使用 Zig dependency 管理第三方依赖
- 示例语言：C，由 Zig build 编译为多个可执行程序
- raylib：GitHub `raysan5/raylib` tag `6.0`

核心约束：不要把项目改成 CMake；不要手动 vendor 第三方源码或预编译二进制；raylib 应始终由 Zig dependency 管理。

## Key structure

- `build.zig`：扫描 `src/bin/*.c`，每个 C 文件生成一个同名 executable，并链接 Zig 管理的 raylib。
- `build.zig.zon`：项目元信息与依赖声明。
- `src/bin/`：平铺存放 C examples，例如 `core_basic_window.c`。
- `assets/`：平铺存放 example 实际需要的资源；C 代码中资源路径统一写 `assets/<filename>`。
- `target/bin/`：构建输出目录。
- `zig-pkg/`、`.zig-cache/`：Zig 缓存目录，不要编辑。

## Working rules

- 保持修改小而聚焦，优先复用现有模式。
- 新增 example 时使用 `src/bin/<example_name>.c`，文件名尽量保持与 upstream raylib example 一致。
- 只加入 example 实际需要的少量资源到 `assets/`，不要复制整个资源目录。
- 项目文件使用 2 空格缩进。
- 新增或更新依赖时优先使用 Zig 包管理命令，不要手动改 dependency hash，除非已经验证过。

## Commands

常用验证命令：

```bash
zig build examples
zig build run
zig build <example_name>
```

需要指定 raylib platform backend 时：

```bash
zig build examples -Dplatform=glfw
```

修改 `build.zig`、`build.zig.zon` 或新增 example 后，至少运行：

```bash
zig build examples
```

## Add an example

1. 将 C 文件放到 `src/bin/`。
2. 将必要资源放到 `assets/`。
3. 将源码中的 `resources/foo.ext` 改为 `assets/foo.ext`。
4. 运行 `zig build examples` 验证。

## Safety

可以直接执行：

- 读取和搜索文件
- 编辑 `build.zig`、`build.zig.zon`、`src/bin/*.c`、`.gitignore` 和本文件
- 在 `src/bin/` 下新增 example
- 在 `assets/` 下添加少量必要资源
- 运行 `zig build examples` 或单个 example step

需要先确认：

- 删除大量文件
- 安装系统依赖
- 添加手动 vendor 的第三方源码树或二进制 release
- 改变项目布局约定
- 用大规模资源目录替换少量必要资源
- 运行耗时长或与任务无关的命令
- 把项目转换到其他构建系统

## Preferred patterns

- 一个 C example 文件对应一个同名可执行程序。
- 运行时工作目录是项目根目录，因此资源路径应相对项目根目录。
- 让 raylib 自己的 Zig build 处理平台和系统库链接；本项目不要手动链接 raylib 静态产物。

## When stuck

如果 example 因缺少资源失败，先查看对应 upstream raylib example 的源码和资源，只添加实际需要的文件并调整路径。若问题看起来需要改变依赖管理方式或项目布局，先询问用户。不要做大规模猜测性修改。
