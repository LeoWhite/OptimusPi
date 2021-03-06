/**
 * MessageQueue
 *
 * A (hopefully) thread safe queue for passing messages to
 * a thread for processing. To avoid the need for polling a
 * eventFD is provided to allow use of select or poll in the
 * thread's main loop.
 *
 * Code is based on several examples found online.
 */
#ifndef _PIWARS_MESSAGEQUEUE_H
#define _PIWARS_MESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <sys/eventfd.h>

namespace PiWars {

template <class T> class MessageQueue {
  public:
    MessageQueue() {
      _fd = eventfd(0, EFD_NONBLOCK);
    }

    ~MessageQueue() {
      close(_fd);
    }

    // Returns the FD that the thread can block on
    //
    // @returns The file descriptor
    int getFD() { return _fd; };

    // Checks if the queue is empty, to avoid unneccessary blocking
    //
    // @returns true if the queue is empty
    //          false otherwise
    bool empty() {
      std::unique_lock<std::mutex> lock(_mutex);
      return _queue.empty();
    }

    // Pops the first element from the queue
    //
    // @param item Where the popped element is written to
    void pop(T& item) {
      std::unique_lock<std::mutex> lock(_mutex);

      while(_queue.empty()) {
        _condition.wait(lock);
      }
      item = _queue.front();

      _queue.pop();

      uint64_t value;

      // Read and discard the event value to drop the count
      read(_fd, &value, sizeof(value));
    }

    // Pushs an element onto the end of the queue
    //
    // @param item The Item to push
    void push(const T& item) {
      std::unique_lock<std::mutex> lock(_mutex);

      _queue.push(item);

      // Write to the FD in case there's a thread
      uint64_t value = 1;
      write(_fd, &value, sizeof(value));

      // Unlock the mutex and wake up anyone waiting
      lock.unlock();
      _condition.notify_one();
    }

  private:
    int _fd; //<! The file descriptor of the eventfd
    std::queue<T> _queue; //<! A basic queue for the items
    mutable std::mutex _mutex; //<! A mutex to protect access to the queue
    std::condition_variable _condition; //<! A condition to help handle multiple threads
};

}

#endif
