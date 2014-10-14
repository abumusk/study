// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#define __STDC_LIMIT_MACROS
#include <muduo/net/TimerQueue.h>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Timer.h>
#include <muduo/net/TimerId.h>

#include <boost/bind.hpp>

#include <sys/timerfd.h>

namespace muduo
{
namespace net
{
namespace detail
{

int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

//���㳬ʱʱ���뵱ǰʱ���ʱ���
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

//�����ʱ��������һֱ����
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

//���ö�ʱ���ĳ�ʱʱ��
void resetTimerfd(int timerfd, Timestamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret)
  {
    LOG_SYSERR << "timerfd_settime()";
  }
}

}
}
}

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_(),
    callingExpiredTimers_(false)
{
  timerfdChannel_.setReadCallback(
      boost::bind(&TimerQueue::handleRead, this));
  // we are always reading the timerfd, we disarm it with timerfd_settime.
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  // do not remove channel, since we're in EventLoop::dtor();
  for (TimerList::iterator it = timers_.begin();
      it != timers_.end(); ++it)
  {
    delete it->second;
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(
      boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
  loop_->runInLoop(
      boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  //����һ����ʱ�����п��ܻ�ʹ�����絽�ڵĶ�ʱ�������ı�
  bool earliestChanged = insert(timer);

  if (earliestChanged)
  {
  	//���ö�ʱ���ĳ�ʱʱ��(timerfd_settime)  ����ʱ�䣨timer->expiration()��
    resetTimerfd(timerfd_, timer->expiration());
  }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  //���Ҹö�ʱ��
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  if (it != activeTimers_.end())
  {
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first; // FIXME: no delete please,�������unique_ptr������Ͳ���Ҫ�ֶ�ɾ����
    activeTimers_.erase(it);
  }
  else if (callingExpiredTimers_)
  {
  //�Ѿ����ڣ��������ڵ��ûص������Ķ�ʱ��
    cancelingTimers_.insert(timer);//��ִ����ص����������reset�Ϳ���ֱ��
  }
  assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);	//������¼�������һֱ����

//��ȡ��ʱ��֮ǰ���еĶ�ʱ���б�(����ʱ��ʱ���б�)
  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  // safe to callback outside critical section
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
  //����ص���ʱ��������
    it->second->run();
  }
  callingExpiredTimers_ = false;

//����һ���Զ�ʱ������Ҫ����
  reset(expired, now);
}

//rvo	���´������һ���¶���������rvo�Ż�û���������� 
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  assert(timers_.size() == activeTimers_.size());
  std::vector<Entry> expired;
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));// reinterpret_cast<Timer*>(UINTPTR_MAX)���ĵ�ַ
  //���ص�һ��δ���ڵ�Timer�ĵ�����
  //lower_bound�ĺ����Ƿ��ص�һֵ>=sentry��Ԫ��iterator
  //��*end >= sentry, �Ӷ�end->first > now  ��Ϊend->first=now��ôend->second��һ��С��UINTPTR_MAX���ֵ
  //��ô*end >=sentry�Ͳ������
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first);//Ҫ��û���ҵ�Ҫ��nowһ�����ҵ���firstС
  //�����ڵĶ�ʱ�����뵽expired��
  std::copy(timers_.begin(), end, back_inserter(expired));//back_inserter������������������expired���һ��λ��
  //��timers_���Ƴ����ڵĶ�ʱ��
  timers_.erase(timers_.begin(), end);

  //��activeTimers_���Ƴ����ڵĶ�ʱ��
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
	//������ظ��Ķ�ʱ��������δȡ����ʱ�����������ö�ʱ��
    if (it->second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      it->second->restart(now);
      insert(it->second);  
    }
    else
    {
    //һ���Զ�ʱ�������ѱ�ȡ���Ķ�ʱ���ǲ������õģ����ɾ���ö�ʱ��
      // FIXME move to a free list
      delete it->second; // FIXME: no delete please
    }
  }

  if (!timers_.empty())
  {
  //��ȡ���絽�ڵĶ�ʱ����ʱʱ��
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
  //���ö�ʱ���ĳ�ʱʱ��(timerfd_settime)    
    resetTimerfd(timerfd_, nextExpire);
  }
}

bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  //���絽��ʱ���Ƿ�ı�
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  //���timers_Ϊ�ջ���whenС��timers_�е����絽��ʱ��
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
  	//���뵽timers_��
  	//set.insert���뷵��ֵ��pair boolֵ�����ж��Ƿ�ɹ�
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
  	//���뵽activetimers_��
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

