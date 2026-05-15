//
// Created by awalol on 2026/5/15.
//

#include <cstddef>
#include <cstring>

#include "utils.h"

namespace {
    constexpr size_t kAudioControlOffset = offsetof(SetStateData, MuteLightMode) - sizeof(uint8_t);
    constexpr size_t kMuteControlOffset = offsetof(SetStateData, RightTriggerFFB) - sizeof(uint8_t);
    constexpr size_t kMotorPowerLevelOffset = offsetof(SetStateData, HostTimestamp) + sizeof(uint32_t);
    constexpr size_t kAudioControl2Offset = kMotorPowerLevelOffset + sizeof(uint8_t);
    constexpr size_t kHapticLowPassFilterOffset = offsetof(SetStateData, LightFadeAnimation) - 2 * sizeof(uint8_t);
    constexpr size_t kPlayerIndicatorsOffset = offsetof(SetStateData, LedRed) - sizeof(uint8_t);
}

static constexpr uint8_t state_init_data[63] = {
    0xfd, 0xf7, 0x0, 0x0,
    0x7f, 0x7f, // Headphones, Speaker
    0xff, 0x9, 0x0, 0x0F, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa,
    0x7, 0x0, 0x0, 0x2, 0x1,
    0x00,
    0xff, 0xd7, 0x00 // RGB LED: R, G, B (Nijika Color!)✨
};

uint8_t state[63]{};

void state_init() {
    memcpy(state, state_init_data, sizeof(state));
}

void state_set(uint8_t *data, const uint8_t size) {
    if (size > 63) {
        printf("[StateMgr] Warning: State Set over 63 bytes\n");
    }
    memcpy(data, state, size);
}

void state_update(const uint8_t *data, const uint8_t size) {
    if (size < sizeof(SetStateData)) {
        printf(
            "[StateMgr] Error: SetStateData at least %u bytes\n",
            static_cast<unsigned>(sizeof(SetStateData))
        );
        return;
    }

    SetStateData update{};
    memcpy(&update, data, sizeof(update));

    const auto copy_if_allowed = [&](const bool allowed, const size_t offset, const size_t length) {
        if (allowed) {
            memcpy(state + offset, data + offset, length);
        }
    };

    const auto copy_field = [&](const bool allowed, const size_t offset, const auto &field) {
        copy_if_allowed(allowed, offset, sizeof(field));
    };

    /*copy_if_allowed(
        update.EnableRumbleEmulation || update.UseRumbleNotHaptics,
        offsetof(SetStateData, RumbleEmulationRight),
        2
    );*/
    // copy_field(update.AllowHeadphoneVolume, offsetof(SetStateData, VolumeHeadphones), update.VolumeHeadphones);
    // copy_field(update.AllowSpeakerVolume, offsetof(SetStateData, VolumeSpeaker), update.VolumeSpeaker);
    // copy_field(update.AllowMicVolume, offsetof(SetStateData, VolumeMic), update.VolumeMic);
    // copy_if_allowed(update.AllowAudioControl, kAudioControlOffset, sizeof(uint8_t));
    copy_field(update.AllowMuteLight, offsetof(SetStateData, MuteLightMode), update.MuteLightMode);
    // copy_if_allowed(update.AllowAudioMute, kMuteControlOffset, sizeof(uint8_t));
    copy_field(update.AllowRightTriggerFFB, offsetof(SetStateData, RightTriggerFFB), update.RightTriggerFFB);
    copy_field(update.AllowLeftTriggerFFB, offsetof(SetStateData, LeftTriggerFFB), update.LeftTriggerFFB);
    // copy_if_allowed(update.AllowMotorPowerLevel, kMotorPowerLevelOffset, sizeof(uint8_t));
    // copy_if_allowed(update.AllowAudioControl2, kAudioControl2Offset, sizeof(uint8_t));
    // copy_if_allowed(update.AllowHapticLowPassFilter, kHapticLowPassFilterOffset, sizeof(uint8_t));
    copy_field(update.AllowColorLightFadeAnimation, offsetof(SetStateData, LightFadeAnimation), update.LightFadeAnimation);
    copy_field(update.AllowLightBrightnessChange, offsetof(SetStateData, LightBrightness), update.LightBrightness);
    copy_if_allowed(update.AllowPlayerIndicators, kPlayerIndicatorsOffset, sizeof(uint8_t));
    copy_if_allowed(update.AllowLedColor, offsetof(SetStateData, LedRed), sizeof(update.LedRed) * 3);
}
