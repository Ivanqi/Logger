#ifndef LOGGER_PROCESSINFO_H
#define LOGGER_PROCESSINFO_H

#include <vector>
#include <sys/types.h>
#include "Timestamp.h"

using namespace std;

namespace PrcoessInfo
{
    pid_t pid();
    string pidString();

    uid_t uid();
    string username();

    uid_t euid();
    Timestamp startTime();

    int clockTicksPerSecond();

    int pageSize();

    bool isDebugBuild();

    string hostname();

    string procname();

};
#endif