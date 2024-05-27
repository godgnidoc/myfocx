#ifndef __MEGREZ_TASK_H__
#define __MEGREZ_TASK_H__

#include <functional>

#include "megrez/lock_free_queue.h"

namespace megrez {

extern MSLockFreeQueue<std::function<void()>> task_queue;

}  // namespace megrez

#endif