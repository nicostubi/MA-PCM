#include <atomic>
#include <iostream>
#include <thread>

std::atomic_flag lock = ATOMIC_FLAG_INIT;

template<typename T>

class Queue {
private:
  T* elements;
  int head, tail, capacity;

  void enableLock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
        // Boucle d'attente active (spinlock)
    }
  }

  void disableLock() {
      lock.clear(std::memory_order_release);
  }

public:
  Queue(int capacity) : head(0), tail(0), capacity(capacity) {
    elements = new T[capacity];
  }

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
    for (int i = 0; i <= 1000; i++) {
        queue.enq(i);
        std::cout << "Enqueued: " << i << std::endl;
    }
}

void consumer(Queue<int>& queue) {
    int value;
    for (int i = 0; i <= 1000; ++i) {
        value = queue.deq();
    }
}

int main() {
  Queue<int> queue(1000);

  std::thread t1(producer, std::ref(queue));
  std::thread t2(consumer, std::ref(queue));

  t1.join();
  t2.join();

  std::cout << "Done! " << std::endl;

  return 0;
}