from __future__ import annotations

from pathlib import Path
import sys
from time import perf_counter
from typing import Callable


try:
    sys.set_int_max_str_digits(0)
except AttributeError:
    pass


def add_build_dir_to_path() -> None:
    """Allow running this demo directly from the project root."""
    build_dir = Path(__file__).resolve().parent / "build"
    if build_dir.exists():
        sys.path.insert(0, str(build_dir))


add_build_dir_to_path()

import fibonacci_cpp  # noqa: E402


def fibonacci_python(n: int) -> int:
    if n < 0:
        raise ValueError("fibonacci_python(n) requires n >= 0")

    previous = 0
    current = 1
    for _ in range(n):
        previous, current = current, previous + current
    return previous


def measure_wall_time(function: Callable[[], int]) -> tuple[int, float]:
    start = perf_counter()
    result = function()
    elapsed = perf_counter() - start
    return result, elapsed


def print_small_examples() -> None:
    for n in range(11):
        print(f"fibonacci_cpp.fibonacci({n}) = {fibonacci_cpp.fibonacci(n)}")


def run_benchmark(n: int) -> None:
    print(f"\nBenchmark n = {n:,}")

    python_result, python_seconds = measure_wall_time(lambda: fibonacci_python(n))
    cpp_result, cpp_seconds = measure_wall_time(lambda: fibonacci_cpp.fibonacci(n))

    if python_result != cpp_result:
        raise RuntimeError("Python and C++ results are different")

    digit_count = len(str(cpp_result))
    speedup = python_seconds / cpp_seconds if cpp_seconds > 0 else float("inf")

    print(f"Result decimal digits : {digit_count:,}")
    print(f"Python Wall Time     : {python_seconds:.6f} s")
    print(f"C++ Wall Time        : {cpp_seconds:.6f} s")
    print(f"Speedup              : {speedup:.2f}x")


def main() -> None:
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 100_000
    print_small_examples()
    run_benchmark(n)


if __name__ == "__main__":
    main()
