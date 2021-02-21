#include "MutexLock.h"
#include "Thread.h"
#include <set>
#include <stdio.h>

class Request;

class Inventory
{
    private:
        mutable MutexLock mutex_;
        std::set<Request*> request_;
    
    public:
        void add(Request *req)
        {
            MutexLockGuard lock(mutex_);
            request_.insert(req);
        }

        void remove(Request *req)
        {
            MutexLockGuard lock(mutex_);
            request_.erase(req);
        }

        void printAll() const;
};

Inventory g_inventory;

class Request
{
    private:
        mutable MutexLock mutex_;
    
    public:
        void process()
        {
            MutexLockGuard lock(mutex_);
            g_inventory.add(this);
        }

        ~Request()
        {
            MutexLockGuard lock(mutex_);
            sleep(1);
            g_inventory.remove(this);
        }

        void print()
        {
            MutexLockGuard lock(mutex_);
            // ...
        }
};

void Inventory::printAll() const
{
    MutexLockGuard lock(mutex_);
    sleep(1);

    for (std::set<Request*>::const_iterator it = request_.begin(); it != request_.end(); ++it) {
        (*it)->print();
    }

    printf("Inventory::printAll() unlocked\n");
}

void threadFunc() {
    Request *req = new Request;
    req->process();
    delete req;
}

int main() {
    Thread th(threadFunc);
    th.start();
    usleep(500 * 1000);
    g_inventory.printAll();
    th.join();
}