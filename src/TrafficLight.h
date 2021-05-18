#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
  T receive();
  void send(T&& msg);

private:
  std::deque<T> _queue;
  std::condition_variable _cv;
  std::mutex _mtx;
};

class TrafficLight : TrafficObject
{
public:
  TrafficLight();
  void waitForGreen();
  void simulate() override;

  enum TrafficLightPhase
  {
    red,
    green
  };
  TrafficLightPhase getCurrentPhase();

private:
  TrafficLightPhase _currentPhase;
  void cycleThroughPhases();

  MessageQueue<TrafficLightPhase> _message_que;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif
