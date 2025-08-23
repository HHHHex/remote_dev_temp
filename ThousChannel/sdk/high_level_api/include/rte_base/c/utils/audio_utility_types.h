#pragma once

#include "rte_base/c/stream/stream.h"

// /**
//  * @brief Audio frame types
//  */
// typedef enum RteAudioFrameType {
//   kRteAudioFrameTypePcm16,  // 16-bit PCM audio data
// } RteAudioFrameType;

// /**
//  * @brief Audio frame structure
//  */
// typedef struct RteAudioFrame {
//   RteAudioFrameType type;                    // Audio frame type
//   int samples_per_channel;                   // Number of samples per channel
//   int bytes_per_sample;                      // Number of bytes per sample
//   int channels;                              // Number of audio channels
//   int samples_per_sec;                       // Sample rate (samples per second)
//   void* buffer;                              // Audio data buffer
//   int64_t render_time_in_ms;                 // Render timestamp in milliseconds
//   int avsync_type;                           // Audio-video sync type (reserved)
//   int64_t presentation_in_ms;                // Presentation timestamp in milliseconds
//   size_t audio_track_number;                 // Audio track number
// } RteAudioFrame;

/**
 * @brief Audio volume information
 */
typedef struct RteAudioVolumeInfo {
  RteStream* stream;
  size_t volume;                             // Volume level (0-255)
  size_t vad;                                // Voice Activity Detection
  double voice_pitch;                        // Voice pitch frequency in Hz
} RteAudioVolumeInfo;

