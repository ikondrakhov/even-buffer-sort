#include <iostream>
#include <vector>
#include <optional>
#include <cassert>
#include <queue>

bool isEven(int value) {
    return !(value & 1);
}

/*
Циклический буфер через вектор
Преимущества:
Быстрое добавление, удаление и доступ к элементам
*/
template <typename T>
class RingBufferV {
public:
    explicit RingBufferV(size_t size) : buffer(size) {
    }

    void PushBack(T value) {
        buffer[write_pos % buffer.size()] = value;
        if (size_ != 0 && write_pos == read_pos) {
            read_pos = (read_pos + 1) % buffer.size();
        }
        write_pos = (write_pos + 1) % buffer.size();
        size_++;
    }

    void Pop() {
        buffer[read_pos].reset();
        read_pos = (read_pos + 1) % size_;
        size_--;
    }

    T operator[](size_t pos) {
        if (pos < size_ && buffer[pos].has_value()) {
            return buffer[pos].value();
        }
        throw std::runtime_error("Invalid position");
    }

private:
    std::vector<std::optional<T>> buffer;
    size_t read_pos = 0;
    size_t write_pos = 0;
    size_t size_ = 0;
};

/*
Циклический буфер через очередь
Быстрое добавление и удаление элеметов
Если добавлять операцию доступа к элементу по индексу, то скорость будет линейной
*/
template<typename T>
class RingBufferQ {
public:
    explicit RingBufferQ(size_t size) : size_(size) {
    }

    void PushBack(T value) {
        if (buffer.size() == size_) {
            throw std::runtime_error("Buffer is full");
        }
        buffer.push(value);
    }

    T Pop() {
        T value = buffer.front();
        buffer.pop();
        return value;
    }

    bool IsEmpty() {
        return buffer.empty();
    }

private:
    std::queue<T> buffer;
    size_t size_;
};

void TestRingbuffer() {
    RingBufferV<int> rb(3);

    rb.PushBack(1);
    rb.PushBack(2);
    rb.PushBack(3);

    assert(rb[0] == 1);
    assert(rb[1] == 2);
    assert(rb[2] == 3);

    rb.PushBack(4);

    assert(rb[0] == 4);

    rb.Pop();

    assert(rb[0] == 4);
    assert(rb[2] == 3);
    try {
        rb[1];
        assert(false);
    }
    catch (std::runtime_error& e) {
        assert(true);
    }

    std::cout << "Tests passed" << std::endl;
}

void TestRingbufferQ() {
    RingBufferQ<int> rb(3);

    rb.PushBack(1);
    rb.PushBack(2);
    rb.PushBack(3);

    try {
        rb.PushBack(4);
        assert(false);
    }
    catch (std::runtime_error& e) {
        assert(true);
    }

    assert(rb.Pop() == 1);
    assert(rb.Pop() == 2);
    assert(rb.Pop() == 3);

    rb.PushBack(4);

    assert(rb.Pop() == 4);

    assert(rb.IsEmpty());

    std::cout << "Tests passed" << std::endl;
}

/*
Реализация сортировки слиянием
Скорость O(n*log(n))
*/
std::vector<int> merge(const std::vector<int>& vl, const std::vector<int>& vr) {
    std::vector<int> result(vl.size() + vr.size());

    int pos = 0;
    int i, j;
    for (i = 0, j = 0; i < vl.size() && j < vr.size();) {
        if (vl[i] > vr[j]) {
            result[pos++] = vr[j++];
        }
        else {
            result[pos++] = vl[i++];
        }
    }
    while (i < vl.size()) {
        result[pos++] = vl[i++];
    }
    while (j < vr.size()) {
        result[pos++] = vr[j++];
    }
    return result;
}

std::vector<int> mergeSort(std::vector<int>& v, int start, int end) {
    int size = end - start;
    if (size == 1) {
        return std::vector<int>(v.begin() + start, v.begin() + end);
    }

    return merge(mergeSort(v, start, start + size / 2), mergeSort(v, start + size / 2, end));
}

int main() {
    TestRingbuffer();
    TestRingbufferQ();

    std::vector<int> test = { 5, 4, 3, 2, 7, 4, 5 };

    for (int i : mergeSort(test, 0, test.size())) {
        std::cout << i << std::endl;
    }

    for(int i = 0; i < 20; i++) {
        if(i % 2 == 0) {
            assert(isEven(i));
        } else {
            assert(!isEven(i));
        }
    }
}