#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <utility>

namespace {

class PyPtr {
public:
    PyPtr() = default;

    explicit PyPtr(PyObject* object) : object_(object) {}

    PyPtr(const PyPtr&) = delete;
    PyPtr& operator=(const PyPtr&) = delete;

    PyPtr(PyPtr&& other) noexcept : object_(std::exchange(other.object_, nullptr)) {}

    PyPtr& operator=(PyPtr&& other) noexcept {
        if (this != &other) {
            Py_XDECREF(object_);
            object_ = std::exchange(other.object_, nullptr);
        }
        return *this;
    }

    ~PyPtr() {
        Py_XDECREF(object_);
    }

    PyObject* get() const {
        return object_;
    }

    PyObject* release() {
        return std::exchange(object_, nullptr);
    }

    explicit operator bool() const {
        return object_ != nullptr;
    }

private:
    PyObject* object_ = nullptr;
};

struct FibPair {
    PyPtr current;
    PyPtr next;
};

PyPtr py_long_from_unsigned(unsigned long long value) {
    return PyPtr(PyLong_FromUnsignedLongLong(value));
}

PyPtr py_add(PyObject* left, PyObject* right) {
    return PyPtr(PyNumber_Add(left, right));
}

PyPtr py_subtract(PyObject* left, PyObject* right) {
    return PyPtr(PyNumber_Subtract(left, right));
}

PyPtr py_multiply(PyObject* left, PyObject* right) {
    return PyPtr(PyNumber_Multiply(left, right));
}

FibPair fibonacci_pair(unsigned long long n) {
    if (n == 0) {
        return {py_long_from_unsigned(0), py_long_from_unsigned(1)};
    }

    FibPair half = fibonacci_pair(n / 2);
    if (!half.current || !half.next) {
        return {};
    }

    PyPtr two = py_long_from_unsigned(2);
    if (!two) {
        return {};
    }

    PyPtr two_b = py_multiply(two.get(), half.next.get());
    if (!two_b) {
        return {};
    }

    PyPtr two_b_minus_a = py_subtract(two_b.get(), half.current.get());
    if (!two_b_minus_a) {
        return {};
    }

    PyPtr c = py_multiply(half.current.get(), two_b_minus_a.get());
    if (!c) {
        return {};
    }

    PyPtr a_squared = py_multiply(half.current.get(), half.current.get());
    if (!a_squared) {
        return {};
    }

    PyPtr b_squared = py_multiply(half.next.get(), half.next.get());
    if (!b_squared) {
        return {};
    }

    PyPtr d = py_add(a_squared.get(), b_squared.get());
    if (!d) {
        return {};
    }

    if (n % 2 == 0) {
        return {std::move(c), std::move(d)};
    }

    PyPtr next = py_add(c.get(), d.get());
    if (!next) {
        return {};
    }

    return {std::move(d), std::move(next)};
}

PyObject* py_fibonacci(PyObject*, PyObject* args) {
    PyObject* n_object = nullptr;
    if (!PyArg_ParseTuple(args, "O", &n_object)) {
        return nullptr;
    }

    if (!PyLong_Check(n_object)) {
        PyErr_SetString(PyExc_TypeError, "fibonacci(n) requires an integer");
        return nullptr;
    }

    PyPtr zero = py_long_from_unsigned(0);
    if (!zero) {
        return nullptr;
    }

    const int is_negative = PyObject_RichCompareBool(n_object, zero.get(), Py_LT);
    if (is_negative < 0) {
        return nullptr;
    }
    if (is_negative) {
        PyErr_SetString(PyExc_ValueError, "fibonacci(n) requires n >= 0");
        return nullptr;
    }

    const unsigned long long n = PyLong_AsUnsignedLongLong(n_object);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_OverflowError, "fibonacci(n) currently supports n <= 18446744073709551615");
        return nullptr;
    }

    FibPair result = fibonacci_pair(n);
    if (!result.current) {
        return nullptr;
    }

    return result.current.release();
}

PyMethodDef fibonacci_methods[] = {
    {
        "fibonacci",
        py_fibonacci,
        METH_VARARGS,
        "Return the n-th Fibonacci number as a Python int.",
    },
    {nullptr, nullptr, 0, nullptr},
};

PyModuleDef fibonacci_module = {
    PyModuleDef_HEAD_INIT,
    "fibonacci_cpp",
    "A Fibonacci extension module written with the Python C API.",
    -1,
    fibonacci_methods,
};

}  // namespace

PyMODINIT_FUNC PyInit_fibonacci_cpp() {
    return PyModule_Create(&fibonacci_module);
}
