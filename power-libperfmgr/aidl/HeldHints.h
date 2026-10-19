/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace aidl {
namespace google {
namespace hardware {
namespace power {
namespace impl {
namespace pixel {

// Holds the hints listed in sys.powerhal.held_hints (comma separated) until they leave the
// list, e.g. a performance profile picked in Settings. Blocks; run it on its own thread
// once the HintManager has started.
void WatchHeldHints();

}  // namespace pixel
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace google
}  // namespace aidl
