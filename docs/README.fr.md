# Extension Python Fibonacci en C++

[English](../README.md) | [中文](README.zh-CN.md) | [日本語](README.ja.md) | [Français](README.fr.md) | [Deutsch](README.de.md)

Ce projet montre comment implémenter le calcul de Fibonacci en C++ et l'appeler depuis Python sous forme de module d'extension natif.

Après la compilation, Python peut l'utiliser ainsi :

```python
import fibonacci_cpp

print(fibonacci_cpp.fibonacci(10))  # 55
```

![Image de démonstration](Picture.png)

## Prérequis

- Python 3.x
- CMake 3.18 ou plus récent
- Un compilateur C++17, par exemple MinGW, MSVC ou Clang

Vérifiez que les commandes nécessaires sont disponibles :

```powershell
python --version
cmake --version
g++ --version
```

Le module d'extension Python doit être compilé et exécuté avec la même version de Python. La configuration CMake de ce projet privilégie l'exécutable `python` présent dans le `PATH` de la ligne de commande courante, afin que le module généré par `cmake --build build` puisse être importé directement avec `python main.py`.

Pour choisir un interpréteur Python précis, configurez CMake ainsi :

```powershell
cmake -S . -B build -DPython_EXECUTABLE="C:\Users\YourName\miniconda3\python.exe"
```

## Compilation

Exécutez ces commandes depuis la racine du projet :

```powershell
cmake -S . -B build
cmake --build build
```

Après une compilation réussie, le dossier `build` contient un fichier semblable à :

```text
fibonacci_cpp.cp313-win_amd64.pyd
```

L'étiquette de version Python dans le nom du fichier peut varier. Par exemple, `cp313` signifie CPython 3.13.

La configuration CMake essentielle est :

```cmake
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

add_library(fibonacci_cpp MODULE
    src/fibonacci.cpp
)

target_link_libraries(fibonacci_cpp PRIVATE Python::Module)
```

Le module généré s'appelle `fibonacci_cpp`, il s'importe donc en Python avec `import fibonacci_cpp`.

## Utilisation

`src/fibonacci.cpp` expose une fonction via l'API C de Python :

```python
fibonacci_cpp.fibonacci(n)
```

Notes :

- `n` doit être un entier positif ou nul.
- La valeur de retour est un `int` Python, donc les grands résultats sont pris en charge.
- Côté C++, `n` est actuellement converti en `unsigned long long`, donc l'indice maximal accepté est `18446744073709551615`. Les limites pratiques dépendent toujours de la mémoire et du temps d'exécution.

Lancer la démonstration :

```powershell
python main.py
```

Lancer un benchmark plus grand :

```powershell
python main.py 1000000
```

## Benchmark Wall Time

`main.py` effectue deux opérations :

- Il affiche de petits exemples de `fibonacci_cpp.fibonacci(0)` à `fibonacci_cpp.fibonacci(10)`.
- Il compare le Wall Time d'une implémentation Python pure et de l'extension C++ pour le même grand `n`.

Benchmark par défaut :

```powershell
python main.py
```

Exemple de résultat :

```text
Benchmark n = 100,000
Result decimal digits : 20,899
Python Wall Time     : 0.052376 s
C++ Wall Time        : 0.000816 s
Speedup              : 64.18x
```

Benchmark avec `n = 1000000` :

```powershell
python main.py 1000000
```

Exemple de résultat :

```text
Benchmark n = 1,000,000
Result decimal digits : 208,988
Python Wall Time     : 4.540529 s
C++ Wall Time        : 0.033997 s
Speedup              : 133.56x
```

Le Wall Time dépend de la machine, de la version de Python, du compilateur et de la charge en arrière-plan. Ces chiffres ne sont donc que des références. Pour une mesure plus stricte, exécutez plusieurs itérations et calculez une moyenne, ou utilisez un outil spécialisé comme `pyperf`.

## Pourquoi C++ aide ici

Les extensions C++ sont utiles pour les chemins de calcul intensifs. Dans ce projet :

- La version Python utilise une boucle simple, donc chaque étape passe par l'exécution du bytecode Python et la liaison des variables.
- L'extension C++ déplace le calcul coûteux dans un module compilé, ce qui réduit le surcoût des boucles côté Python.
- L'implémentation C++ utilise la méthode de doublement rapide, qui calcule Fibonacci avec environ `O(log n)` opérations sur grands entiers au lieu de `O(n)` itérations.
- Python garde le point d'entrée et l'affichage, tandis que C++ gère le calcul lourd.

Tout code Python ne devient pas automatiquement plus rapide en C++. Les appels entre langages ont aussi un coût. Cette approche convient surtout aux parties stables, intensives en calcul et appelées avec une fréquence maîtrisée.

## Ressources

- Tutoriel officiel Python : Extending and Embedding the Python Interpreter  
  https://docs.python.org/3/extending/

- Référence officielle Python : Python/C API Reference Manual  
  https://docs.python.org/3/c-api/index.html

- Tutoriel Real Python : Building a Python C Extension Module  
  https://realpython.com/build-python-c-extension-module/

- Pour une enveloppe C++ plus moderne, voir pybind11  
  https://pybind11.readthedocs.io/
