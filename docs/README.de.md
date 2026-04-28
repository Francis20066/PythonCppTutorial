# Fibonacci-Python-Erweiterung in C++

[English](../README.md) | [中文](README.zh-CN.md) | [日本語](README.ja.md) | [Français](README.fr.md) | [Deutsch](README.de.md)

Dieses Projekt zeigt, wie Fibonacci in C++ implementiert und als natives Python-Erweiterungsmodul aus Python aufgerufen werden kann.

Nach dem Build kann Python das Modul so verwenden:

```python
import fibonacci_cpp

print(fibonacci_cpp.fibonacci(10))  # 55
```

![Demo-Bild](Picture.png)

## Voraussetzungen

- Python 3.x
- CMake 3.18 oder neuer
- Ein C++17-Compiler, zum Beispiel MinGW, MSVC oder Clang

Prüfen Sie, ob die benötigten Befehle verfügbar sind:

```powershell
python --version
cmake --version
g++ --version
```

Das Python-Erweiterungsmodul muss mit derselben Python-Version gebaut und ausgeführt werden. Die CMake-Konfiguration dieses Projekts bevorzugt das `python`-Programm aus dem aktuellen Kommandozeilen-`PATH`, sodass das mit `cmake --build build` erzeugte Modul direkt über `python main.py` importiert werden kann.

Um einen bestimmten Python-Interpreter auszuwählen, konfigurieren Sie CMake so:

```powershell
cmake -S . -B build -DPython_EXECUTABLE="C:\Users\YourName\miniconda3\python.exe"
```

## Build

Führen Sie diese Befehle im Projektstamm aus:

```powershell
cmake -S . -B build
cmake --build build
```

Nach einem erfolgreichen Build enthält der Ordner `build` eine Datei ähnlich wie:

```text
fibonacci_cpp.cp313-win_amd64.pyd
```

Das Python-Versionskennzeichen im Dateinamen kann abweichen. `cp313` steht zum Beispiel für CPython 3.13.

Die zentrale CMake-Konfiguration lautet:

```cmake
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

add_library(fibonacci_cpp MODULE
    src/fibonacci.cpp
)

target_link_libraries(fibonacci_cpp PRIVATE Python::Module)
```

Das erzeugte Modul heißt `fibonacci_cpp` und wird in Python mit `import fibonacci_cpp` importiert.

## Verwendung

`src/fibonacci.cpp` stellt über die Python C API eine Funktion bereit:

```python
fibonacci_cpp.fibonacci(n)
```

Hinweise:

- `n` muss eine nicht negative Ganzzahl sein.
- Der Rückgabewert ist ein Python-`int`, daher werden große Ergebnisse unterstützt.
- Auf der C++-Seite wird `n` derzeit als `unsigned long long` eingelesen. Der größte akzeptierte Index ist also `18446744073709551615`. Die praktischen Grenzen hängen weiterhin von Speicher und Laufzeit ab.

Demo ausführen:

```powershell
python main.py
```

Größeren Benchmark ausführen:

```powershell
python main.py 1000000
```

## Wall-Time-Benchmark

`main.py` macht zwei Dinge:

- Es gibt kleine Beispiele von `fibonacci_cpp.fibonacci(0)` bis `fibonacci_cpp.fibonacci(10)` aus.
- Es vergleicht die Wall Time einer reinen Python-Implementierung mit der C++-Erweiterung für dasselbe große `n`.

Standard-Benchmark:

```powershell
python main.py
```

Beispielergebnis:

```text
Benchmark n = 100,000
Result decimal digits : 20,899
Python Wall Time     : 0.052376 s
C++ Wall Time        : 0.000816 s
Speedup              : 64.18x
```

Benchmark mit `n = 1000000`:

```powershell
python main.py 1000000
```

Beispielergebnis:

```text
Benchmark n = 1,000,000
Result decimal digits : 208,988
Python Wall Time     : 4.540529 s
C++ Wall Time        : 0.033997 s
Speedup              : 133.56x
```

Die Wall Time hängt von Maschine, Python-Version, Compiler und Hintergrundlast ab. Diese Zahlen sind daher nur Referenzwerte. Für genauere Messungen sollten mehrere Durchläufe gemittelt oder ein Werkzeug wie `pyperf` verwendet werden.

## Warum C++ hier hilft

C++-Erweiterungen eignen sich gut für rechenintensive Hot Paths. In diesem Projekt:

- Die Python-Version nutzt eine einfache Schleife, sodass jeder Schritt über Python-Bytecode-Ausführung und Variablenbindung läuft.
- Die C++-Erweiterung verlagert die rechenintensive Logik in ein kompiliertes Modul und reduziert damit den Schleifen-Overhead auf Python-Ebene.
- Die C++-Implementierung verwendet schnelles Verdoppeln und berechnet Fibonacci mit ungefähr `O(log n)` Großzahloperationen statt mit `O(n)` Schleifeniterationen.
- Python übernimmt weiterhin Einstiegspunkt und Ausgabe, während C++ die schwere Berechnung erledigt.

Nicht jedes Python-Programm wird schneller, wenn Code nach C++ verschoben wird. Sprachübergreifende Aufrufe haben ebenfalls Kosten. Dieser Ansatz passt besonders gut zu stabiler, rechenintensiver Logik mit kontrollierter Aufruffrequenz.

## Lernressourcen

- Offizielles Python-Tutorial: Extending and Embedding the Python Interpreter  
  https://docs.python.org/3/extending/

- Offizielle Python-Referenz: Python/C API Reference Manual  
  https://docs.python.org/3/c-api/index.html

- Real-Python-Tutorial: Building a Python C Extension Module  
  https://realpython.com/build-python-c-extension-module/

- Für eine modernere C++-Abstraktion siehe pybind11  
  https://pybind11.readthedocs.io/
