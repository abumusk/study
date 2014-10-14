// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_NET_INSPECT_INSPECTOR_H
#define MUDUO_NET_INSPECT_INSPECTOR_H

#include <muduo/base/Mutex.h>
#include <muduo/net/http/HttpRequest.h>
#include <muduo/net/http/HttpServer.h>

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
namespace net
{

class ProcessInspector;

// A internal inspector of the running process, usually a singleton.
class Inspector : boost::noncopyable
{
 public:
  typedef std::vector<string> ArgList;
  typedef boost::function<string (HttpRequest::Method, const ArgList& args)> Callback;
  Inspector(EventLoop* loop,
            const InetAddress& httpAddr,
            const string& name);
  ~Inspector();

// ��add("proc", "pid", ProcessInspector::pid, "print pid");
//http://192.168.42.188:12345/proc/pid�������ͻ���Ӧ�ĵ���ProcessInspector::pid������
  void add(const string& module,
           const string& command,
           const Callback& cb,
           const string& help);

 private:
  typedef std::map<string, Callback> CommandList;	//key command, value Callback
  typedef std::map<string, string> HelpList;	//key command value help

  void start();
  void onRequest(const HttpRequest& req, HttpResponse* resp);

  HttpServer server_;
  boost::scoped_ptr<ProcessInspector> processInspector_;
  MutexLock mutex_;		//����Ļ�������ν�ģ�ֻ��һ���̲߳���
  std::map<string, CommandList> commands_;	//key proc  value CommandList
  std::map<string, HelpList> helps_;	//key proc value HelpList
};

}
}

#endif  // MUDUO_NET_INSPECT_INSPECTOR_H
