# Logger
- 项目[来源](https://github.com/chenshuo/muduo)
- 功能介绍
  - 日志级别
    - 用于调试程序
    - 测试环境使用DEBUG级别的日志
    - 生产环境使用INFO级别的日志
  - 日志轮转
    - 以本地文件为日志的destination，那么日志文件的滚动(rolling)是必需的，这样可以简化日志归档(archive)的实现
    - rolling的条件通常有两个
      - 文件大小(例如每写满1GB就换下一个文件)
      - 时间(例如每条零点新建一个日志文件，不论前一个文件是否写满)
      - 这个库的实现LogFile会自动根据文件大小和时间来主动滚动日志文件
  - 日志格式
  ```
  日期      时间     微妙     线程   级别  正文       源文件名：行号        
  20200823 15:29:51.006293Z 39063 INFO  Hello - asyncLogging_test.cpp:31
  20200823 15:29:51.006294Z 39063 INFO  Hello - asyncLogging_test.cpp:31
  ```

  - 阻塞日志(同步日志)
    - 性能需求
      - 4核，4GB内存的虚拟机
      - 测试日志数量：100万
      - 不写任何文件
        - 0.409192 seconds, 114888890 bytes, 2443840.54 msg/s, 267.76 MiB/s
      - 写普通文件
        - 0.441504 seconds, 114888890 bytes, 2264985.14 msg/s, 248.17 MiB/s
      - 写LogFile文件，线程不安全(不加锁)
        - 1.756032 seconds, 114888890 bytes,  569465.70 msg/s, 62.39 MiB/s
      - 写LogFile文件，线程安全(加锁)
        - 1.829351 seconds, 114888890 bytes,  546641.95 msg/s, 59.89 MiB/s
  - 非阻塞日志(异步日志)
    - 多线程异步日志需要线程安全保证，即多个线程可以写日志文件而不发生错乱
    - 简单的线程安全并不难办到，用一个额全局的Mutex对日志的IO操作进行保护或单独写一个日志文件即可，但是前者会造成多个线程竞争锁资源，后者会造成某个业务线程阻塞
    - 解决方案
      - 用一个背景线程负责将日志消息后将其写入后端，其他业务线程只负责生成日志并将其传输到日志线程
    - 实现
      - 应用双缓冲区技术，即存在两个Buffer，日志的实现分为前端和后端
      - 前端负责向CurrentBuffer中写，后弹负责将其写入文件中
      - 具体来说，当CurrentBuffer写满时，先讲CurrentBuffer中的消息存入Buffer中，再交换CurrentBuffer和NextBuffer
      - 这样前端日志就可以继续往CurrentBuffer中写入新的日志消息，最后再调用notify_all通知后端将其写入文件
- 功能图
  - ![avatar](./docs/logger_1.png)
