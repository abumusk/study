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
  if (::isdigit(d->d_name[0]))	//�ж�d->name�Ƿ���0-9�ַ���ȷ���ط��㣬���󷵻�0
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

Timestamp g_startTime = Timestamp::now();	//��ȡ����ʱ��
}
}

using namespace muduo;
using namespace muduo::detail;

pid_t ProcessInfo::pid()
{
  return ::getpid();	//��ȡ��ǰ����ID	
}

string ProcessInfo::pidString()
{
  char buf[32];
  snprintf(buf, sizeof buf, "%d", pid());	//�ѽ���ID��ʽ�����ַ���
  return buf;
}

//geteuid()��������Ч�û���ID��
//getuid����������ʵ���û���ID�� 
//��Ч�û�ID��EUID���������ִ�г���ʱ���õ�ID   
//��ʾ��ID�ǳ����������   
//��ʵ�û�ID��UID���ǳ���ִ�й����в��õ�ID   
//��ID������ǰ����λ�ó����ִ����   
//�ٸ�����   
//����myprogram��������Ϊ501/anna   
//��501���иó����ʱUID��EUID����501   
//���������м�Ҫ����ĳЩϵͳ��Դ   
//��Ҫʹ��root���   
//��ʱUIDΪ0��EUID����501
uid_t ProcessInfo::uid()
{
  return ::getuid();	//��ʱ�û�ID
}

//��ȡ��ǰ�û�����
string ProcessInfo::username()
{
  struct passwd pwd;
  struct passwd* result = NULL;
  char buf[8192];
  const char* name = "unknownuser";

//��getpwuid_r��������������passwd�ṹָ��PWD�ʹ洢ָ��ýṹ��λ�ã�
//�ɽ��ָ���� ��ṹӦ�������û����� ??�����ƥ���uid��Ŀ�� �洢�ɸ�
//�ṹ���õ��Ǵ������л�������Ĵ洢���������ڴ�СBUFSIZE�ֽڷ��䡣 
//��Ҫ��������������ߴ������{_SC_GETPW_R_SIZE_MAX}ȷ����sysconf����
//�Ĳ����� NULLָ��Ӧ�˻��ڸ�λ����ָ��Ľ��������������Ҳ���Ҫ�����Ŀ
//uid���������-������Ҫ���ҵ��û���UID�� ʣ�µĻ����������������
//ָ��Ľṹ��pwbuf��������������Ϣ�����Ҹ�ָ����ָ���pwbufp��������Ϊ��ֵpwbuf������гɹ�����NULL������ǣ�
// ��buf��buflen�Բ���ָ����Ҫ���ڴ洢�ַ�����ָ��ĳ�Ա�û��ṩ�Ļ���struct passwd���صĽṹ��

  getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
  if (result)
  {
    name = pwd.pw_name;
  }
  return name;
}

uid_t ProcessInfo::euid()
{
  return ::geteuid();	//��ȡ��ǰ��Ч�û�ID
}

Timestamp ProcessInfo::startTime()
{
  return g_startTime;	//��ȡ����ʱ��
}

string ProcessInfo::hostname()
{
  char buf[64] = "unknownhost";
  buf[sizeof(buf)-1] = '\0';
  ::gethostname(buf, sizeof buf);	//��ȡ������
  return buf;
}

string ProcessInfo::procStatus()
{
  string result;
  FileUtil::readFile("/proc/self/status", 65536, &result);	//��ȡ/proc/self/status��������ļ�

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

