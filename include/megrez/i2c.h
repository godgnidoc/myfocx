#ifndef __MEGREZ_I2C_H__
#define __MEGREZ_I2C_H__

#include <stm32f1xx_hal.h>

#include <functional>
#include <map>

#include "megrez/buffer.h"
#include "megrez/lock_free_queue.h"

extern "C" void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
extern "C" void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);

namespace megrez {

namespace i2c {
enum TaskDir {
  kTx = 0,
  kRx = 1,
};

struct Task {
  TaskDir dir;
  uint16_t addr;
  std::function<void()> done;
  BufferRef buffer;
};
}  // namespace i2c

class I2C {
  friend void ::HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
  friend void ::HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);

 public:
  /**
   * 获取 I2C 实例
   *
   * 此函数没有中断保护。无比确保在初始化阶段获取全部需要的 I2C 实例
   *
   * @param hi2c I2C_HandleTypeDef 实例
   * @param create 不存在时是否创建
   * @return I2C 实例
   */
  static I2C *Inst(I2C_HandleTypeDef *hi2c, bool create = true);

  /**
   * 发送数据到指定地址
   *
   * 此函数具备中断保护，可以在中断中调用
   * 多个发送任务会按照调用顺序依次执行
   *
   * @param addr 目标地址
   * @param buffer 数据缓冲区，由于底层接口限制，必须可写
   * @param done 发送完成回调
   */
  void SendTo(uint16_t addr, BufferRef buffer,
              std::function<void()> done = nullptr);

  /**
   * 从指定地址接收数据
   *
   * 此函数具备中断保护，可以在中断中调用
   * 多个接收任务会按照调用顺序依次执行
   *
   * @param addr 源地址
   * @param buffer 数据缓冲区
   * @param done 接收完成回调
   */
  void ReceiveFrom(uint16_t addr, BufferRef buffer,
                   std::function<void()> done = nullptr);

 private:
  I2C(I2C_HandleTypeDef *hi2c);
  void TaskDone();
  void Continues();

 private:
  I2C_HandleTypeDef *handle_;

  MSLockFreeQueue<i2c::Task> tasks_;
  std::atomic_bool bus_busy_;

  static std::map<I2C_HandleTypeDef *, I2C *> insts_;
};

}  // namespace megrez

#endif