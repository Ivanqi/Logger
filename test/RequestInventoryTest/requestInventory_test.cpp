#include "MutexLock.h"
#include "Thread.h"
#include <set>
#include <memory>

class Request;
typedef std::shared_ptr<Request> RequestPtr;

class Inventory
{
    private:
        typedef std::set<RequestPtr> RequestList;
        typedef std::shared_ptr<RequestList> RequestListPtr;
        mutable MutexLock mutex_;
        RequestListPtr requests_;

    private:
        RequestListPtr getData() const
        {
            MutexLockGuard lock(mutex_);
            return requests_;
        }

    public:
        Inventory(): requests_(new RequestList())
        {

        }

        void add(const RequestPtr& req)
        {
            MutexLockGuard lock(mutex_);
            if (!requests_.unique()) {
                requests_.reset(new RequestList(*requests_));
                printf("Inventory:add() copy the whole list\n");
            }

            assert(requests_.unique());
            requests_->insert(req);
        }

        void remove(const RequestPtr& req)
        {
            MutexLockGuard lock(mutex_);
            if (!requests_.unique()) {
                requests_.reset(new RequestList(*requests_));
                printf("Inventory:remove() copy the whole list\n");
            }

            assert(requests_.unique());
            requests_->erase(req);
        }

        void printAll() const;
};

Inventory g_inventory;

class Request: public std::enable_shared_from_this<Request>
{   
    private:
        mutable MutexLock mutex_;
        int x_;

    public:
        Request(): x_(0)
        {

        }

        ~Request()
        {
            x_ = -1;
        }

        void cancel()
        {
            MutexLockGuard lock(mutex_);
            x_ = 1;
            sleep(1);
            printf("cancel()\n");
            g_inventory.remove(shared_from_this());
        }

        void process()
        {
            MutexLockGuard lock(mutex_);
            g_inventory.add(shared_from_this());
        }

        void print() const
        {
            MutexLockGuard lock(mutex_);
            printf("print Request %p x = %d\n", this, x_);
        }
};


void Inventory::printAll() const
{
    RequestListPtr requests = getData();
    printf("printAll()");
    sleep(1);

    for (std::set<RequestPtr>::const_iterator it = requests->begin(); it != requests->end(); ++it) {
        (*it)->print();
    }
}

void threadFunc()
{
    RequestPtr req(new Request);
    req->process();
    req->cancel();
}

int main() {

    Thread th(threadFunc);
    th.start();
    usleep(500 * 1000);
    g_inventory.printAll();
    th.join();
    return 0;
}