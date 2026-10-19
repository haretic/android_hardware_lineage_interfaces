/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "powerhal-libperfmgr"

#include "HeldHints.h"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <perfmgr/HintManager.h>
#include <sys/system_properties.h>

#include <set>
#include <string>

namespace aidl {
namespace google {
namespace hardware {
namespace power {
namespace impl {
namespace pixel {

using ::android::perfmgr::HintManager;

constexpr char kHeldHintsProp[] = "sys.powerhal.held_hints";

static std::set<std::string> ReadHeldHints() {
    std::set<std::string> hints;
    for (const auto &hint :
         ::android::base::Split(::android::base::GetProperty(kHeldHintsProp, ""), ",")) {
        const std::string name = ::android::base::Trim(hint);
        if (name.empty()) {
            continue;
        }
        if (!HintManager::GetInstance()->IsHintSupported(name)) {
            LOG(WARNING) << kHeldHintsProp << ": no hint " << name;
            continue;
        }
        hints.insert(name);
    }
    return hints;
}

void WatchHeldHints() {
    // Only set once something asks for a hint
    ::android::base::WaitForPropertyCreation(kHeldHintsProp);
    const prop_info *info = __system_property_find(kHeldHintsProp);
    if (info == nullptr) {
        LOG(ERROR) << kHeldHintsProp << " vanished";
        return;
    }
    std::set<std::string> held;
    uint32_t serial = 0;
    for (;;) {
        // Read after taking the serial, so a change in between is seen on the next round
        const uint32_t current = __system_property_serial(info);
        const std::set<std::string> wanted = ReadHeldHints();
        for (const auto &hint : held) {
            if (!wanted.count(hint)) {
                HintManager::GetInstance()->EndHint(hint);
                LOG(INFO) << "Released held hint " << hint;
            }
        }
        for (const auto &hint : wanted) {
            if (!held.count(hint)) {
                HintManager::GetInstance()->DoHint(hint);
                LOG(INFO) << "Holding hint " << hint;
            }
        }
        held = wanted;
        serial = current;
        __system_property_wait(info, serial, &serial, nullptr);
    }
}

}  // namespace pixel
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace google
}  // namespace aidl
