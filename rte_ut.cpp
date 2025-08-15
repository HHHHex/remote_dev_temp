/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#include <memory>
#include <thread>
#include "test_base/agora_gtest.h"
#include "test_base/test_utils.h"

#include <api/cpp/aosl_ares_class.h>

#include "api2/internal/agora_service_i.h"

#define private public
#define protected public
#include "rte.h"

#include "rte_cpp.h"

#include "rte/utils/string.h"
#include "rte/utils/url.h"
#undef private
#undef protected

#include "user_channel_test_base.h"
#include "gtest/gtest.h"
#include "test_base/agora_dynamic_key/RtmTokenBuilder2.h"

#include "test_base/agora_dynamic_key/access_token2_utils.h"
#include "test_base/agora_dynamic_key/access_token_utils.h"
#include "test_base/agora_dynamic_key/token_appid_sign.h"

#include "../rtc_engine_sdk/window/simple_window.h"

#if defined(_WIN32)
#define SimpleWindow SimpleWindow_Win
#elif defined(WEBRTC_ANDROID)
#define SimpleWindow SimpleWindow_Android
#elif defined(WEBRTC_IOS)
#define SimpleWindow SimpleWindow_iOS
#elif (defined(WEBRTC_MAC) && !defined(WEBRTC_IOS))
#define SimpleWindow SimpleWindow_Mac
#endif

ATEST(RteApiCppTestChannel, RteChannelTest) {
  rte::Error err;

  rte::Observer observer;
  rte::Rte rte;

  rte::Config rteConfig;
  rteConfig.SetAppId(kTestDynamicAppId.c_str(), nullptr);

  rte.SetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.GetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.RegisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> initResult = std::make_shared<AsyncResult<bool>>();
    rte.InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            initResult->SetResult(true);
          } else {
            initResult->SetResult(false);
          }
        },
        nullptr);

    auto result = initResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  rte::LocalUser localUser(&rte);
  LocalUserObserver localUserObserver;

  rte::LocalUserConfig localUserConfig;
  localUserConfig.SetUserId(kTestLocalUserId);

  std::string token = agora::tools::RtmTokenBuilder2::BuildToken(
      kTestDynamicAppId, kTestDynamicAppCert, kTestLocalUserId, 600);
  localUserConfig.SetUserToken(token);

  localUser.SetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.GetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.RegisterObserver(&localUserObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> connectResult = std::make_shared<AsyncResult<bool>>();
    localUser.Connect([connectResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        connectResult->SetResult(true);
      } else {
        connectResult->SetResult(false);
      }
    });

    auto result = connectResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  rte::Channel channel(&rte);
  ChannelObserver channelObserver;

  rte::ChannelConfig channelConfig;
  channelConfig.SetChannelId(kTestChannelId);

  rte.UnregisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.Destroy();
}

ATEST(RteApiCppTestChannel, RteChannelPublishStream) {
  rte::Error err;

  rte::Observer observer;
  rte::Rte rte;

  rte::Config rteConfig;
  rteConfig.SetAppId(LIVE_DYNAMIC_APPID, nullptr);

rteConfig.SetJsonParameter("{\"rtc.vocs_list\":[\"183.131.160.208\"]}", &err);
ASSERT_EQ(err.Code(), kRteOk);

  rte.SetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.GetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.RegisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> initResult = std::make_shared<AsyncResult<bool>>();
    rte.InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            initResult->SetResult(true);
          } else {
            initResult->SetResult(false);
          }
        },
        nullptr);

    auto result = initResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // set canvas to render video track
  rte::CanvasInitialConfig canvas_initial_config;
  rte::Canvas canvas(&rte);
  rte::ViewConfig viewConfig;

#if defined(FEATURE_VIDEO)
#if defined(WEBRTC_WIN) || defined(WEBRTC_ANDROID) || defined(WEBRTC_MAC) || defined(WEBRTC_IOS)

  auto win_window = std::make_unique<SimpleWindow>("SimpleWindow");

  rte::View view = reinterpret_cast<rte::View>(win_window->GetView());

  canvas.AddView(&view, &viewConfig, &err);

  rte::CanvasConfig canvasConfig;
  canvasConfig.SetRenderMode(rte::VideoRenderMode::kRteVideoRenderModeFit);
  canvas.SetConfigs(&canvasConfig, &err);

  ASSERT_EQ(err.Code(), kRteOk);

