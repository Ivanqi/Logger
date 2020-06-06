#include <iostream>
#include <Logging.h>
using namespace std;

void test_case_1() {
    Logger log(__FILE__, __LINE__);
    log.stream() << "aaaa";
    log.stream() << "bbbb";

    cout << log.getBuffer() << endl;
}

void test_case_2() {
    LOG << "asss";
}

int main() {
    
    test_case_1();
    return 0;
}