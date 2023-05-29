// Copyright 2023 The ChromiumOS Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Syntax:
// DEFINE_FEATURE(name, default_enabled)
DEFINE_FEATURE(CrOSLateBootAudioTestFeatureFlag, false)
DEFINE_FEATURE(CrOSLateBootAudioHFPOffload, false)
DEFINE_FEATURE(CrOSLateBootAudioHFPMicSR, false)
DEFINE_FEATURE(CrOSLateBootAudioFlexibleLoopback, false)
DEFINE_FEATURE(CrOSLateBootAudioAPNoiseCancellation, false)
DEFINE_FEATURE(CrOSLateBootCrasSplitAlsaUSBInternal, true)