#endif
#endif

  // local user
  rte::LocalUser localUser(&rte);

  rte::LocalUserConfig localUserConfig;
  localUserConfig.SetUserId(kTestLocalUserId);

  std::string token = agora::tools::RtmTokenBuilder2::BuildToken(LIVE_DYNAMIC_APPID, LIVE_SIGN,
                                                                 kTestLocalUserId, 600);
  localUserConfig.SetUserToken(token);

  localUser.SetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // channel
  // std::string channel_id = API_CALL_CHANNEL_NAME;
  std::string channel_id = "123456";

  rte::Channel channel(&rte);
  ChannelObserver channelObserver;

  rte::ChannelConfig channelConfig;
  channelConfig.SetChannelId(channel_id);

  channel.SetConfigs(&channelConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  channel.RegisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // mic audio track
  rte::MicAudioTrack mic_audio_track(&rte);
  rte::MicAudioTrackConfig mic_audio_track_config;
  mic_audio_track.SetConfigs(&mic_audio_track_config, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // start mic audio track
  {
    std::shared_ptr<AsyncResult<bool>> mic_audio_track_start_result =
        std::make_shared<AsyncResult<bool>>();
    mic_audio_track.Start([=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        mic_audio_track_start_result->SetResult(true);
      } else {
        mic_audio_track_start_result->SetResult(false);
      }
    });

    auto result = mic_audio_track_start_result->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  rte::CameraVideoTrack camera_video_track(&rte);
  rte::CameraVideoTrackConfig camera_video_track_config;
  camera_video_track.SetConfigs(&camera_video_track_config, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // start camera video track
  {
    std::shared_ptr<AsyncResult<bool>> camera_video_track_start_result =
        std::make_shared<AsyncResult<bool>>();
    camera_video_track.Start([=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        camera_video_track_start_result->SetResult(true);
      } else {
        camera_video_track_start_result->SetResult(false);
      }
    });

    auto result = camera_video_track_start_result->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // set canvas to render video track
  {
    std::shared_ptr<AsyncResult<bool>> set_canvas_result = std::make_shared<AsyncResult<bool>>();
    camera_video_track.SetCanvas(
        &canvas, rte::VideoPipelinePosition::kRteVideoPipelinePositionLocalPostCapturer,
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            set_canvas_result->SetResult(true);
          } else {
            set_canvas_result->SetResult(false);
          }
        });

    // auto result = set_canvas_result->Wait(WaitTimeOutMs);
  }

  // local realtime stream
  rte::LocalRealTimeStream local_realtime_stream(&rte);

  std::string stream_id = "bob_stream_id";
  rte::LocalRealTimeStreamConfig local_realtime_stream_config;
  local_realtime_stream_config.SetStreamId(stream_id, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // generate token
  const int kLongTokenExpireTimeInSeconds = 3600 * 24;
  const std::string bob_stream_token =
      genAccessToken2WithStringUid(LIVE_DYNAMIC_APPID, LIVE_SIGN, channel_id, stream_id,
                                   kLongTokenExpireTimeInSeconds, kLongTokenExpireTimeInSeconds);
  EXPECT_FALSE(bob_stream_token.empty());

  local_realtime_stream_config.SetStreamToken(bob_stream_token, &err);
  ASSERT_EQ(err.Code(), kRteOk);
  local_realtime_stream_config.SetAudioEncoderProfile(
      rte::AudioEncoderProfile::kRteAudioEncoderProfileDefault);

  local_realtime_stream.SetConfigs(&local_realtime_stream_config, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  local_realtime_stream.AddAudioTrack(&mic_audio_track, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  local_realtime_stream.AddVideoTrack(&camera_video_track, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // publish stream
  {
    std::shared_ptr<AsyncResult<bool>> publishResult = std::make_shared<AsyncResult<bool>>();
    channel.PublishStream(&localUser, &local_realtime_stream, [=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        publishResult->SetResult(true);
      } else {
        publishResult->SetResult(false);
      }
    });

    auto result = publishResult->Wait(WaitTimeOutMs);
     ASSERT_EQ(result.valid, true);
     ASSERT_EQ(result.value, true);
  }

  // sleep for 10s
  std::cout << "channel id:" << channel_id << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(60 * 5));

  // unpublish stream
  {
    std::shared_ptr<AsyncResult<bool>> unpublishResult = std::make_shared<AsyncResult<bool>>();
    channel.UnpublishStream(&local_realtime_stream, [=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        unpublishResult->SetResult(true);
      } else {
        unpublishResult->SetResult(false);
      }
    });

    auto result = unpublishResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // stop mic audio track
  {
    std::shared_ptr<AsyncResult<bool>> mic_audio_track_stop_result =
        std::make_shared<AsyncResult<bool>>();
    mic_audio_track.Stop([=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        mic_audio_track_stop_result->SetResult(true);
      } else {
        mic_audio_track_stop_result->SetResult(false);
      }
    });

    auto result = mic_audio_track_stop_result->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // stop camera video track
  {
    std::shared_ptr<AsyncResult<bool>> camera_video_track_stop_result =
        std::make_shared<AsyncResult<bool>>();
    camera_video_track.Stop([=](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        camera_video_track_stop_result->SetResult(true);
      } else {
        camera_video_track_stop_result->SetResult(false);
      }
    });

    auto result = camera_video_track_stop_result->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  channel.UnregisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // unregister observer
  rte.UnregisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.Destroy();
}

class RteChannelStreamPublisher {
 public:
  RteChannelStreamPublisher(const std::string& app_id, const std::string& app_cert,
                            const std::string& user_id, const std::string& channel_id,
                            const std::string& stream_id) {
    app_id_ = app_id;
    app_cert_ = app_cert;
    user_id_ = user_id;
    channel_id_ = channel_id;
    stream_id_ = stream_id;

    Init();
  }

  ~RteChannelStreamPublisher() { Deinit(); }

  void Init() {
    rte::Error err;
    rte_ = std::make_shared<rte::Rte>();
    rte::Config rteConfig;
    rteConfig.SetAppId(app_id_.c_str(), nullptr);
    rte_->SetConfigs(&rteConfig, &err);

    std::shared_ptr<AsyncResult<bool>> init_media_engine_result_ =
        std::make_shared<AsyncResult<bool>>();
    rte_->InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            init_media_engine_result_->SetResult(true);
          } else {
            init_media_engine_result_->SetResult(false);
          }
        },
        nullptr);

    auto result = init_media_engine_result_->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  void Deinit() {
    if (rte_) {
      rte_->Destroy();
      rte_.reset();
    }
  }

  void PublishStream() {
    rte::Error err;

    local_user_ = std::make_shared<rte::LocalUser>(rte_.get());

    rte::LocalUserConfig local_user_config;
    local_user_config.SetUserId(user_id_);

    std::string token =
        agora::tools::RtmTokenBuilder2::BuildToken(app_id_, app_cert_, user_id_, 600);
    local_user_config.SetUserToken(token);

    local_user_->SetConfigs(&local_user_config, &err);
    ASSERT_EQ(err.Code(), kRteOk);

    {
      std::shared_ptr<AsyncResult<bool>> connect_result_ = std::make_shared<AsyncResult<bool>>();
      local_user_->Connect([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          connect_result_->SetResult(true);
        } else {
          connect_result_->SetResult(false);
        }
      });

      auto result = connect_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    channel_ = std::make_shared<rte::Channel>(rte_.get());
    rte::ChannelConfig channel_config;
    channel_config.SetChannelId(channel_id_);
    channel_->SetConfigs(&channel_config, &err);

    local_realtime_stream_ = std::make_shared<rte::LocalRealTimeStream>(rte_.get());
    rte::LocalRealTimeStreamConfig local_realtime_stream_config;
    local_realtime_stream_config.SetStreamId(stream_id_, &err);

    const int kLongTokenExpireTimeInSeconds = 3600 * 24;
    const std::string stream_token =
        genAccessToken2WithStringUid(app_id_, app_cert_, channel_id_, stream_id_,
                                     kLongTokenExpireTimeInSeconds, kLongTokenExpireTimeInSeconds);
    EXPECT_FALSE(stream_token.empty());

    local_realtime_stream_config.SetStreamToken(stream_token, &err);

    local_realtime_stream_->SetConfigs(&local_realtime_stream_config, &err);
    ASSERT_EQ(err.Code(), kRteOk);

    mic_audio_track_ = std::make_shared<rte::MicAudioTrack>(rte_.get());
    rte::MicAudioTrackConfig mic_audio_track_config;
    mic_audio_track_->SetConfigs(&mic_audio_track_config, &err);
    ASSERT_EQ(err.Code(), kRteOk);

    {
      std::shared_ptr<AsyncResult<bool>> mic_audio_track_start_result_ =
          std::make_shared<AsyncResult<bool>>();
      mic_audio_track_->Start([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          mic_audio_track_start_result_->SetResult(true);
        } else {
          mic_audio_track_start_result_->SetResult(false);
        }
      });

      auto result = mic_audio_track_start_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    camera_video_track_ = std::make_shared<rte::CameraVideoTrack>(rte_.get());
    rte::CameraVideoTrackConfig camera_video_track_config;
    camera_video_track_->SetConfigs(&camera_video_track_config, &err);
    ASSERT_EQ(err.Code(), kRteOk);

    {
      std::shared_ptr<AsyncResult<bool>> camera_video_track_start_result_ =
          std::make_shared<AsyncResult<bool>>();

      camera_video_track_->Start([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          camera_video_track_start_result_->SetResult(true);
        } else {
          camera_video_track_start_result_->SetResult(false);
        }
      });

      auto result = camera_video_track_start_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    local_realtime_stream_->AddAudioTrack(mic_audio_track_.get(), &err);
    ASSERT_EQ(err.Code(), kRteOk);

    local_realtime_stream_->AddVideoTrack(camera_video_track_.get(), &err);
    ASSERT_EQ(err.Code(), kRteOk);

    {
      std::shared_ptr<AsyncResult<bool>> publish_result_ = std::make_shared<AsyncResult<bool>>();
      channel_->PublishStream(local_user_.get(), local_realtime_stream_.get(),
                              [=](rte::Error* err) {
                                if (err && err->Code() == kRteOk) {
                                  publish_result_->SetResult(true);
                                } else {
                                  publish_result_->SetResult(false);
                                }
                              });

      auto result = publish_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }
  }

  void UnpublishStream() {
    rte::Error err;

    {
      std::shared_ptr<AsyncResult<bool>> unpublish_result_ = std::make_shared<AsyncResult<bool>>();
      channel_->UnpublishStream(local_realtime_stream_.get(), [=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          unpublish_result_->SetResult(true);
        } else {
          unpublish_result_->SetResult(false);
        }
      });

      auto result = unpublish_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    local_realtime_stream_->DelAudioTrack(mic_audio_track_.get(), &err);
    ASSERT_EQ(err.Code(), kRteOk);

    local_realtime_stream_->DelVideoTrack(camera_video_track_.get(), &err);
    ASSERT_EQ(err.Code(), kRteOk);

    {
      std::shared_ptr<AsyncResult<bool>> mic_audio_track_stop_result_ =
          std::make_shared<AsyncResult<bool>>();
      mic_audio_track_->Stop([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          mic_audio_track_stop_result_->SetResult(true);
        } else {
          mic_audio_track_stop_result_->SetResult(false);
        }
      });

      auto result = mic_audio_track_stop_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    {
      std::shared_ptr<AsyncResult<bool>> camera_video_track_stop_result_ =
          std::make_shared<AsyncResult<bool>>();
      camera_video_track_->Stop([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          camera_video_track_stop_result_->SetResult(true);
        } else {
          camera_video_track_stop_result_->SetResult(false);
        }
      });

      auto result = camera_video_track_stop_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    {
      std::shared_ptr<AsyncResult<bool>> disconnect_result_ = std::make_shared<AsyncResult<bool>>();
      local_user_->Disconnect([=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          disconnect_result_->SetResult(true);
        } else {
          disconnect_result_->SetResult(false);
        }
      });

      auto result = disconnect_result_->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }
    rte_->Destroy();
    rte_.reset();
  }

 public:
  std::shared_ptr<rte::Rte> rte_ = nullptr;
  std::shared_ptr<rte::LocalUser> local_user_ = nullptr;
  std::shared_ptr<rte::Channel> channel_ = nullptr;
  std::shared_ptr<rte::LocalRealTimeStream> local_realtime_stream_ = nullptr;
  std::shared_ptr<rte::MicAudioTrack> mic_audio_track_ = nullptr;
  std::shared_ptr<rte::CameraVideoTrack> camera_video_track_ = nullptr;

  std::string app_id_ = "";
  std::string app_cert_ = "";
  std::string user_id_ = "";
  std::string user_token_ = "";
  std::string channel_id_ = "";
  std::string stream_id_ = "";
  std::string token_ = "";
};

