// LR3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <stdexcept>
#include <algorithm> // для std::swap
#include <cassert>

template <typename T>
class Vector {
private:
    T* data;      // указатель на динамический массив
    int sz;       // текущий размер (количество элементов)
    int cap;      // ёмкость (выделенная память)

public:
    // Конструкторы и деструктор (Правило пяти)

    // Конструктор по умолчанию
    Vector() : data(nullptr), sz(0), cap(0) {}

    // Конструктор с параметром (размер)
    explicit Vector(int n) : sz(n), cap(n) {
        if (n < 0) throw std::invalid_argument("Size cannot be negative");
        data = cap > 0 ? new T[cap]() : nullptr; // () для value-инициализации
    }

    // Конструктор копирования (глубокое копирование)
    Vector(const Vector& other) : sz(other.sz), cap(other.cap) {
        data = cap > 0 ? new T[cap] : nullptr;
        for (int i = 0; i < sz; ++i) {
            data[i] = other.data[i];
        }
    }

    // Конструктор перемещения (noexcept обязателен для эффективности)
    Vector(Vector&& other) noexcept
        : data(other.data), sz(other.sz), cap(other.cap) {
        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    // Деструктор
    ~Vector() {
        delete[] data;
    }

    // Оператор присваивания копированием (copy-and-swap idiom)
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other); // копируем
            swap(temp);         // обмениваемся
        }
        return *this;
    }

    // Оператор присваивания перемещением
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data; // освобождаем свои ресурсы

            data = other.data;
            sz = other.sz;
            cap = other.cap;

            other.data = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    // Методы доступа
    int size() const { return sz; }
    int capacity() const { return cap; }
    bool empty() const { return sz == 0; }

    // Оператор доступа по индексу (с проверкой границ)
    T& operator[](int index) {
        if (index < 0 || index >= sz) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= sz) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // Методы изменения
    void push_back(const T& value) {
        if (sz >= cap) {
            int newCap = (cap == 0) ? 1 : cap * 2;
            reserve(newCap);
        }
        data[sz++] = value;
    }

    void pop_back() {
        if (empty()) {
            throw std::underflow_error("Vector is empty");
        }
        --sz;
    }

    // Дополнительная функциональность (Вариант 1)
    // Метод reserve: резервирует память под newCap элементов
    void reserve(int newCap) {
        if (newCap <= cap) return; 

        T* newData = new T[newCap];

        for (int i = 0; i < sz; ++i) {
            newData[i] = std::move_if_noexcept(data[i]);
        }

        delete[] data;
        data = newData;
        cap = newCap;
    }

    // Метод shrink_to_fit: уменьшает ёмкость до размера (sz)
    void shrink_to_fit() {
        if (sz == cap) return; 

        if (sz == 0) {
            delete[] data;
            data = nullptr;
            cap = 0;
            return;
        }

        // Выделяем память ровно под текущий размер
        T* newData = new T[sz];
        for (int i = 0; i < sz; ++i) {
            newData[i] = std::move_if_noexcept(data[i]);
        }

        delete[] data;
        data = newData;
        cap = sz;
    }

    // Вспомогательные методы
    void swap(Vector& other) noexcept {
        std::swap(data, other.data);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    void clear() {
        sz = 0;
    }
};

// Демонстрационная программа
int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "=== Демонстрация работы Vector (Вариант 1) ===" << std::endl;

    // Создание и заполнение
    Vector<int> v;
    for (int i = 1; i <= 5; ++i) {
        v.push_back(i * 10);
    }
    std::cout << "Исходный вектор: размер=" << v.size()
        << ", ёмкость=" << v.capacity() << std::endl;

    // Демонстрация reserve
    std::cout << "\n-- Вызов reserve(20) --" << std::endl;
    v.reserve(20);
    std::cout << "После reserve(20): размер=" << v.size()
        << ", ёмкость=" << v.capacity() << std::endl;

    // Демонстрация shrink_to_fit
    std::cout << "\n-- Вызов shrink_to_fit() --" << std::endl;
    v.shrink_to_fit();
    std::cout << "После shrink_to_fit(): размер=" << v.size()
        << ", ёмкость=" << v.capacity() << std::endl;

    // Проверка "Правила пяти"
    std::cout << "\n-- Проверка 'Правила пяти' --" << std::endl;

    // Конструктор копирования
    Vector<int> v_copy = v;
    std::cout << "Копия создана. Ёмкость копии: " << v_copy.capacity() << std::endl;

    // Конструктор перемещения
    Vector<int> v_move = std::move(v_copy);
    std::cout << "Перемещение выполнено. Размер перемещённого: " << v_move.size()
        << ", размер оригинала: " << v_copy.size() << std::endl;

    // Оператор присваивания копированием
    Vector<int> v_assign;
    v_assign = v;
    std::cout << "Присваивание копированием выполнено. Размер: " << v_assign.size() << std::endl;

    // Оператор присваивания перемещением
    Vector<int> v_move_assign;
    v_move_assign = std::move(v_move);
    std::cout << "Присваивание перемещением выполнено. Размер: " << v_move_assign.size() << std::endl;

    // Вывод элементов
    std::cout << "\nЭлементы итогового вектора: ";
    for (int i = 0; i < v_move_assign.size(); ++i) {
        std::cout << v_move_assign[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
