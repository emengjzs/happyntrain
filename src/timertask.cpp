#include "timertask.h"

#include <algorithm>

#include "util/logging.h"
#include "util/time.h"

using namespace std;
using namespace happyntrain::concurrent;
using namespace happyntrain::timer;
using namespace happyntrain::time;

namespace happyntrain {
namespace timer {

// --------------------
class TimerTaskManager;
// -----------------------

TaskId TimerTaskManager::AddTask(concurrent::Runnable&& task,
                                 uint64_t delay_ms) {
  uint64_t deadline = delay_ms + time::current_time<ms>();
  DEBUG("now(%ld) delay(%lu) deadline(%lu)", time::current_time<ms>(), delay_ms,
        deadline);
  TaskId taskId(taskSequencer_.GetNextId(), deadline);
  taskQueue_.insert({taskId, std::move(task)});
  DEBUG("+ TimerTask id(%ld) delay(%lu ms)", taskId.id, delay_ms);
  return taskId;
}

int64_t TimerTaskManager::GetNextTimeout() {
  if (taskQueue_.empty()) {
    return 1 << 30;
  } else {
    const TaskId& top = taskQueue_.begin()->first;
    DEBUG("deadline(%ld ms), now(%ld ms)", top.deadline, time::now<ms>());
    return max(int64_t(0), (int64_t)top.deadline - (int64_t)time::now<ms>());
  }
}

void TimerTaskManager::ExecuteTimeoutTasks() {
  int64_t current_ms = time::now<ms>();
  while (!taskQueue_.empty()) {
    TaskQueueIterator top = taskQueue_.begin();
    if (current_ms < (top->first).deadline) break;
    Runnable task(move(top->second));
    taskQueue_.erase(top);
    task();
  }
}

// namespace happyntrain
}
// namespace timer
}