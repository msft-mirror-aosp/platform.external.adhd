// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <gtest/gtest.h>

extern "C" {
#include "dev_io.h" // tested
#include "dev_stream.h" // stubbed
#include "cras_rstream.h" // stubbed
#include "cras_iodev.h" // stubbed
#include "cras_shm.h"
#include "cras_types.h"
#include "utlist.h"

struct audio_thread_event_log* atlog;
}

#include "dev_io_stubs.h"
#include "iodev_stub.h"
#include "metrics_stub.h"
#include "rstream_stub.h"

static float dev_stream_capture_software_gain_scaler_val;
static unsigned int dev_stream_capture_avail_ret = 480;

namespace {


class DevIoSuite : public testing::Test{
 protected:
  virtual void SetUp() {
    atlog = static_cast<audio_thread_event_log*>(calloc(1, sizeof(*atlog)));
    iodev_stub_reset();
    rstream_stub_reset();
    fill_audio_format(&format, 48000);
    stream = create_stream(1, 1, CRAS_STREAM_INPUT, cb_threshold, &format);
  }

  virtual void TearDown() {
    free(atlog);
  }

  size_t cb_threshold = 480;
  cras_audio_format format;
  StreamPtr stream;
};

TEST_F(DevIoSuite, SendCapturedFails) {


  // rstream's next callback is now and there is enough data to fill.
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  stream->rstream->next_cb_ts = start;
  AddFakeDataToStream(stream.get(), 480);

  struct open_dev* dev_list = NULL;
  DevicePtr dev = create_device(CRAS_STREAM_INPUT, cb_threshold,
                                &format, CRAS_NODE_TYPE_MIC);
  DL_APPEND(dev_list, dev->odev.get());
  add_stream_to_dev(dev->dev, stream);

  // Set failure response from frames_queued.
  iodev_stub_frames_queued(dev->dev.get(), -3, start);

  EXPECT_EQ(-3, dev_io_send_captured_samples(dev_list));
}

TEST_F(DevIoSuite, CaptureGain) {
  struct open_dev* dev_list = NULL;
  struct timespec ts;
  DevicePtr dev = create_device(CRAS_STREAM_INPUT, cb_threshold,
                                &format, CRAS_NODE_TYPE_MIC);

  dev->dev->state = CRAS_IODEV_STATE_NORMAL_RUN;
  dev->dev->software_gain_scaler = 0.99f;
  iodev_stub_frames_queued(dev->dev.get(), 20, ts);
  DL_APPEND(dev_list, dev->odev.get());
  add_stream_to_dev(dev->dev, stream);

  /* For stream that uses APM, always apply gain scaler 1.0f regardless of
   * what node/stream gains are. */
  stream->rstream->apm_list = reinterpret_cast<struct cras_apm_list *>(0xf0f);
  dev_io_capture(&dev_list);
  EXPECT_EQ(1.0f, dev_stream_capture_software_gain_scaler_val);

  stream->rstream->apm_list = 0x0;
  dev_io_capture(&dev_list);
  EXPECT_EQ(0.99f, dev_stream_capture_software_gain_scaler_val);
}

/* Stubs */
extern "C" {

int input_data_get_for_stream(
		struct input_data *data,
		struct cras_rstream *stream,
		struct buffer_share *offsets,
		struct cras_audio_area **area,
		unsigned int *offset)
{
  return 0;
}

int input_data_put_for_stream(struct input_data *data,
			   struct cras_rstream *stream,
			   struct buffer_share *offsets,
			   unsigned int frames)
{
  return 0;
}
int dev_stream_attached_devs(const struct dev_stream *dev_stream)
{
  return 0;
}
void dev_stream_update_frames(const struct dev_stream *dev_stream)
{
}
int dev_stream_playback_frames(const struct dev_stream *dev_stream)
{
  return 0;
}
int dev_stream_is_pending_reply(const struct dev_stream *dev_stream)
{
  return 0;
}
int dev_stream_mix(struct dev_stream *dev_stream,
       const struct cras_audio_format *fmt,
       uint8_t *dst,
       unsigned int num_to_write)
{
  return 0;
}
void dev_stream_set_dev_rate(struct dev_stream *dev_stream,
           unsigned int dev_rate,
           double dev_rate_ratio,
           double master_rate_ratio,
           int coarse_rate_adjust)
{
}
int dev_stream_capture_update_rstream(struct dev_stream *dev_stream)
{
  return 0;
}
int dev_stream_wake_time(struct dev_stream *dev_stream,
       unsigned int curr_level,
       struct timespec *level_tstamp,
       unsigned int cap_limit,
       int is_cap_limit_stream,
       struct timespec *wake_time_out)
{
  return 0;
}
int dev_stream_flush_old_audio_messages(struct dev_stream *dev_stream)
{
  return 0;
}
void dev_stream_set_delay(const struct dev_stream *dev_stream,
        unsigned int delay_frames)
{
}
unsigned int dev_stream_capture(struct dev_stream *dev_stream,
      const struct cras_audio_area *area,
      unsigned int area_offset,
      float software_gain_scaler)
{
  dev_stream_capture_software_gain_scaler_val = software_gain_scaler;
  return 0;
}
void dev_stream_update_next_wake_time(struct dev_stream *dev_stream)
{
}
int dev_stream_request_playback_samples(struct dev_stream *dev_stream,
          const struct timespec *now)
{
  return 0;
}
int dev_stream_playback_update_rstream(struct dev_stream *dev_stream)
{
  return 0;
}
void dev_stream_destroy(struct dev_stream *dev_stream)
{
}
unsigned int dev_stream_capture_avail(const struct dev_stream *dev_stream)
{
  return dev_stream_capture_avail_ret;
}
}  // extern "C"

}  //  namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}