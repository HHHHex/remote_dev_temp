#pragma once

#include "rte_base/c/utils/buf.h"


constexpr size_t DEFAULT_BUFFER_SIZE = 1024;

namespace rte {

/**
 * @brief 
 * @technical preview
 */
class Buffer {
  public: 
    Buffer() {
      c_buf = RteBufCreate(nullptr);
      RteBufInit(c_buf, DEFAULT_BUFFER_SIZE, nullptr);
    }

    Buffer(size_t size) {
      c_buf = RteBufCreateWithCapacity(size, nullptr);
    }
    
    Buffer(const char* buf, size_t size) {
      c_buf = RteBufCreate(nullptr);
      RteBufInitFromBuffer(c_buf, static_cast<void*>(const_cast<char*>(buf)), size, nullptr);
    }

    Buffer(RteBuf* buf) {
      c_buf = RteBufCreate(nullptr);
      if (buf != nullptr) {
          RteBufInitFromBuffer(c_buf, buf->data, buf->size, nullptr);
      } 
    }

    Buffer(const Buffer& other) {
      c_buf = RteBufCreate(nullptr);
      if (other.c_buf != nullptr) {
        RteBufInitFromBuffer(c_buf, other.c_buf->data, other.c_buf->size, nullptr);
      }
    }

    Buffer(Buffer&& other) {
        c_buf = other.c_buf;
        other.c_buf = nullptr;
    }

    Buffer& operator=(const Buffer& other) {
        RteBufReset(c_buf, nullptr);

        if (other.c_buf) {
            c_buf = RteBufCreate(nullptr);
            RteBufInitFromBuffer(c_buf, other.c_buf->data, other.c_buf->size, nullptr);
        }

        return *this;
    }

    /**
     * @brief 
     * @technical preview
     * @return void* 
     */
    void* Data() const {
        if (c_buf != nullptr) {
            return c_buf->data;
        }

        return nullptr;
    }

    /**
     * @brief 
     * @technical preview
     * @return void* 
     */
    size_t Size() const {
        if (c_buf != nullptr) {
            return c_buf->size;
        }

        return 0;
    }

    ~Buffer() {
      RteBufDeinit(c_buf, nullptr);
      RteBufDestroy(c_buf, nullptr);
    }

    ::RteBuf* get_underlying_impl() const {
      return c_buf;
    }

  private:
    ::RteBuf* c_buf;
};

}
