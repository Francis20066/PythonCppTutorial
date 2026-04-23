# Python 调用 C++ 实现 Fibonacci

这个示例演示如何用 C++ 编写 Fibonacci 函数，并通过 Python 原生扩展模块的方式在 Python 中调用。

构建完成后，Python 可以这样使用：

```python
import fibonacci_cpp

print(fibonacci_cpp.fibonacci(10))  # 55
```

## 一、环境配置

需要安装：

- Python 3.x
- CMake 3.18 或更高版本
- 一个可用的 C++17 编译器，例如 MinGW、MSVC 或 Clang

确认命令可用：

```powershell
python --version
cmake --version
g++ --version
```

注意：Python 扩展模块必须使用同一个 Python 版本来构建和运行。本项目的 CMake 会优先选择当前命令行 PATH 中的 `python`，这样 `cmake --build build` 生成的模块可以直接被 `python main.py` 导入。

如果你想指定某个 Python 解释器，可以这样配置：

```powershell
cmake -S . -B build -DPython_EXECUTABLE="C:\Users\你的用户名\miniconda3\python.exe"
```

## 二、构建 C++ 扩展模块

在项目根目录运行：

```powershell
cmake -S . -B build
cmake --build build
```

构建成功后，`build` 目录里会生成类似下面的文件：

```text
fibonacci_cpp.cp313-win_amd64.pyd
```

文件名中的 Python 版本号可能不同，这是正常的。例如 `cp313` 表示 CPython 3.13。

`CMakeLists.txt` 的核心配置是：

```cmake
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

add_library(fibonacci_cpp MODULE
    src/fibonacci.cpp
)

target_link_libraries(fibonacci_cpp PRIVATE Python::Module)
```

这里生成的模块名是 `fibonacci_cpp`，所以 Python 中使用 `import fibonacci_cpp`。

## 三、Python 调用方式

`src/fibonacci.cpp` 使用 Python C API 暴露了一个函数：

```python
fibonacci_cpp.fibonacci(n)
```

参数说明：

- `n` 必须是非负整数。
- 返回值是 Python 的 `int`，所以结果可以是大整数。
- C++ 侧当前把 `n` 解析为 `unsigned long long`，因此索引 `n` 最大支持 `18446744073709551615`，但实际可计算大小还取决于内存和时间。

运行示例：

```powershell
python main.py
```

你也可以传入更大的 n：

```powershell
python main.py 1000000
```

## 四、Wall Time 性能测试

`main.py` 会做两件事：

- 先打印 `fibonacci_cpp.fibonacci(0)` 到 `fibonacci_cpp.fibonacci(10)` 的小规模结果。
- 再对比纯 Python 版本和 C++ 扩展版本计算同一个大数时消耗的 Wall Time。

默认测试 `n = 100000`：

```powershell
python main.py
```

在当前机器上的一次结果如下：

```text
Benchmark n = 100,000
Result decimal digits : 20,899
Python Wall Time     : 0.052376 s
C++ Wall Time        : 0.000816 s
Speedup              : 64.18x
```

测试 `n = 1000000`：

```powershell
python main.py 1000000
```

在当前机器上的一次结果如下：

```text
Benchmark n = 1,000,000
Result decimal digits : 208,988
Python Wall Time     : 4.540529 s
C++ Wall Time        : 0.033997 s
Speedup              : 133.56x
```

不同机器、Python 版本、编译器和后台负载都会影响 Wall Time，所以这些数字只作为参考。更严谨的性能评估可以多运行几次取平均值，或者使用 `pyperf` 这类专业工具。

## 五、为什么 C++ 实现更有优势

C++ 扩展适合放置计算密集型热点代码。在这个例子里，收益主要来自：

- Python 版本使用直观循环实现，每一步都要经过 Python 字节码解释和变量绑定。
- C++ 扩展把热点逻辑下沉到编译后的模块中，减少 Python 层循环开销。
- C++ 版本使用快速倍增法，能用大约 `O(log n)` 次大整数运算计算 Fibonacci，比普通循环的 `O(n)` 次迭代更适合大 n。
- Python 仍然负责调用入口和结果展示，C++ 负责重计算部分，可以兼顾开发效率和性能。

也要注意：不是所有代码改成 C++ 都会更快。跨语言调用本身有成本，适合下沉到 C++ 的通常是计算密集、调用次数可控、逻辑相对稳定的部分。

## 六、Python C API 学习链接

- Python 官方教程：Extending and Embedding the Python Interpreter  
  https://docs.python.org/3/extending/

- Python 官方参考：Python/C API Reference Manual  
  https://docs.python.org/3/c-api/index.html

- Real Python 教程：Building a Python C Extension Module  
  https://realpython.com/build-python-c-extension-module/

- 如果后续想用更现代的 C++ 封装方式，也可以了解 pybind11  
  https://pybind11.readthedocs.io/
