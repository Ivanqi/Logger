#include <iostream>
#include <Logging.h>
#include <Thread.h>
#include <string.h>
#include <unistd.h>
using namespace std;


void test_logger_1() {
    Logger log(__FILE__, __LINE__);
    log.stream() << "aaaa";
    log.stream() << "bbbb";

    cout << log.getBuffer() << endl;
}

void test_logger_2() {
    LOG << "asss";
}

void test_logger_3() {
    string logPath = "./log.log";
    Logger::setLogFileName(logPath);
    LOG << "Hooray! It's snowing! It's time to make a snowman.";
    LOG << "Hooray! It's snowing! It's time to make a snowman.";
    LOG << "Hooray! It's snowing! It's time to make a snowman.";
    LOG << "Hooray! It's snowing! It's time to make a snowman.";
    LOG << "Hooray! It's snowing! It's time to make a snowman.";
}

int main() {
    
    test_logger_3();
    return 0;
}