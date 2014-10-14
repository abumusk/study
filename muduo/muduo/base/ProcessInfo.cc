// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include <muduo/base/ProcessInfo.h>
#include <muduo/base/FileUtil.h>

#include <algorithm>

#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h> // snprintf
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

namespace muduo
{
namespace detail
{
__thread int t_numOpenedFiles = 0;
int fdDirFilter(const struct dirent* d)
{
  if (::isdigit(d->d_name[0]))	//判定d->name是否是0-9字符正确返回非零，错误返回0
  {
    ++t_numOpenedFiles;
  }
  return 0;
}

__thread std::vector<pid_t>* t_pids = NULL;
int taskDirFilter(const struct dirent* d)
{
  if (::isdigit(d->d_name[0]))
  {
    t_pids->push_back(atoi(d->d_name));
  }
  return 0;
}

int scanDir(const char *dirpath, int (*filter)(const struct dirent *))
{
  struct dirent** namelist = NULL;
  int result = ::scandir(dirpath, &namelist, filter, alphasort);
  assert(namelist == NULL);
  return result;
}

Timestamp g_startTime = Timestamp::now();	//获取运行时间
}
}

using namespace muduo;
using namespace muduo::detail;

pid_t ProcessInfo::pid()
{
  return ::getpid();	//获取当前进程ID	
}

string ProcessInfo::pidString()
{
  char buf[32];
  snprintf(buf, sizeof buf, "%d", pid());	//把进程ID格式化成字符串
  return buf;
}

//geteuid()：返回有效用户的ID。
//getuid（）：返回实际用户的ID。 
//有效用户ID（EUID）是你最初执行程序时所用的ID   
//表示该ID是程序的所有者   
//真实用户ID（UID）是程序执行过程中采用的ID   
//该ID表明当前运行位置程序的执行者   
//举个例子   
//程序myprogram的所有者为501/anna   
//以501运行该程序此时UID和EUID都是501   
//但是由于中间要访问某些系统资源   
//需要使用root身份   
//此时UID为0而EUID仍是501
uid_t ProcessInfo::uid()
{
  return ::getuid();	//当时用户ID
}

//获取当前用户名称
string ProcessInfo::username()
{
  struct passwd pwd;
  struct passwd* result = NULL;
  char buf[8192];
  const char* name = "unknownuser";

//该getpwuid_r（）函数将更新passwd结构指向PWD和存储指向该结构的位置，
//由结果指出。 其结构应包括从用户数据 ??库具有匹配的uid条目。 存储由该
//结构引用的是从设置有缓冲参数的存储器，这是在大小BUFSIZE字节分配。 
//需要这个缓冲区的最大尺寸可以与{_SC_GETPW_R_SIZE_MAX}确定的sysconf（）
//的参数。 NULL指针应退还在该位置所指向的结果出错，或者如果找不到要求的项目
//uid是输入参数-这是您要查找的用户的UID。 剩下的基本上是输出参数：
//指向的结构由pwbuf将充满的密码信息，并且该指针所指向的pwbufp将被设置为的值pwbuf如果呼叫成功（和NULL如果不是）
// 在buf和buflen对参数指定将要用于存储字符串所指向的成员用户提供的缓冲struct passwd返回的结构。

  getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
  if (result)
  {
    name = pwd.pw_name;
  }
  return name;
}

uid_t ProcessInfo::euid()
{
  return ::geteuid();	//获取当前有效用户ID
}

Timestamp ProcessInfo::startTime()
{
  return g_startTime;	//获取运行时间
}

string ProcessInfo::hostname()
{
  char buf[64] = "unknownhost";
  buf[sizeof(buf)-1] = '\0';
  ::gethostname(buf, sizeof buf);	//获取主机名
  return buf;
}

string ProcessInfo::procStatus()
{
  string result;
  FileUtil::readFile("/proc/self/status", 65536, &result);	//读取/proc/self/status这个特殊文件

  return result;
}

int ProcessInfo::openedFiles()
{
  t_numOpenedFiles = 0;
  scanDir("/proc/self/fd", fdDirFilter);
  return t_numOpenedFiles;
}

int ProcessInfo::maxOpenFiles()
{
  struct rlimit rl;
  if (::getrlimit(RLIMIT_NOFILE, &rl))
  {
    return openedFiles();
  }
  else
  {
    return static_cast<int>(rl.rlim_cur);
  }
}

int ProcessInfo::numThreads()
{
  int result = 0;
  string status = procStatus();
  size_t pos = status.find("Threads:");
  if (pos != string::npos)
  {
    result = ::atoi(status.c_str() + pos + 8);
  }
  return result;
}

std::vector<pid_t> ProcessInfo::threads()
{
  std::vector<pid_t> result;
  t_pids = &result;
  scanDir("/proc/self/task", taskDirFilter);
  t_pids = NULL;
  std::sort(result.begin(), result.end());
  return result;
}

