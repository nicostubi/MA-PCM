#include <atomic>
#include <iostream>
#include <thread>

std::atomic_flag lock = ATOMIC_FLAG_INIT;

template<typename T>

class Queue {
private:
  T* elements;
  int head, tail;

  void enableLock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
        // Boucle d'attente active (spinlock)
    }
  }

  void disableLock() {
      lock.clear(std::memory_order_release);
  }

public:
  int capacity;

  Queue(int capacity) : elements(new T[capacity]), capacity(capacity), head(0), tail(0) {}

  ~Queue() {
    delete[] elements;
  }

  T deq() {
    while(tail == head);
    enableLock();
    T e = elements[head % capacity]; 
    head++;
    disableLock();
    return e;
  }

  void enq(T e) {
    while (tail == head + capacity);
    enableLock(); 
    elements[tail % capacity] = e; 
    tail++;
    disableLock();
  }
};

void producer(Queue<int>& queue) {
    for (int i = 0; i <= queue.capacity; i++) {
        queue.enq(i);
        std::cout << "Enqueued: " << i << std::endl;
    }
}

void consumer(Queue<int>& queue) {
    int value;
    for (int i = 0; i <= queue.capacity; ++i) {
        value = queue.deq();
        std::cout << "Dequeued: " << value << std::endl;
    }
}

int main() {
  Queue<int> queue(10);

  std::thread t2(consumer, std::ref(queue));
  std::thread t1(producer, std::ref(queue));

  t1.join();
  t2.join();

  std::cout << "Done! " << std::endl;

  return 0;
}