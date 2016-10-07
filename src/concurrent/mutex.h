#pragma once

#include <pthread.h>
#include "exception/errno_exception.h"


namespace std {

/**
 * 利用 pthread 读写锁实现的 shared_mutex.
 *
 * 接口与 c++17 中待引入的 shared_mutex 保持一致.
 */
struct shared_mutex {
    shared_mutex() {
        int pthread_rc = pthread_rwlock_init(&rwlock_, nullptr);
        if (pthread_rc != 0) {
            THROW(pthread_rc, "pthread_rwlock_init");
        }
        return ;
    }

    ~shared_mutex() noexcept {
        int pthread_rc = pthread_rwlock_destroy(&rwlock_);
        if (pthread_rc != 0) {
            THROW(pthread_rc, "pthread_rwlock_destroy");
        }
        return ;
    }

    void lock() {
        int pthread_rc = pthread_rwlock_wrlock(&rwlock_);
        if (pthread_rc != 0) {
            THROW(pthread_rc, "pthread_rwlock_wrlock");
        }
        return ;
    }

    bool try_lock() noexcept {
        return pthread_rwlock_trywrlock(&rwlock_) == 0;
    }

    void unlock() {
        int pthread_rc = pthread_rwlock_unlock(&rwlock_);
        if (pthread_rc != 0) {
            THROW(pthread_rc, "pthread_rwlock_unlock");
        }
        return ;
    }

    void lock_shared() {
        int pthread_rc = pthread_rwlock_rdlock(&rwlock_);
        if (pthread_rc != 0) {
            THROW(pthread_rc, "pthread_rwlock_rdlock");
        }
        return ;
    }

    bool try_lock_shared() noexcept {
        return pthread_rwlock_tryrdlock(&rwlock_) == 0;
    }

    void unlock_shared() {
        unlock();
        return ;
    }

    // native_handle_type native_handle() = delete;

private:
    pthread_rwlock_t rwlock_;

private:
    shared_mutex(const shared_mutex &) = delete;
    shared_mutex(shared_mutex &&) = delete;
    shared_mutex& operator=(const shared_mutex &) = delete;
    shared_mutex& operator=(shared_mutex &&) = delete;
};

} // namespace std
