#include "megrez/task.h"

namespace megrez {

MSLockFreeQueue<std::function<void()>> task_queue;

}