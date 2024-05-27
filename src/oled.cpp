#include "megrez/oled.h"

namespace megrez {
OLED::OLED(I2C* i2c) : i2c_(i2c) {}

void OLED::Init(void) {
  HAL_Delay(100);

  static uint8_t cmd[] = {
      0x00,  // Continuation Commands
      0xAE,  // display off
      0x20,  // Set Memory Addressing Mode
      0x10,  // 00,Horizontal Addressing Mode;01,Vertical Addressing
             // Mode;10,Page Addressing Mode (RESET);11,Invalid
      0xb0,  // Set Page Start Address for Page Addressing Mode,0-7
      0xc8,  // Set COM Output Scan Direction
      0x00,  //---set low column address
      0x10,  //---set high column address
      0x40,  //--set start line address
      0x81,  //--set contrast control register
      0xff,  // 亮度调节 0x00~0xff
      0xa1,  //--set segment re-map 0 to 127
      0xa6,  //--set normal display
      0xa8,  //--set multiplex ratio(1 to 64)
      0x3F,  //
      0xa4,  // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
      0xd3,  //-set display offset
      0x00,  //-not offset
      0xd5,  //--set display clock divide ratio/oscillator frequency
      0xf0,  //--set divide ratio
      0xd9,  //--set pre-charge period
      0x22,  //
      0xda,  //--set com pins hardware configuration
      0x12,
      0xdb,  //--set vcomh
      0x20,  // 0x20,0.77xVcc
      0x8d,  //--set DC-DC enable
      0x14,  //
      0xaf,  //--turn on oled panel
  };

  i2c_->SendTo(kAddr, Buffer::Wrap(cmd, sizeof(cmd)));
}

void OLED::SetPos(uint8_t x, uint8_t y) {
  x *= 6;
  auto cmd = new uint8_t[4]{0x00, (uint8_t)(0xb0 | (y & 0x07)),
                            (uint8_t)(x & 0x0f), (uint8_t)(0x10 | (x >> 4))};
  auto buffer = Buffer::Create([cmd] { delete[] cmd; }, cmd, 4);
  i2c_->SendTo(kAddr, buffer);
}

void OLED::ShowStr(uint8_t x, uint8_t y, char const* str) {
  SetPos(x, y);
  for (int i = 0; str[i] != '\0'; i++) {
    auto dat = new uint8_t[7]{0x40};
    auto buffer = Buffer::Create([dat] { delete[] dat; }, dat, 7);

    auto base = (str[i] - 32) * 6;
    for (int j = 0; j < 6; j++) dat[j + 1] = F6x8[base + j];

    i2c_->SendTo(kAddr, buffer);
  }
}

void OLED::Fill(uint8_t data) {
  static uint8_t cmd[] = {0x00, 0xb0, 0x10, 0x00};
  auto cmd_buf = Buffer::Wrap(cmd, sizeof(cmd));
  i2c_->SendTo(kAddr, cmd_buf);

  auto dat = new uint8_t[129]{0x40};
  for (auto i = 1; i < 129; i++) dat[i] = data;
  auto dat_buf = Buffer::Create([dat] { delete[] dat; }, dat, 129);

  for (auto i = 0; i < 8; i++) i2c_->SendTo(kAddr, dat_buf);
}

void OLED::Clear(void) { Fill(0x00); }

void OLED::ON() {
  static uint8_t cmd[] = {
      0x00,
      0x8d,  // 设置电荷泵
      0x14,  // 开启电荷泵
      0xaf,  // OLED唤醒
  };

  i2c_->SendTo(kAddr, Buffer::Wrap(cmd, sizeof(cmd)));
}

void OLED::OFF() {
  static uint8_t cmd[] = {
      0x00,
      0x8d,  // 设置电荷泵
      0x10,  // 关闭电荷泵
      0xae,  // OLED休眠
  };

  i2c_->SendTo(kAddr, Buffer::Wrap(cmd, sizeof(cmd)));
}

}  // namespace megrez