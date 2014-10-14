// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include <muduo/base/Mutex.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Channel.h>

namespace muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

///
/// A best efforts timer queue.
/// No guarantee that the callback will be on time.
///
class TimerQueue : boost::noncopyable
{
 public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  ///
  /// Schedules the callback to be run at given time,
  /// repeats if @c interval > 0.0.
  ///
  /// Must be thread safe. Usually be called from other threads.
  //一定是线程安全的，可以跨线程调用。通常情况下被其它线程调用
  TimerId addTimer(const TimerCallback& cb,
                   Timestamp when,
                   double interval);

  void cancel(TimerId timerId);

 private:

  // FIXME: use unique_ptr<Timer> instead of raw pointers.
  //unique_ptr是c++ 11标准的一个独享所有权的智能指针
  //无法得到指向同一对象的两个unique_ptr指针
  //但可以进行移动构造与移动赋值，即所有权可以移动到另一个对象(而非拷贝构造)
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;			//按时间排序(与ActiveTimerSet保存的相同东西)
  typedef std::pair<Timer*, int64_t> ActiveTimer;	//timer* 地址int64_t序号
  typedef std::set<ActiveTimer> ActiveTimerSet;//按地址排序

//以下成员函数只可能在其所属的I/O线程中调用，因而不必加锁。
//服务器性能杀手之一是锁竞争，所以要尽可能少用锁
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  // called when timerfd alarms
  void handleRead();
  // move out all expired timers
  //返回超时的定时器列表
  std::vector<Entry> getExpired(Timestamp now);
  //对超时的定时器重置(针对重复定时器)
  void reset(const std::vector<Entry>& expired, Timestamp now);

//插入定时器
  bool insert(Timer* timer);

  EventLoop* loop_;			//所属的EventLoop
  const int timerfd_;		//定时器描述符
  Channel timerfdChannel_;	//定时器通道	定时器返回会调用handleRead  TimerQueue与Channel是组合关系
  // Timer list sorted by expiration
  TimerList timers_;		//timers_是按到期时间排序

  // for cancel()
  //timers_与activeTimers_保存的是相同的数据
  //timers_是按到期时间排序，activeTimers_是按对象地址排序
  ActiveTimerSet activeTimers_;	
  bool callingExpiredTimers_; /* atomic *///是否处于处理超时定时器中
  ActiveTimerSet cancelingTimers_;//保存的是被取消的定时器
};

}
}
#endif  // MUDUO_NET_TIMERQUEUE_H
