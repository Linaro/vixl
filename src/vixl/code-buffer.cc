// Copyright 2014, ARM Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vixl/code-buffer.h"
#include "vixl/utils.h"

namespace vixl {


CodeBuffer::CodeBuffer(size_t capacity) : managed_(true), capacity_(capacity) {
  VIXL_CHECK(capacity_ != 0);
  buffer_ = reinterpret_cast<byte*>(malloc(capacity_));
  VIXL_CHECK(buffer_ != NULL);
  // A64 instructions must be word aligned, we assert the default allocator
  // always returns word align memory.
  VIXL_ASSERT(IsWordAligned(buffer_));

  cursor_ = buffer_;
  dirty_ = false;
}


CodeBuffer::CodeBuffer(void* buffer, size_t capacity)
    : buffer_(reinterpret_cast<byte*>(buffer)),
      managed_(false),
      cursor_(reinterpret_cast<byte*>(buffer)),
      dirty_(false),
      capacity_(capacity) {
  VIXL_ASSERT(buffer_ != NULL);
}


CodeBuffer::~CodeBuffer() {
  VIXL_ASSERT(!IsDirty());
  if (managed_) {
    free(buffer_);
  }
}


void CodeBuffer::EmitString(const char* string) {
  VIXL_ASSERT(RemainingBytes() > strlen(string));
  char* dst = reinterpret_cast<char*>(cursor_);
  dirty_ = true;
  char* null_char = stpcpy(dst, string);
  cursor_ = reinterpret_cast<byte*>(null_char) + 1;
}


void CodeBuffer::Align() {
  byte* end = AlignUp(cursor_, 4);
  VIXL_ASSERT(end >= cursor_);
  const size_t padding_size = end - cursor_;
  VIXL_ASSERT(RemainingBytes() >= padding_size);
  VIXL_ASSERT(padding_size <= 4);
  const byte padding[] = { 0, 0, 0, 0};
  dirty_ = true;
  memcpy(cursor_, padding, padding_size);
  cursor_ = end;
}


void CodeBuffer::Reset() {
#ifdef VIXL_DEBUG
  if (managed_) {
    // TODO(all): Consider allowing for custom default values, e.g. HLT.
    memset(buffer_, 0, capacity_);
  }
#endif
  cursor_ = buffer_;
  SetClean();
}


void CodeBuffer::Grow(size_t new_capacity) {
  VIXL_ASSERT(managed_);
  VIXL_ASSERT(new_capacity > capacity_);
  size_t size = CursorOffset();
  buffer_ = static_cast<byte*>(realloc(buffer_, new_capacity));
  VIXL_CHECK(buffer_ != NULL);

  cursor_ = buffer_ + size;
  capacity_ = new_capacity;
}


}  // namespace vixl
