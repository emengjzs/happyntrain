#pragma once

#include <map>
#include <string>

#include "util/concurrent.h"
#include "util/core.h"

namespace happyntrain {
namespace timer {

struct TaskId {
  int64_t id;
  int64_t deadline;
  bool valid;
  std::string name;

  TaskId() : id(0), deadline(0), valid(false), name("") {}
  TaskId(uint64_t id_, uint32_t deadline_)
      : id(id_), deadline(deadline_), valid(true) {}
  bool isValid() const { return valid; }
};

struct TaskComparator {
  inline bool operator()(const TaskId& lhs, const TaskId& rhs) {
    return lhs.deadline < rhs.deadline ||
           ((!(rhs.deadline < lhs.deadline)) && lhs.id < rhs.id);
  }
};

class TimerTaskManager : NoCopy {
  SequenceCreator<uint64_t> taskSequencer_;
  std::map<TaskId, Runnable, TaskComparator> taskQueue_;
  using TaskQueueIterator =
      std::map<TaskId, Runnable, TaskComparator>::iterator;

 public:
  TimerTaskManager() : taskSequencer_(1), taskQueue_() {}
  ~TimerTaskManager() {}

  TaskId AddTask(Runnable&& task, uint64_t delay);

  int64_t GetNextTimeout();

  void ExecuteTimeoutTasks();
};

}  // end timer
// end happyntrain
}