ATEST(RteApiCppTestChannel, RteChannelReceiveStream) {
  std::string app_id = LIVE_DYNAMIC_APPID;
  std::string app_cert = LIVE_SIGN;
  std::string publisher_user_id = "publisher_user_id";
  // std::string channel_id = API_CALL_CHANNEL_NAME;
  std::string channel_id = "123456";
  std::string publisher_stream_id = "publisher_stream_id";

  std::string subscriber_user_id = "subscriber_user_id";
  //std::string subscriber_user_id = "100001";

  // RteChannelStreamPublisher publisher(app_id, app_cert, publisher_user_id, channel_id,
  //                                    publisher_stream_id);

  // publisher.Init();
  // publisher.PublishStream();

  rte::Error err;

  rte::Observer observer;
  rte::Rte rte;

  rte::Config rteConfig;
  rteConfig.SetAppId(app_id.c_str(), nullptr);
  // rteConfig.SetUseStringUid(false);

  rteConfig.SetJsonParameter("{\"rtc.vocs_list\":[\"183.131.160.208\"]}", &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.SetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.GetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.RegisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> initResult = std::make_shared<AsyncResult<bool>>();
    rte.InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            initResult->SetResult(true);
          } else {
            initResult->SetResult(false);
          }
        },
        nullptr);

    auto result = initResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // set canvas to render video track
  rte::CanvasInitialConfig canvas_initial_config;
  rte::Canvas canvas(&rte);
  rte::ViewConfig viewConfig;

