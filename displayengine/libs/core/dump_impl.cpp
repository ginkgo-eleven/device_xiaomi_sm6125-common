/*
* Copyright (c) 2014, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted
* provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright notice, this list of
*      conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright notice, this list of
*      conditions and the following disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its contributors may be used to
*      endorse or promote products derived from this software without specific prior written
*      permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// SDE_LOG_TAG definition must precede debug.h include.
#define SDE_LOG_TAG kTagCore
#define SDE_MODULE_NAME "DumpInterface"
#include <utils/debug.h>

#include <utils/constants.h>

#include "dump_impl.h"

namespace sde {

DumpImpl* DumpImpl::dump_list_[] = { 0 };
uint32_t DumpImpl::dump_count_ = 0;

DisplayError DumpInterface::GetDump(uint8_t *buffer, uint32_t length, uint32_t *filled) {
  if (!buffer || !length || !filled) {
    return kErrorParameters;
  }

  DumpImpl::GetDump(buffer, length, filled);
  return kErrorNone;
}

DumpImpl::DumpImpl() {
  Register(this);
}

DumpImpl::~DumpImpl() {
  Unregister(this);
}

// Caller has to ensure that it does not create or destroy devices while using dump interface.
void DumpImpl::GetDump(uint8_t *buffer, uint32_t length, uint32_t *filled) {
  *filled = 0;
  for (uint32_t i = 0; (i < DumpImpl::dump_count_) && (*filled < length); i++) {
    *filled += DumpImpl::dump_list_[i]->GetDump(buffer + *filled, length - *filled);
  }
}

// Every object is created or destroyed through display core only, which itself protects the
// the access, so no need to protect registration or de-registration.
void DumpImpl::Register(DumpImpl *dump_impl) {
  if (dump_count_ < kMaxDumpObjects) {
    dump_list_[dump_count_] = dump_impl;
    dump_count_++;
  }
}

void DumpImpl::Unregister(DumpImpl *dump_impl) {
  for (uint32_t i = 0; i < dump_count_; i++) {
    if (dump_list_[i] == dump_impl) {
      dump_count_--;
      for (; i < dump_count_; i++) {
        dump_list_[i] = dump_list_[i + 1];
      }
    }
  }
}

}  // namespace sde

