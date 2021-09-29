// Copyright 2021 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <gtest/gtest.h>

#include "cras_string.h"

namespace {

TEST(String, Strerror) {
  EXPECT_STREQ(cras_strerror(ENOENT), "No such file or directory");
  EXPECT_STREQ(cras_strerror(65536), "Unknown error 65536");
}

}  //  namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}