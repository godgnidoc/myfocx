#include "megrez/AS5600.h"

namespace megrez {

AS5600::AS5600(I2C* i2c) : i2c_(i2c) {}

void AS5600::GetAngle(std::function<void(float)> cb) {
  static uint8_t reg_addr = 0x0E;
  i2c_->SendTo(kAddr, Buffer::Wrap(&reg_addr, 1), [this, cb] {
    auto buffer = Buffer::Create(2);
    i2c_->ReceiveFrom(kAddr, buffer, [buffer, cb] {
      uint16_t angle = ((buffer->data[0] & 0x0f) << 8) | buffer->data[1];

      cb(angle / 4096.0);
    });
  });
}

}  // namespace megrez