// Copyright 2021 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

/*
 * Generates a vector of float samples with given |size| and random
 * |engine|.
 */
std::vector<float> gen_float_samples(size_t size, std::mt19937& engine);

/*
 * Generates a vector of int16_t samples with given |size| and random
 * |engine|.
 */
std::vector<int16_t> gen_s16_le_samples(size_t size, std::mt19937& engine);