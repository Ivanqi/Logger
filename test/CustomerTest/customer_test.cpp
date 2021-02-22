#include <map>
#include <string>
#include <vector>
#include <memory>
#include "MutexLock.h"

using std::string;

/**
 * 场景： 一个多线程的C++程序，24h * 5.5d运行
 *  有几个工作线程Thread-Worker[0, 1, 2, 3]，处理客户发过来的交易请求
 *  另外有一个背景线程ThreadBackground,不定期更新程序内部的参考数据
 *  这些线程都跟一个hash表打交道，工作线程只读，背景线程读写，必然要用到一些同步机制，防止数据损坏
 */
class CustomerData: boost::noncopyable
{
    private:
        typedef std::pair<string, int> Entry;
        typedef std::vector<Entry> EntryList;
        typedef std::map<string, EntryList> Map;
        typedef std::shared_ptr<Map> MapPtr;

        mutable MutexLock mutex_;
        MapPtr data_;

    public:
        CustomerData():data_(new Map)
        {

        }

        // 用引用计数加1的办法，用局部MapPtr data变量来持有data，防止并发修改
        int query(const string& customer, const string& stock) const
        {
            MapPtr data = getData();
            
            /**
             * data 一旦拿到，就不再需要锁了
             * 取数据的时候只有 getData() 内部有锁，多线程并发读的性能很好
             */
            Map::const_iterator entries = data->find(customer);
            if (entries != data->end()) {
                return findEntry(entries->second, stock);
            } else {
                return -1;
            }
        }

    private:
        static int findEntry(const EntryList& entries, const string& stock);
        static MapPtr parseData(const string& message);  // 解析收到的信息，返回新的MapPtr

        MapPtr getData() const
        {
            MutexLockGuard lock(mutex_);
            return data_;
        }

        /**
         * update 方法。既然要更新数据，那肯定要得加锁，如果这时候其他线程正在读，那么不能在原来的数据上修改
         * 需要创建一个副本，在副本上修改，修改完了再替换
         * 如果没有用户在读，那么直接直接修改，节约一次Map拷贝
         */
        void update(const string& customer, const EntryList& entries)
        {
            MutexLockGuard lock(mutex_);    // 必须全程持锁
            if (!data_.unique()) {
                MapPtr newData(new Map(*data_));
                data_.swap(newData);
            }
    
            assert(data_.unique());
            (*data_)[customer] = entries;
        }

        // 如果每次都要更新全部数据，而且始终是在同一个线程更新，临界区还可以进一步缩小
        void udpate(const string& message)
        {
            // 解析新数据，在临界区之外
            MapPtr newData = parseData(message);   
            if (newData) {
                MutexLockGuard lock(mutex_);
                data_.swap(newData);    // 不要用 data_ = newData
            }
        }
};


int main() {
    
    CustomerData data;
    return 0;
}