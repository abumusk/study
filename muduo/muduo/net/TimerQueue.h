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
  //һ�����̰߳�ȫ�ģ����Կ��̵߳��á�ͨ������±������̵߳���
  TimerId addTimer(const TimerCallback& cb,
                   Timestamp when,
                   double interval);

  void cancel(TimerId timerId);

 private:

  // FIXME: use unique_ptr<Timer> instead of raw pointers.
  //unique_ptr��c++ 11��׼��һ����������Ȩ������ָ��
  //�޷��õ�ָ��ͬһ���������unique_ptrָ��
  //�����Խ����ƶ��������ƶ���ֵ��������Ȩ�����ƶ�����һ������(���ǿ�������)
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;			//��ʱ������(��ActiveTimerSet�������ͬ����)
  typedef std::pair<Timer*, int64_t> ActiveTimer;	//timer* ��ַint64_t���
  typedef std::set<ActiveTimer> ActiveTimerSet;//����ַ����

//���³�Ա����ֻ��������������I/O�߳��е��ã�������ؼ�����
//����������ɱ��֮һ��������������Ҫ������������
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  // called when timerfd alarms
  void handleRead();
  // move out all expired timers
  //���س�ʱ�Ķ�ʱ���б�
  std::vector<Entry> getExpired(Timestamp now);
  //�Գ�ʱ�Ķ�ʱ������(����ظ���ʱ��)
  void reset(const std::vector<Entry>& expired, Timestamp now);

//���붨ʱ��
  bool insert(Timer* timer);

  EventLoop* loop_;			//������EventLoop
  const int timerfd_;		//��ʱ��������
  Channel timerfdChannel_;	//��ʱ��ͨ��	��ʱ�����ػ����handleRead  TimerQueue��Channel����Ϲ�ϵ
  // Timer list sorted by expiration
  TimerList timers_;		//timers_�ǰ�����ʱ������

  // for cancel()
  //timers_��activeTimers_���������ͬ������
  //timers_�ǰ�����ʱ������activeTimers_�ǰ������ַ����
  ActiveTimerSet activeTimers_;	
  bool callingExpiredTimers_; /* atomic *///�Ƿ��ڴ���ʱ��ʱ����
  ActiveTimerSet cancelingTimers_;//������Ǳ�ȡ���Ķ�ʱ��
};

}
}
#endif  // MUDUO_NET_TIMERQUEUE_H
