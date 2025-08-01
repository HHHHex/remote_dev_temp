/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef enum RteAudioFrameType {
  kRteAudioFrameTypePcm16,
} RteAudioFrameType;

typedef struct RteAudioFrame {
  RteAudioFrameType type;
  int samples_per_channel;
  int bytes_per_sample;
  int channels;
  int samples_per_sec;
  void *buffer;
  int64_t render_time_in_ms;
  int avsync_type;
  int64_t presentation_in_ms;
  size_t audio_track_number;
} RteAudioFrame;


typedef enum RteVideoPixelFormat {
  kRteVideoPixelFormatDefault = 0,
  kRteVideoPixelFormatI420,
  kRteVideoPixelFormatI422,
  kRteVideoPixelFormatBgra,
  kRteVideoPixelFormatNv21,
  kRteVideoPixelFormatRgba,
  kRteVideoPixelFormatNv12,
  kRteVideoPixelFormatTexture2d,
  kRteVideoPixelFormatTextureOes,
  kRteVideoPixelFormatTextureId3d11Texture2d,
  kRteVideoPixelFormatCvNv12,
  kRteVideoPixelFormatCvI420,
  kRteVideoPixelFormatCvBgra,
} RteVideoPixelFormat;

typedef struct RteVideoFrame {
  RteVideoPixelFormat type;
  size_t width;
  size_t height;
  size_t y_stride;
  size_t u_stride;
  size_t v_stride;
  uint8_t *y_buffer;
  uint8_t *u_buffer;
  uint8_t *v_buffer;
  size_t rotation;
  int64_t render_time_in_ms;
  int avsync_type;
  uint8_t *metadata_buffer;
  size_t metadata_size;
  void *shared_context;
  int texture_id;
  void *d3d11Texture2d;
  float matrix[16];
  uint8_t *alpha_buffer;
  uint8_t *pixel_buffer;
} RteVideoFrame;


#ifdef __cplusplus
}
#endif  // __cplusplus
