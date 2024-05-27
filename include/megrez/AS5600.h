#ifndef __MEGREZ_AS5600_H__
#define __MEGREZ_AS5600_H__

#include <cstdint>

#include "megrez/i2c.h"

namespace megrez {

class AS5600 {
 public:
  static constexpr uint16_t kAddr = 0x36 << 1;

 public:
  AS5600(I2C* i2c);

  void GetAngle(std::function<void(float)> cb);

 private:
  I2C* i2c_;
};

}  // namespace megrez

#endif