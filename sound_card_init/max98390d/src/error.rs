// Copyright 2020 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
use std::error;
use std::fmt;
use std::io;
use std::num::ParseIntError;
use std::sync::PoisonError;
use std::time;

use remain::sorted;

pub type Result<T> = std::result::Result<T, Error>;

#[sorted]
#[derive(Debug)]
pub enum Error {
    AlsaCardError(cros_alsa::CardError),
    AlsaControlError(cros_alsa::ControlError),
    CalibrationFailed,
    CalibrationTimeout,
    CrasClientFailed(libcras::Error),
    DeserializationFailed(String, serde_yaml::Error),
    FileIOFailed(String, io::Error),
    HotSpeaker,
    InternalSpeakerNotFound,
    InvalidDatastore,
    InvalidShutDownTime,
    InvalidTemperature(i32),
    LargeCalibrationDiff(i32, i32),
    MissingDSMParam,
    MutexPoisonError,
    NewPlayStreamFailed(libcras::BoxError),
    NextPlaybackBufferFailed(libcras::BoxError),
    PlaybackFailed(io::Error),
    ReadTimestampFailed(utils::error::Error),
    SerializationFailed(serde_yaml::Error),
    StartPlaybackTimeout,
    SystemTimeError(time::SystemTimeError),
    VPDParseFailed(String, ParseIntError),
    WorkerPanics,
}

impl From<cros_alsa::CardError> for Error {
    fn from(err: cros_alsa::CardError) -> Error {
        Error::AlsaCardError(err)
    }
}

impl From<cros_alsa::ControlError> for Error {
    fn from(err: cros_alsa::ControlError) -> Error {
        Error::AlsaControlError(err)
    }
}

impl<T> From<PoisonError<T>> for Error {
    fn from(_: PoisonError<T>) -> Error {
        Error::MutexPoisonError
    }
}

impl error::Error for Error {}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use Error::*;
        match self {
            AlsaCardError(e) => write!(f, "{}", e),
            AlsaControlError(e) => write!(f, "{}", e),
            CalibrationFailed => write!(f, "amp calibration failed"),
            CalibrationTimeout => write!(f, "calibration is not finished in time"),
            CrasClientFailed(e) => write!(f, "failed to create cras client: {}", e),
            DeserializationFailed(file, e) => write!(f, "failed to parse {}: {}", file, e),
            FileIOFailed(file, e) => write!(f, "{}: {}", file, e),
            InvalidShutDownTime => write!(f, "invalid shutdown time"),
            InternalSpeakerNotFound => write!(f, "internal speaker is not found in cras"),
            InvalidTemperature(temp) => write!(
                f,
                "invalid calibration temperature: {}, and there is no datastore",
                temp
            ),
            InvalidDatastore => write!(f, "invalid datastore format"),
            HotSpeaker => write!(f, "skip boot time calibration as the speakers may be hot"),
            LargeCalibrationDiff(rdc, temp) => write!(
                f,
                "calibration difference is too large, rdc: {}, temp: {}",
                rdc, temp
            ),
            MissingDSMParam => write!(f, "missing dsm_param.bin"),
            MutexPoisonError => write!(f, "mutex is poisoned"),
            NewPlayStreamFailed(e) => write!(f, "{}", e),
            NextPlaybackBufferFailed(e) => write!(f, "{}", e),
            PlaybackFailed(e) => write!(f, "{}", e),
            ReadTimestampFailed(e) => write!(f, "{}", e),
            SerializationFailed(e) => write!(f, "failed to serialize yaml: {}", e),
            StartPlaybackTimeout => write!(f, "playback is not started in time"),
            SystemTimeError(e) => write!(f, "{}", e),
            VPDParseFailed(file, e) => write!(f, "failed to parse vpd {}: {}", file, e),
            WorkerPanics => write!(f, "run_play_zero_worker panics"),
        }
    }
}