#if defined(FEATURE_VIDEO)
#if defined(WEBRTC_WIN) || defined(WEBRTC_ANDROID) || defined(WEBRTC_MAC) || defined(WEBRTC_IOS)

  auto win_window = std::make_unique<SimpleWindow>("SimpleWindow");

  rte::View view = reinterpret_cast<rte::View>(win_window->GetView());

  canvas.AddView(&view, &viewConfig, &err);

  rte::CanvasConfig canvasConfig;
  canvasConfig.SetRenderMode(rte::VideoRenderMode::kRteVideoRenderModeFit);
  canvas.SetConfigs(&canvasConfig, &err);

  ASSERT_EQ(err.Code(), kRteOk);

#endif
#endif

  rte::LocalUser localUser(&rte);
  LocalUserObserver localUserObserver;

  rte::LocalUserConfig localUserConfig;
  localUserConfig.SetUserId(subscriber_user_id);

  std::string token =
      agora::tools::RtmTokenBuilder2::BuildToken(app_id, app_cert, subscriber_user_id, 600);
  localUserConfig.SetUserToken(token);

  localUser.SetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.GetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.RegisterObserver(&localUserObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // {
  //   std::shared_ptr<AsyncResult<bool>> connectResult = std::make_shared<AsyncResult<bool>>();
  //   localUser.Connect([connectResult](rte::Error* err) {
  //     if (err && err->Code() == kRteOk) {
  //       connectResult->SetResult(true);
  //     } else {
  //       connectResult->SetResult(false);
  //     }
  //   });

  //   auto result = connectResult->Wait(WaitTimeOutMs);
  //   ASSERT_EQ(result.valid, true);
  //   ASSERT_EQ(result.value, true);
  // }

  rte::Channel channel(&rte);
  ChannelObserver channelObserver;

  const int kLongTokenExpireTimeInSeconds = 3600 * 24;
  const std::string channel_token =
      genAccessToken2WithStringUid(app_id, app_cert, channel_id, subscriber_user_id,
                                   kLongTokenExpireTimeInSeconds, kLongTokenExpireTimeInSeconds);
  EXPECT_FALSE(channel_token.empty());

  rte::ChannelConfig channelConfig;
  channelConfig.SetChannelId(channel_id);
  channelConfig.SetAllowAutoSubscribeAll(false);
  channelConfig.SetChannelToken(channel_token, &err);
  channelConfig.SetAutoSubscribeMessage(false);
  channelConfig.SetAutoSubscribeUserPresence(false);
  channelConfig.SetAutoSubscribeMetadata(false);

  ASSERT_EQ(err.Code(), kRteOk);

  channel.SetConfigs(&channelConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  channel.RegisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    channel.Join(&localUser, &err);
    ASSERT_EQ(err.Code(), kRteOk);
    // {
    //   auto result = channelObserver.has_subscribe_user_presence_result_.Wait(WaitTimeOutMs);
    //   ASSERT_EQ(result.valid, true);
    //   ASSERT_EQ(result.value, true);
    //   ASSERT_EQ(channelObserver.subscribe_user_presence_error_.Code(), kRteOk);
    // }

    {
      auto result = channelObserver.has_subscribe_stream_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.subscribe_stream_presence_error_.Code(), kRteOk);
    }
  }

  // wait for remote stream added
  {
    auto result = channelObserver.has_remote_stream_added_.Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  size_t remote_streams_count = channel.GetRemoteStreamsCount(&err);

  std::vector<rte::RemoteStream> remote_streams;
  channel.GetRemoteStreams(remote_streams, 0, remote_streams_count, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // subscribe per remote stream
  for (auto& remote_stream : remote_streams) {
    rte::RemoteStreamInfo remote_stream_info;
    remote_stream.GetInfo(&remote_stream_info, &err);
    std::string remote_stream_id = remote_stream_info.GetStreamId();
    bool has_audio = remote_stream_info.HasAudio();
    bool has_video = remote_stream_info.HasVideo();

    has_audio = true;
    has_video = true;

    // subscribe audio track
    if (has_audio) {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);

      std::shared_ptr<AsyncResult<bool>> subscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.SubscribeTrack(remote_stream_id, &subscribeOption,
                             [=](rte::Track* track, rte::Error* err) {
                               if (err && err->Code() == kRteOk) {
                                 subscribeResult->SetResult(true);
                               } else {
                                 subscribeResult->SetResult(false);
                               }
                             });

      auto result = subscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    // subscribe video track
    if (has_video) {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);

      std::shared_ptr<rte::Track> subscribe_track;
      std::shared_ptr<AsyncResult<bool>> subscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.SubscribeTrack(
          remote_stream_id, &subscribeOption,
          [=, &subscribe_track](rte::Track* track, rte::Error* err) {
            if (err && err->Code() == kRteOk) {
              subscribe_track = std::make_shared<rte::Track>(track->get_underlying_impl()->handle);
              subscribeResult->SetResult(true);
            } else {
              subscribeResult->SetResult(false);
            }
          });

      auto result = subscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);

      rte::VideoTrack video_track(subscribe_track->get_underlying_impl()->handle);

      video_track.SetCanvas(&canvas,
                            rte::VideoPipelinePosition::kRteVideoPipelinePositionRemotePreRenderer,
                            [=](rte::Error* err) { ASSERT_EQ(err->Code(), kRteOk); });
    }
  }

  // wait for some time

  std::this_thread::sleep_for(std::chrono::seconds(600));

  for (auto& remote_stream : remote_streams) {
    // unsubscribe audio track
    {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);

      std::shared_ptr<AsyncResult<bool>> unsubscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.UnsubscribeTrack(remote_stream_id, &subscribeOption, [=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          unsubscribeResult->SetResult(true);
        } else {
          unsubscribeResult->SetResult(false);
        }
      });

      auto result = unsubscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    // unsubscribe video track
    {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);

      std::shared_ptr<AsyncResult<bool>> unsubscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.UnsubscribeTrack(remote_stream_id, &subscribeOption, [=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          unsubscribeResult->SetResult(true);
        } else {
          unsubscribeResult->SetResult(false);
        }
      });

      auto result = unsubscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }
  }

  {
    channel.Leave(&err);

    ASSERT_EQ(err.Code(), kRteOk);

    {
      auto result = channelObserver.has_unsubscribe_user_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.unsubscribe_user_presence_error_.Code(), kRteOk);
    }

    {
      auto result = channelObserver.has_unsubscribe_stream_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.unsubscribe_stream_presence_error_.Code(), kRteOk);
    }
  }

  channel.UnregisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    // local user logout
    std::shared_ptr<AsyncResult<bool>> disconnectResult = std::make_shared<AsyncResult<bool>>();
    localUser.Disconnect([disconnectResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        disconnectResult->SetResult(true);
      } else {
        disconnectResult->SetResult(false);
      }
    });

    auto result = disconnectResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // local user unregister observer
  localUser.UnregisterObserver(&localUserObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // unregister observer
  rte.UnregisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.Destroy();

  // publisher.UnpublishStream();
  // publisher.Deinit();
}


