#ifndef __MEGREZ_OLED_H__
#define __MEGREZ_OLED_H__

#include <stdint.h>

#include "main.h"
#include "megrez/i2c.h"

namespace megrez {
class OLED {
 public:
  static constexpr uint16_t kAddr = 0x78;

 public:
  OLED(I2C* i2c);

  void Init();
  void Clear();
  void ON();
  void OFF();
  void ShowStr(uint8_t x, uint8_t y, char const* str);

 private:
  void Fill(uint8_t data);
  void SetPos(uint8_t x, uint8_t y);

 private:
  I2C* i2c_;
  static uint8_t const F6x8[92 * 6];
};
}  // namespace megrez

#endif