#include "megrez/i2c.h"

#include <atomic>
#include <mutex>

#include "megrez/task.h"

extern "C" void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  auto inst = megrez::I2C::Inst(hi2c, false);

  inst->TaskDone();
}

extern "C" void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  auto inst = megrez::I2C::Inst(hi2c, false);

  inst->TaskDone();
}

namespace megrez {

std::map<I2C_HandleTypeDef *, I2C *> I2C::insts_;

I2C *I2C::Inst(I2C_HandleTypeDef *hi2c, bool create) {
  auto inst = insts_.find(hi2c);
  if (inst != insts_.end()) return inst->second;
  if (!create) return nullptr;

  inst = insts_.emplace(hi2c, new I2C{hi2c}).first;
  return inst->second;
}

void I2C::SendTo(uint16_t addr, BufferRef buffer, std::function<void()> done) {
  tasks_.push_back(
      i2c::Task{.dir = i2c::kTx, .addr = addr, .done = done, .buffer = buffer});

  Continues();
}

void I2C::ReceiveFrom(uint16_t addr, BufferRef buffer,
                      std::function<void()> done) {
  tasks_.push_back(
      i2c::Task{.dir = i2c::kRx, .addr = addr, .done = done, .buffer = buffer});

  Continues();
}

I2C::I2C(I2C_HandleTypeDef *hi2c) : handle_{hi2c}, bus_busy_{false} {}

void I2C::TaskDone() {
  i2c::Task task;
  tasks_.pop_front(task);
  if (task.done) task_queue.push_back(task.done);

  auto pnext = tasks_.peek_front();
  if (!pnext) {
    bus_busy_.store(false);
  } else {
    switch (pnext->dir) {
      case i2c::kTx: {
        HAL_I2C_Master_Transmit_DMA(
            handle_, pnext->addr, pnext->buffer->writable, pnext->buffer->size);
      } break;
      case i2c::kRx: {
        HAL_I2C_Master_Receive_DMA(
            handle_, pnext->addr, pnext->buffer->writable, pnext->buffer->size);
      } break;
    }
  }
}

void I2C::Continues() {
  bool expected = false;
  if (!bus_busy_.compare_exchange_strong(expected, true)) return;

  auto ptask = tasks_.peek_front();
  switch (ptask->dir) {
    case i2c::kTx: {
      HAL_I2C_Master_Transmit_DMA(handle_, ptask->addr, ptask->buffer->writable,
                                  ptask->buffer->size);
    } break;
    case i2c::kRx: {
      HAL_I2C_Master_Receive_DMA(handle_, ptask->addr, ptask->buffer->writable,
                                 ptask->buffer->size);
    } break;
  }
}

}  // namespace megrez