ATEST(RteApiCppTestChannel, RteChannelPublishAndReceiveStreamWithIntUid) {
  std::string app_id = LIVE_DYNAMIC_APPID;
  std::string app_cert = LIVE_SIGN;
  std::string publisher_user_id = "100001";
  std::string channel_id = "123456";
  std::string publisher_stream_id = "100001";
  std::string subscriber_user_id = "100002";

  RteChannelStreamPublisher publisher(app_id, app_cert, publisher_user_id, channel_id,
                                     publisher_stream_id);

  publisher.Init();
  publisher.PublishStream();

  rte::Error err;

  rte::Observer observer;
  rte::Rte rte;

  rte::Config rteConfig;
  rteConfig.SetAppId(app_id.c_str(), nullptr);
  rteConfig.SetUseStringUid(false);

  rteConfig.SetJsonParameter("{\"rtc.vocs_list\":[\"183.131.160.208\"]}", &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.SetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.GetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.RegisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> initResult = std::make_shared<AsyncResult<bool>>();
    rte.InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            initResult->SetResult(true);
          } else {
            initResult->SetResult(false);
          }
        },
        nullptr);

    auto result = initResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // set canvas to render video track
  rte::CanvasInitialConfig canvas_initial_config;
  rte::Canvas canvas(&rte);
  rte::ViewConfig viewConfig;

