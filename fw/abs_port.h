// Copyright 2021 Josh Pieper, jjp@pobox.com.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "PinNames.h"

#include "mjlib/micro/persistent_config.h"
#include "mjlib/micro/pool_ptr.h"
#include "mjlib/micro/telemetry_manager.h"

#include "fw/millisecond_timer.h"

namespace moteus {

/// Manages the ABS port on the moteus controller.
class AbsPort {
 public:
  struct Options {
    PinName scl = NC;
    PinName sda = NC;
  };

  AbsPort(mjlib::micro::Pool*,
          mjlib::micro::PersistentConfig* persistent_config,
          mjlib::micro::TelemetryManager* telemetry_manager,
          MillisecondTimer* timer,
          const Options&);
  ~AbsPort();

  void PollMillisecond();
  void Poll();

  enum Mode {
    kDisabled = 0,
    kAs5048 = 1,
    kNumModes = 2,
  };

  struct Config {
    Mode mode = kDisabled;

    uint8_t encoder_i2c_address = 0x40;
    int32_t encoder_poll_ms = 10;

    template <typename Archive>
    void Serialize(Archive* a) {
      a->Visit(MJ_NVP(mode));
      a->Visit(MJ_NVP(encoder_i2c_address));
      a->Visit(MJ_NVP(encoder_poll_ms));
    }
  };

  struct Status {
    uint16_t encoder_raw = 0;
    bool encoder_valid = false;

    uint8_t as5048_agc = 0;
    uint8_t as5048_diag = 0;
    uint16_t as5048_mag = 0;
    uint32_t as5048_error_count = 0;

    template <typename Archive>
    void Serialize(Archive* a) {
      a->Visit(MJ_NVP(encoder_raw));
      a->Visit(MJ_NVP(encoder_valid));
      a->Visit(MJ_NVP(as5048_agc));
      a->Visit(MJ_NVP(as5048_diag));
      a->Visit(MJ_NVP(as5048_mag));
      a->Visit(MJ_NVP(as5048_error_count));
    }
  };

 private:
  class Impl;
  mjlib::micro::PoolPtr<Impl> impl_;
};
}

namespace mjlib {
namespace base {

template <>
struct IsEnum<moteus::AbsPort::Mode> {
  static constexpr bool value = true;

  using M = moteus::AbsPort::Mode;
  static std::array<std::pair<M, const char*>,
                    static_cast<int>(M::kNumModes)> map() {
    return { {
        { M::kDisabled, "disabled" },
        { M::kAs5048, "as5048" },
    } };
  }
};

}
}
