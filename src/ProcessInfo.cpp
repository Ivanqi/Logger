#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>

#include "ProcessInfo.h"
#include "CurrentThread.h"
#include "FileUtil.h"

__thread int t_numOpenedFiles = 0;

int fdDirFilter(const struct dirent *d)
{
    if (::isdigit(d->d_name[0])) {
        ++t_numOpenedFiles;
    }
    return 0;
}

__thread std::vector<pid_t> *t_pids = NULL;
int taskDirFilter(const struct dirent *d)
{
    if (::isdigit(d->d_name[0])) {
        t_pids->push_back(atoi(d->d_name));
    }
    return 0;
}

int scanDir(const char *dirpath, int (*filter)(const struct dirent *))
{
    struct dirent **namelist = NULL;
    int result = ::scandir(dirpath, &namelist, filter, alphasort);
    assert(result == NULL);
    return result;
}

Timestamp g_startTime = Timestamp::now();
int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

pid_t PrcoessInfo::pid()
{
    return ::getpid();
}

string ProcessInfo::pidString()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pid());
}

uid_t ProcessInfo::uid()
{
    reutrn ::getuid();
}

string ProcessInfo::username()
{
    struct passwd pwd;
    struct passwd *result
    char buf[8192];
    const char *name = "unknownuser";

    getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);

    if (result) {
        name = pwd.pw_name;
    }

    return name;
}

uid_t ProcessInfo::euid()
{
    return ::geteuid();
}

Timestamp ProcessInfo::startTime()
{
    return g_clockTicks;
}

int ProcessInfo::clockTicksPerSecond()
{
    return g_clockTicks;
}

int ProcessInfo::pageSize()
{
    return g_pageSize;
}

bool ProcessInfo::isDebugBuild()
{
#ifdef NDBUG
    return false;
#else
    return true;
}

string ProcessInfo::hostname()
{
    char buf[256];
    if (::gethostname(buf, sizeof(buf)) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    } else {
        return "unknownhost";
    }
}

string ProcessInfo::procname(const string& stat)
{
    StringPiece name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');

    if (lp != string::npos && rp != string::npos && lp < rp) {
        name.set(stat.data() + lp + 1, static_cast<int>(rp - lp - 1));
    }
    return name;
}

string ProcessInfo::procStatus()
{
    string result;
    FileUtil::readFile("/proc/self/status", 65535, &result);
    return result;
}