#if defined(FEATURE_VIDEO)
#if defined(WEBRTC_WIN) || defined(WEBRTC_ANDROID) || defined(WEBRTC_MAC) || defined(WEBRTC_IOS)

  auto win_window = std::make_unique<SimpleWindow>("SimpleWindow");

  rte::View view = reinterpret_cast<rte::View>(win_window->GetView());

  canvas.AddView(&view, &viewConfig, &err);

  rte::CanvasConfig canvasConfig;
  canvasConfig.SetRenderMode(rte::VideoRenderMode::kRteVideoRenderModeFit);
  canvas.SetConfigs(&canvasConfig, &err);

  ASSERT_EQ(err.Code(), kRteOk);

#endif
#endif

  rte::LocalUser localUser(&rte);
  LocalUserObserver localUserObserver;

  rte::LocalUserConfig localUserConfig;
  localUserConfig.SetUserId(subscriber_user_id);

  std::string token =
      agora::tools::RtmTokenBuilder2::BuildToken(app_id, app_cert, subscriber_user_id, 600);
  localUserConfig.SetUserToken(token);

  localUser.SetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.GetConfigs(&localUserConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  localUser.RegisterObserver(&localUserObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> connectResult = std::make_shared<AsyncResult<bool>>();
    localUser.Connect([connectResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        connectResult->SetResult(true);
      } else {
        connectResult->SetResult(false);
      }
    });

    auto result = connectResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  rte::Channel channel(&rte);
  ChannelObserver channelObserver;

  const int kLongTokenExpireTimeInSeconds = 3600 * 24;
  const std::string channel_token =
      genAccessToken2WithStringUid(app_id, app_cert, channel_id, subscriber_user_id,
                                   kLongTokenExpireTimeInSeconds, kLongTokenExpireTimeInSeconds);
  EXPECT_FALSE(channel_token.empty());

  rte::ChannelConfig channelConfig;
  channelConfig.SetChannelId(channel_id);
  channelConfig.SetAllowAutoSubscribeAll(false);
  channelConfig.SetChannelToken(channel_token, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  channel.SetConfigs(&channelConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  channel.RegisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);


  {
    channel.Join(&localUser, &err);
    ASSERT_EQ(err.Code(), kRteOk);
    {
      auto result = channelObserver.has_subscribe_user_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.subscribe_user_presence_error_.Code(), kRteOk);
    }

    {
      auto result = channelObserver.has_subscribe_stream_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.subscribe_stream_presence_error_.Code(), kRteOk);
    }
  }

  // wait for remote stream added
  {
    auto result = channelObserver.has_remote_stream_added_.Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  size_t remote_streams_count = channel.GetRemoteStreamsCount(&err);

  std::vector<rte::RemoteStream> remote_streams;
  channel.GetRemoteStreams(remote_streams, 0, remote_streams_count, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // subscribe per remote stream
  for (auto& remote_stream : remote_streams) {
    rte::RemoteStreamInfo remote_stream_info;
    remote_stream.GetInfo(&remote_stream_info, &err);
    std::string remote_stream_id = remote_stream_info.GetStreamId();
    bool has_audio = remote_stream_info.HasAudio();
    bool has_video = remote_stream_info.HasVideo();

    has_audio = true;
    has_video = true;

    // subscribe audio track
    if (has_audio) {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);

      std::shared_ptr<AsyncResult<bool>> subscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.SubscribeTrack(remote_stream_id, &subscribeOption,
                             [=](rte::Track* track, rte::Error* err) {
                               if (err && err->Code() == kRteOk) {
                                 subscribeResult->SetResult(true);
                               } else {
                                 subscribeResult->SetResult(false);
                               }
                             });

      auto result = subscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    // subscribe video track
    if (has_video) {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);

      std::shared_ptr<rte::Track> subscribe_track;
      std::shared_ptr<AsyncResult<bool>> subscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.SubscribeTrack(
          remote_stream_id, &subscribeOption,
          [=, &subscribe_track](rte::Track* track, rte::Error* err) {
            if (err && err->Code() == kRteOk) {
              subscribe_track = std::make_shared<rte::Track>(track->get_underlying_impl()->handle);
              subscribeResult->SetResult(true);
            } else {
              subscribeResult->SetResult(false);
            }
          });

      auto result = subscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);

      rte::VideoTrack video_track(subscribe_track->get_underlying_impl()->handle);

      video_track.SetCanvas(&canvas,
                            rte::VideoPipelinePosition::kRteVideoPipelinePositionRemotePreRenderer,
                            [=](rte::Error* err) { ASSERT_EQ(err->Code(), kRteOk); });
    }
  }

  // wait for some time

  std::this_thread::sleep_for(std::chrono::seconds(600));

  for (auto& remote_stream : remote_streams) {
    // unsubscribe audio track
    {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeAudio);

      std::shared_ptr<AsyncResult<bool>> unsubscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.UnsubscribeTrack(remote_stream_id, &subscribeOption, [=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          unsubscribeResult->SetResult(true);
        } else {
          unsubscribeResult->SetResult(false);
        }
      });

      auto result = unsubscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }

    // unsubscribe video track
    {
      rte::SubscribeOptions subscribeOption;
      subscribeOption.SetTrackMediaType(rte::TrackMediaType::kRteTrackMediaTypeVideo);

      std::shared_ptr<AsyncResult<bool>> unsubscribeResult = std::make_shared<AsyncResult<bool>>();

      rte::RemoteStreamInfo remote_stream_info;
      remote_stream.GetInfo(&remote_stream_info, &err);
      std::string remote_stream_id = remote_stream_info.GetStreamId();

      channel.UnsubscribeTrack(remote_stream_id, &subscribeOption, [=](rte::Error* err) {
        if (err && err->Code() == kRteOk) {
          unsubscribeResult->SetResult(true);
        } else {
          unsubscribeResult->SetResult(false);
        }
      });

      auto result = unsubscribeResult->Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
    }
  }

  {
    channel.Leave(&err);

    ASSERT_EQ(err.Code(), kRteOk);

    {
      auto result = channelObserver.has_unsubscribe_user_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.unsubscribe_user_presence_error_.Code(), kRteOk);
    }

    {
      auto result = channelObserver.has_unsubscribe_stream_presence_result_.Wait(WaitTimeOutMs);
      ASSERT_EQ(result.valid, true);
      ASSERT_EQ(result.value, true);
      ASSERT_EQ(channelObserver.unsubscribe_stream_presence_error_.Code(), kRteOk);
    }
  }

  channel.UnregisterObserver(&channelObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    // local user logout
    std::shared_ptr<AsyncResult<bool>> disconnectResult = std::make_shared<AsyncResult<bool>>();
    localUser.Disconnect([disconnectResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        disconnectResult->SetResult(true);
      } else {
        disconnectResult->SetResult(false);
      }
    });

    auto result = disconnectResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  // local user unregister observer
  localUser.UnregisterObserver(&localUserObserver, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  // unregister observer
  rte.UnregisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.Destroy();

  publisher.UnpublishStream();
  publisher.Deinit();
}

