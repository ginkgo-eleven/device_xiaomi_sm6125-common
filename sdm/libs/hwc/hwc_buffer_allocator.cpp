/*
* Copyright (c) 2015 - 2017, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above
*    copyright notice, this list of conditions and the following
*    disclaimer in the documentation and/or other materials provided
*    with the distribution.
*  * Neither the name of The Linux Foundation nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <gralloc_priv.h>
#include <memalloc.h>
#include <gr.h>
#include <alloc_controller.h>
#include <utils/constants.h>
#include <utils/debug.h>
#include <core/buffer_allocator.h>
#include <drm_master.h>
#include <qd_utils.h>

#include "hwc_debugger.h"
#include "hwc_buffer_allocator.h"

#define __CLASS__ "HWCBufferAllocator"

using drm_utils::DRMMaster;
using drm_utils::DRMBuffer;

namespace sdm {

HWCBufferAllocator::HWCBufferAllocator() {
  alloc_controller_ = gralloc::IAllocController::getInstance();
}

DisplayError HWCBufferAllocator::AllocateBuffer(BufferInfo *buffer_info) {
  gralloc::alloc_data data;

  const BufferConfig &buffer_config = buffer_info->buffer_config;
  AllocatedBufferInfo *alloc_buffer_info = &buffer_info->alloc_buffer_info;
  MetaBufferInfo *meta_buffer_info = new MetaBufferInfo();

  if (!meta_buffer_info) {
    return kErrorMemory;
  }

  int alloc_flags = INT(GRALLOC_USAGE_PRIVATE_IOMMU_HEAP);
  int error = 0;

  int width = INT(buffer_config.width);
  int height = INT(buffer_config.height);
  int format;

  if (buffer_config.secure_camera) {
    alloc_flags = GRALLOC_USAGE_HW_CAMERA_WRITE;
    alloc_flags |= (GRALLOC_USAGE_PROTECTED | GRALLOC_USAGE_HW_COMPOSER);
    data.align = SZ_2M;
  } else if (buffer_config.secure) {
    alloc_flags = INT(GRALLOC_USAGE_PRIVATE_MM_HEAP);
    alloc_flags |= INT(GRALLOC_USAGE_PROTECTED);
    data.align = SECURE_ALIGN;
  } else {
    data.align = UINT32(getpagesize());
  }

  if (buffer_config.cache == false) {
    // Allocate uncached buffers
    alloc_flags |= GRALLOC_USAGE_PRIVATE_UNCACHED;
  }

  error = SetBufferInfo(buffer_config.format, &format, &alloc_flags);
  if (error != 0) {
    delete meta_buffer_info;
    return kErrorParameters;
  }

  int aligned_width = 0, aligned_height = 0;
  uint32_t buffer_size = getBufferSizeAndDimensions(width, height, format, alloc_flags,
                                                    aligned_width, aligned_height);

  buffer_size = ROUND_UP(buffer_size, data.align) * buffer_config.buffer_count;

  data.base = 0;
  data.fd = -1;
  data.offset = 0;
  data.size = buffer_size;
  data.uncached = !buffer_config.cache;

  error = alloc_controller_->allocate(data, alloc_flags);
  if (error != 0) {
    DLOGE("Error allocating memory size %d uncached %d", data.size, data.uncached);
    delete meta_buffer_info;
    return kErrorMemory;
  }

  alloc_buffer_info->fd = data.fd;
  // TODO(user): define stride for all planes and fix stride in bytes
  alloc_buffer_info->stride = UINT32(aligned_width);
  alloc_buffer_info->aligned_width = UINT32(aligned_width);
  alloc_buffer_info->aligned_height = UINT32(aligned_height);
  alloc_buffer_info->size = buffer_size;

  meta_buffer_info->base_addr = data.base;
  meta_buffer_info->alloc_type = data.allocType;

  buffer_info->private_data = meta_buffer_info;

  if (qdutils::getDriverType() == qdutils::DriverType::DRM) {
    private_handle_t handle(-1, 0, 0, 0, 0, 0, 0);
    // Setup only the required stuff, skip rest
    handle.base = reinterpret_cast<uint64_t>(data.base);
    handle.format = format;
    handle.width = aligned_width;
    handle.height = aligned_height;
    if (alloc_flags & GRALLOC_USAGE_PRIVATE_ALLOC_UBWC) {
      handle.flags = private_handle_t::PRIV_FLAGS_UBWC_ALIGNED;
    }
    private_handle_t *hnd = &handle;
    DRMBuffer buf = {};
    int ret = getPlaneStrideOffset(hnd, buf.stride, buf.offset,
                                   &buf.num_planes);
    if (ret < 0) {
      DLOGE("getPlaneStrideOffset failed");
      return kErrorParameters;
    }

    buf.fd = data.fd;
    buf.width = UINT32(hnd->width);
    buf.height = UINT32(hnd->height);
    getDRMFormat(hnd->format, hnd->flags, &buf.drm_format,
                 &buf.drm_format_modifier);

    DRMMaster *master = nullptr;
    ret = DRMMaster::GetInstance(&master);
    if (ret < 0) {
      DLOGE("Failed to acquire DRMMaster instance");
      return kErrorParameters;
    }

    ret = master->CreateFbId(buf, &alloc_buffer_info->gem_handle, &alloc_buffer_info->fb_id);
    if (ret < 0) {
      DLOGE("CreateFbId failed. width %d, height %d, " \
            "format: %s, stride %u, error %d",
            buf.width, buf.height,
            qdutils::GetHALPixelFormatString(hnd->format),
            buf.stride[0], errno);
      return kErrorParameters;
    }
  }

  return kErrorNone;
}

DisplayError HWCBufferAllocator::FreeBuffer(BufferInfo *buffer_info) {
  int ret = 0;
  AllocatedBufferInfo *alloc_buffer_info = &buffer_info->alloc_buffer_info;

  // Deallocate the buffer, only if the buffer fd is valid.
  if (alloc_buffer_info->fd > 0) {
    MetaBufferInfo *meta_buffer_info = static_cast<MetaBufferInfo *> (buffer_info->private_data);
    gralloc::IMemAlloc *memalloc = alloc_controller_->getAllocator(meta_buffer_info->alloc_type);
    if (memalloc == NULL) {
      DLOGE("Memalloc handle is NULL, alloc type %d", meta_buffer_info->alloc_type);
      return kErrorResources;
    }

    ret = memalloc->free_buffer(meta_buffer_info->base_addr, alloc_buffer_info->size, 0,
                                alloc_buffer_info->fd);
    if (ret != 0) {
      DLOGE("Error freeing buffer base_addr %p size %d fd %d", meta_buffer_info->base_addr,
            alloc_buffer_info->size, alloc_buffer_info->fd);
      return kErrorMemory;
    }

    alloc_buffer_info->fd = -1;
    alloc_buffer_info->stride = 0;
    alloc_buffer_info->aligned_width = 0;
    alloc_buffer_info->aligned_height = 0;
    alloc_buffer_info->size = 0;

    meta_buffer_info->base_addr = NULL;
    meta_buffer_info->alloc_type = 0;

    delete meta_buffer_info;
    meta_buffer_info = NULL;

    if (alloc_buffer_info->fb_id) {
      DRMMaster *master = nullptr;
      int ret = DRMMaster::GetInstance(&master);
      if (ret < 0) {
        DLOGE("Failed to acquire DRMMaster instance");
        return kErrorParameters;
      }

      ret = master->RemoveFbId(alloc_buffer_info->gem_handle, alloc_buffer_info->fb_id);
      if (ret < 0) {
        DLOGE("Removing fb_id %d failed with error %d",
              alloc_buffer_info->fb_id, errno);
        return kErrorParameters;
      }

      alloc_buffer_info->fb_id = 0;
      alloc_buffer_info->gem_handle = 0;
    }
  }

  return kErrorNone;
}

uint32_t HWCBufferAllocator::GetBufferSize(BufferInfo *buffer_info) {
  uint32_t align = UINT32(getpagesize());

  const BufferConfig &buffer_config = buffer_info->buffer_config;

  int alloc_flags = INT(GRALLOC_USAGE_PRIVATE_IOMMU_HEAP);

  int width = INT(buffer_config.width);
  int height = INT(buffer_config.height);
  int format;

  if (buffer_config.secure_camera) {
    alloc_flags = GRALLOC_USAGE_HW_CAMERA_WRITE;
    alloc_flags |= (GRALLOC_USAGE_PROTECTED | GRALLOC_USAGE_HW_COMPOSER);
    align = SZ_2M;
  } else if (buffer_config.secure) {
    alloc_flags = INT(GRALLOC_USAGE_PRIVATE_MM_HEAP);
    alloc_flags |= INT(GRALLOC_USAGE_PROTECTED);
    align = SECURE_ALIGN;
  }

  if (buffer_config.cache == false) {
    // Allocate uncached buffers
    alloc_flags |= GRALLOC_USAGE_PRIVATE_UNCACHED;
  }

  if (SetBufferInfo(buffer_config.format, &format, &alloc_flags) < 0) {
    return 0;
  }

  int aligned_width = 0;
  int aligned_height = 0;
  uint32_t buffer_size = getBufferSizeAndDimensions(width, height, format, alloc_flags,
                                                    aligned_width, aligned_height);

  buffer_size = ROUND_UP(buffer_size, align) * buffer_config.buffer_count;

  return buffer_size;
}

int HWCBufferAllocator::SetBufferInfo(LayerBufferFormat format, int *target, int *flags) {
  switch (format) {
  case kFormatRGBA8888:                 *target = HAL_PIXEL_FORMAT_RGBA_8888;             break;
  case kFormatRGBX8888:                 *target = HAL_PIXEL_FORMAT_RGBX_8888;             break;
  case kFormatRGB888:                   *target = HAL_PIXEL_FORMAT_RGB_888;               break;
  case kFormatRGB565:                   *target = HAL_PIXEL_FORMAT_RGB_565;               break;
  case kFormatBGR565:                   *target = HAL_PIXEL_FORMAT_BGR_565;               break;
  case kFormatBGRA8888:                 *target = HAL_PIXEL_FORMAT_BGRA_8888;             break;
  case kFormatYCrCb420PlanarStride16:   *target = HAL_PIXEL_FORMAT_YV12;                  break;
  case kFormatYCrCb420SemiPlanar:       *target = HAL_PIXEL_FORMAT_YCrCb_420_SP;          break;
  case kFormatYCbCr420SemiPlanar:       *target = HAL_PIXEL_FORMAT_YCbCr_420_SP;          break;
  case kFormatYCbCr422H2V1Packed:       *target = HAL_PIXEL_FORMAT_YCbCr_422_I;           break;
  case kFormatCbYCrY422H2V1Packed:      *target = HAL_PIXEL_FORMAT_CbYCrY_422_I;          break;
  case kFormatYCbCr422H2V1SemiPlanar:   *target = HAL_PIXEL_FORMAT_YCbCr_422_SP;          break;
  case kFormatYCbCr420SemiPlanarVenus:  *target = HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS;    break;
  case kFormatYCrCb420SemiPlanarVenus:  *target = HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS;    break;
  case kFormatYCbCr420SPVenusUbwc:
    *target = HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatRGBA5551:                 *target = HAL_PIXEL_FORMAT_RGBA_5551;             break;
  case kFormatRGBA4444:                 *target = HAL_PIXEL_FORMAT_RGBA_4444;             break;
  case kFormatRGBA1010102:              *target = HAL_PIXEL_FORMAT_RGBA_1010102;          break;
  case kFormatARGB2101010:              *target = HAL_PIXEL_FORMAT_ARGB_2101010;          break;
  case kFormatRGBX1010102:              *target = HAL_PIXEL_FORMAT_RGBX_1010102;          break;
  case kFormatXRGB2101010:              *target = HAL_PIXEL_FORMAT_XRGB_2101010;          break;
  case kFormatBGRA1010102:              *target = HAL_PIXEL_FORMAT_BGRA_1010102;          break;
  case kFormatABGR2101010:              *target = HAL_PIXEL_FORMAT_ABGR_2101010;          break;
  case kFormatBGRX1010102:              *target = HAL_PIXEL_FORMAT_BGRX_1010102;          break;
  case kFormatXBGR2101010:              *target = HAL_PIXEL_FORMAT_XBGR_2101010;          break;
  case kFormatYCbCr420P010:             *target = HAL_PIXEL_FORMAT_YCbCr_420_P010;        break;
  case kFormatYCbCr420TP10Ubwc:
    *target = HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatYCbCr420P010Ubwc:
    *target = HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatRGBA8888Ubwc:
    *target = HAL_PIXEL_FORMAT_RGBA_8888;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatRGBX8888Ubwc:
    *target = HAL_PIXEL_FORMAT_RGBX_8888;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatBGR565Ubwc:
    *target = HAL_PIXEL_FORMAT_BGR_565;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatRGBA1010102Ubwc:
    *target = HAL_PIXEL_FORMAT_RGBA_1010102;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  case kFormatRGBX1010102Ubwc:
    *target = HAL_PIXEL_FORMAT_RGBX_1010102;
    *flags |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
    break;
  default:
    DLOGE("Unsupported format = 0x%x", format);
    return -EINVAL;
  }

  return 0;
}

DisplayError HWCBufferAllocator::GetAllocatedBufferInfo(const BufferConfig &buffer_config,
                                 AllocatedBufferInfo *allocated_buffer_info) {
  int width = INT(buffer_config.width);
  int height = INT(buffer_config.height);
  int alloc_flags = INT(GRALLOC_USAGE_PRIVATE_IOMMU_HEAP);

  if (buffer_config.secure) {
    alloc_flags = INT(GRALLOC_USAGE_PRIVATE_MM_HEAP);
    alloc_flags |= INT(GRALLOC_USAGE_PROTECTED);
  }

  if (buffer_config.cache == false) {
    // Allocate uncached buffers
    alloc_flags |= GRALLOC_USAGE_PRIVATE_UNCACHED;
  }

  int format;
  int error = SetBufferInfo(buffer_config.format, &format, &alloc_flags);
  if (error) {
    DLOGE("Failed: format = %d or width = %d height = %d", buffer_config.format, width, height);
    return kErrorNotSupported;
  }

  int width_aligned = 0, height_aligned = 0;
  uint32_t buffer_size = 0;
  buffer_size = getBufferSizeAndDimensions(width, height, format, alloc_flags,
                                           width_aligned, height_aligned);

  allocated_buffer_info->stride = UINT32(width_aligned);
  allocated_buffer_info->aligned_width = UINT32(width_aligned);
  allocated_buffer_info->aligned_height = UINT32(height_aligned);
  allocated_buffer_info->size = UINT32(buffer_size);

  return kErrorNone;
}

}  // namespace sdm
