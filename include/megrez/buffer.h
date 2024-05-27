#ifndef __MEGREZ_BUFFER_H__
#define __MEGREZ_BUFFER_H__

#include <stddef.h>
#include <stdint.h>

#include <memory>

namespace megrez {

struct Buffer;
using BufferRef = std::shared_ptr<Buffer>;

/**
 * 缓冲区抽象
 */
struct Buffer {
  /**
   * 缓冲区数据指针
   */
  uint8_t const* const data;

  /**
   * 可写数据指针，应当与 data 指向同一块内存
   * 若缓冲不可写则为 nullptr
   *
   * @note 保留此字段是因为使用从 const_cast 转换的指针可能会导致未定义行为
   */
  uint8_t* const writable;

  /**
   * 缓冲区大小
   */
  size_t const size;

  /**
   * 创建只读缓冲区并获取所有权，需要指定释放器
   *
   * 释放器可以是一个可调用对象，当Buffer被析构时，释放器会被调用以释放资源
   * 也可以是一个存续对象，当Buffer被析构时，存续对象会被析构
   *
   * @param deleter 释放器
   * @param data 缓冲区数据
   * @param size 缓冲区大小
   */
  // template <typename D>
  // static BufferRef Create(D&& deleter, void const* data, size_t size) {
  //   return BufferRef(
  //       new Buffer{
  //           .data{static_cast<uint8_t const*>(data)},
  //           .writable{nullptr},
  //           .size{size},
  //       },
  //       [deleter](Buffer* ptr) {
  //         delete ptr;
  //         if constexpr (std::is_invocable_v<D>) {
  //           deleter();
  //         }
  //       });
  // }

  /**
   * 创建可读写缓冲区并获取所有权，需要指定释放器
   *
   * 释放器可以是一个可调用对象，当Buffer被析构时，释放器会被调用以释放资源
   * 也可以是一个存续对象，当Buffer被析构时，存续对象会被析构
   *
   * @param deleter 释放器或存续对象
   * @param data 缓冲区数据
   * @param size 缓冲区大小
   */
  template <typename D>
  static BufferRef Create(D&& deleter, void* data, size_t size) {
    return BufferRef(
        new Buffer{
            static_cast<uint8_t const*>(data),
            static_cast<uint8_t*>(data),
            size,
        },
        [deleter](Buffer* ptr) {
          delete ptr;
          if constexpr (std::is_invocable_v<D>) {
            deleter();
          }
        });
  }

  /**
   * 创建一个具有所有权的缓冲区
   *
   * @param size 缓冲区大小
   * @return 缓冲区
   */
  static BufferRef Create(size_t size) {
    auto p = new uint8_t[size];
    auto d = [p]() { delete[] p; };
    return Create(d, p, size);
  }

  /**
   * 创建一个不具有所有权的只读缓冲区
   *
   * @param data 缓冲区数据
   * @param size 缓冲区大小
   */
  // static BufferRef Wrap(void const* data, size_t size) {
  //   return BufferRef(new Buffer{
  //       .data{static_cast<uint8_t const*>(data)},
  //       .writable{nullptr},
  //       .size{size},
  //   });
  // }

  /**
   * 创建一个不具有所有权的可读写缓冲区
   *
   * @param data 缓冲区数据
   * @param size 缓冲区大小
   */
  static BufferRef Wrap(void* data, size_t size) {
    return BufferRef(new Buffer{
        static_cast<uint8_t const*>(data),
        static_cast<uint8_t*>(data),
        size,
    });
  }

  /**
   * 创建一个空缓冲区
   */
  // static BufferRef Empty() {
  //   return BufferRef(new Buffer{
  //       .data{nullptr},
  //       .writable{nullptr},
  //       .size{0},
  //   });
  // }
};

}  // namespace megrez

#endif