ATEST(RteApiCppTestChannel, RteChannelWithoutJoinTest) {
  rte::Error err;

  rte::Observer observer;
  rte::Rte rte;

  rte::Config rteConfig;
  rteConfig.SetAppId(API_CALL_APPID_LIVE, nullptr);

  rte.SetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.GetConfigs(&rteConfig, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.RegisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  {
    std::shared_ptr<AsyncResult<bool>> initResult = std::make_shared<AsyncResult<bool>>();
    rte.InitMediaEngine(
        [=](rte::Error* err) {
          if (err && err->Code() == kRteOk) {
            initResult->SetResult(true);
          } else {
            initResult->SetResult(false);
          }
        },
        nullptr);

    auto result = initResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, true);
  }

  rte::Channel channel(&rte);

  // publish message
  {
    std::shared_ptr<AsyncResult<bool>> publishResult = std::make_shared<AsyncResult<bool>>();
    const char* data = "Hello World!";
    rte::Buffer buffer(data, strlen(data));
    channel.PublishMessage(&buffer, [publishResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        publishResult->SetResult(true);
      } else {
        publishResult->SetResult(false);
      }
    });

    auto result = publishResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, false);
  }

  // get metadata
  {
    std::shared_ptr<AsyncResult<bool>> getMetadataResult = std::make_shared<AsyncResult<bool>>();
    channel.GetMetadata([getMetadataResult](rte::Metadata* metadata, rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        getMetadataResult->SetResult(true);
      } else {
        getMetadataResult->SetResult(false);
      }
    });

    auto result = getMetadataResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, false);
  }

  // set metadata
  {
    std::shared_ptr<AsyncResult<bool>> setMetadataResult = std::make_shared<AsyncResult<bool>>();
    rte::Metadata metadata;
    rte::MetadataItem metadataItem;
    metadataItem.SetKey("key");
    metadataItem.SetValue("value");
    metadata.AddItem(&metadataItem);

    channel.SetMetadata(&metadata, nullptr, [setMetadataResult](rte::Error* err) {
      if (err && err->Code() == kRteOk) {
        setMetadataResult->SetResult(true);
      } else {
        setMetadataResult->SetResult(false);
      }
    });

    auto result = setMetadataResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, false);
  }

  // get user state
  {
    std::shared_ptr<AsyncResult<bool>> getUserStateResult = std::make_shared<AsyncResult<bool>>();
    channel.GetUserState(kTestLocalUserId,
                         [getUserStateResult](rte::PresenceState* state, rte::Error* err) {
                           if (err && err->Code() == kRteOk) {
                             getUserStateResult->SetResult(true);
                           } else {
                             getUserStateResult->SetResult(false);
                           }
                         });

    auto result = getUserStateResult->Wait(WaitTimeOutMs);
    ASSERT_EQ(result.valid, true);
    ASSERT_EQ(result.value, false);
  }

  rte.UnregisterObserver(&observer, &err);
  ASSERT_EQ(err.Code(), kRteOk);

  rte.Destroy();
}

