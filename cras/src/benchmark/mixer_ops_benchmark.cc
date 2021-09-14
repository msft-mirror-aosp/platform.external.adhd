// Copyright 2021 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include "benchmark_util.h"

namespace {
extern "C" {
#include "src/server/cras_mix_ops.h"
}

static void BM_CrasMixerOpsScaleBuffer(benchmark::State& state) {
  std::random_device rnd_device;
  std::mt19937 engine{rnd_device()};
  std::vector<int16_t> samples = gen_s16_le_samples(state.range(0), engine);
  std::uniform_real_distribution<double> distribution(0.5, 2);
  for (auto _ : state) {
    double scale = distribution(engine);
    mixer_ops.scale_buffer(SND_PCM_FORMAT_S16_LE, (uint8_t*)(samples.data()),
                           state.range(0), scale);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)) * /*bytes per sample=*/2);
}

BENCHMARK(BM_CrasMixerOpsScaleBuffer)->RangeMultiplier(2)->Range(256, 8 << 10);

static void BM_CrasMixerOpsMixAdd(benchmark::State& state) {
  std::random_device rnd_device;
  std::mt19937 engine{rnd_device()};
  std::vector<int16_t> src = gen_s16_le_samples(state.range(0), engine);
  std::vector<int16_t> dst = gen_s16_le_samples(state.range(0), engine);
  std::uniform_real_distribution<double> distribution(0.5, 2);
  for (auto _ : state) {
    double scale = distribution(engine);
    mixer_ops.add(SND_PCM_FORMAT_S16_LE, (uint8_t*)dst.data(),
                  (uint8_t*)src.data(), state.range(0), 0, 0, scale);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0) * /*bytes per sample=*/2));
}

BENCHMARK(BM_CrasMixerOpsMixAdd)->RangeMultiplier(2)->Range(256, 8 << 10);
}  // namespace
