#ifndef __MEGREZ_MOTOR_H__
#define __MEGREZ_MOTOR_H__

#include <cstdint>

namespace megrez {

class Motor {
 public:
  /**
   * 构造函数
   *
   * @param pole_pairs 极对数
   * @param period PWM周期（脉冲数）
   * @param a 电机a相PWM比较器地址
   * @param b 电机b相PWM比较器地址
   * @param c 电机c相PWM比较器地址
   */
  Motor(int pole_pairs, uint32_t period, volatile uint32_t* a,
        volatile uint32_t* b, volatile uint32_t* c);

  /**
   * 设置电机驱动电压
   *
   * @param voltage 电机驱动电压 0~1
   * @param theta 电机转子位置（机械角度）
   */
  void SetVoltage(float voltage, float theta);

 private:
  int const pole_pairs_;
  float const half_period_;
  volatile uint32_t* const a_ = 0;
  volatile uint32_t* const b_ = 0;
  volatile uint32_t* const c_ = 0;
};

}  // namespace megrez

#endif