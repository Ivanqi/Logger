#include "MutexLock.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <stdio.h>

class Foo
{
    public:
        void doit() const;
};

typedef std::vector<Foo> FooList;
typedef std::shared_ptr<FooList> FooListPtr;
FooListPtr g_foos;
MutexLock mutex;

/**
 * 如果g_foos.unique()为true，可以放心原地(in-place)修改FooList
 * 如果g_foos.unique()为false，说明这时别的线程正在读取Foolist，不能原地修改，而是复制一份，在副本上修改
 * 这样就可以避免死锁
 */
void post(const Foo& f) {
    printf("post\n");
    MutexLockGuard lock(mutex);
    if (!g_foos.unique()) { // 检查所管理对象是否仅由当前 shared_ptr 的实例管理
        g_foos.reset(new FooList(*g_foos));
        printf("copy the whole list\n");
    }

    assert(g_foos.unique());
    g_foos->push_back(f);
}

void traverse() {
    FooListPtr foos;
    {
        MutexLockGuard lock(mutex);
        foos = g_foos;
        assert(!g_foos.unique());
    }

    for (std::vector<Foo>::const_iterator it = foos->begin(); it != foos->end(); ++it) {
        it->doit();
    }
}

void Foo::doit() const {
    Foo f;
    post(f);
}

int main() {

    g_foos.reset(new FooList);
    Foo f;
    post(f);
    traverse();
    return 0;
}
