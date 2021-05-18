#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
  std::unique_lock<std::mutex> unique_lock_mtx(_mtx);
  _cv.wait(unique_lock_mtx, [this] { return !_queue.empty(); });

  T msg = std::move(_queue.back());
  _queue.clear();

  return msg;
}

template <typename T>
void MessageQueue<T>::send(T&& msg)
{
  std::lock_guard<std::mutex> lg_mutex(_mtx);
  _queue.push_back(std::move(msg));
  _cv.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
  _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
  while (true)
  {
    if (TrafficLightPhase::green == _message_que.receive())
    {
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
  return _currentPhase;
}

void TrafficLight::simulate()
{
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<double> distr(4.0, 6.0);
  auto random_number = distr(eng);
  auto start = std::chrono::high_resolution_clock::now();

  while (true)
  {
    std::chrono::duration<double> elapsed_time = std::chrono::high_resolution_clock::now() - start;

    if (random_number <= elapsed_time.count())
    {
      _currentPhase = (_currentPhase == TrafficLightPhase::green) ? TrafficLightPhase::red : TrafficLightPhase::green;
      TrafficLightPhase current_phase = _currentPhase;
      _message_que.send(std::move(current_phase));
      start = std::chrono::high_resolution_clock::now();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