ATEST(RteApiCppTestChannel, RteUserInteractionTest) {
  RteUserAndChannelTester local_user_and_channel_test;

  local_user_and_channel_test.Init();

  std::this_thread::sleep_for(std::chrono::seconds(2));

  local_user_and_channel_test.LocalUserPublishMessage();
  local_user_and_channel_test.RemoteUserReceiveMessageCheck();

  local_user_and_channel_test.LocalUserSetMetadata("key", "value");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteUserSubscribeMetadata();
  local_user_and_channel_test.RemoteUserCheckMetadataSnapshot("key", "value");
  local_user_and_channel_test.RemoteUserGetMetadata("key", "value");
  local_user_and_channel_test.LocalUserRemoveMetadata("key", "value");

  local_user_and_channel_test.LocalUserSetMetadata("key", "value2");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteUserCheckMetadataChanged("key", "value2");

  local_user_and_channel_test.LocalUserSetMetadata("key", "value3");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteUserCheckMetadataChanged("key", "value3");

  local_user_and_channel_test.RemoteUserUnsubscribeMetadata();

  local_user_and_channel_test.LocalChannelCheckRemoteUserPresence();
  local_user_and_channel_test.RemoteChannelCheckLocalUserPresence();

  local_user_and_channel_test.LocalChannelPublishMessage("Hello World!");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteChannelReceiveMessageCheck("Hello World!");

  local_user_and_channel_test.LocalChannelSetUserState("user_state_key", "user_state_value");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteChannelReceiveUserStateCheck("user_state_key",
                                                                 "user_state_value");
  local_user_and_channel_test.RemoteChannelGetUserState("user_state_key", "user_state_value");
  local_user_and_channel_test.LocalChannelRemoveUserState("user_state_key");

  local_user_and_channel_test.LocalChannelSetMetadata("metadata_key", "metadata_value");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  local_user_and_channel_test.RemoteChannelReceiveMetadataChangedCheck("metadata_key",
                                                                       "metadata_value");
  local_user_and_channel_test.RemoteChannelGetMetadata("metadata_key", "metadata_value");
  local_user_and_channel_test.LocalChannelRemoveMetadata("metadata_key", "metadata_value");

  local_user_and_channel_test.Deinit();
}

ATEST(RteApiCppTestChannel, RteChannelConfigTest) {
  rte::ChannelConfig channelConfig;
  channelConfig.SetChannelId(kTestChannelId);
  ASSERT_EQ(channelConfig.GetChannelId(), kTestChannelId);

  channelConfig.SetAutoSubscribeMessage(true);
  ASSERT_EQ(channelConfig.GetAutoSubscribeMessage(), true);

  channelConfig.SetAutoSubscribeMetadata(true);
  ASSERT_EQ(channelConfig.GetAutoSubscribeMetadata(), true);

  channelConfig.SetAutoSubscribeUserPresence(true);
  ASSERT_EQ(channelConfig.GetAutoSubscribeUserPresence(), true);

  channelConfig.SetBeQuiet(true);
  ASSERT_EQ(channelConfig.GetBeQuiet(), true);

  channelConfig.SetAutoSubscribeAudio(true);
  ASSERT_EQ(channelConfig.GetAutoSubscribeAudio(), true);

  channelConfig.SetAutoSubscribeVideo(true);
  ASSERT_EQ(channelConfig.GetAutoSubscribeVideo(), true);

  channelConfig.SetJsonParameter("{\"key\":\"value\"}");
  ASSERT_EQ(channelConfig.GetJsonParameter(), "{\"key\":\"value\"}");
}


