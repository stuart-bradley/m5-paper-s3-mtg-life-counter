# MTG Life Counter for M5Paper S3

## Project Overview
A Magic: The Gathering life counter application for the M5Paper S3 e-ink device.

## Hardware
- **Device**: M5Paper S3
- **MCU**: ESP32-S3R8 (dual-core 240MHz, 8MB PSRAM, 16MB Flash)
- **Display**: 4.7" e-ink, 960×540 pixels, 16 grayscale levels
- **Touch**: GT911 capacitive, 2-point touch + gestures
- **Other**: BMI270 IMU, BM8563 RTC, buzzer, 1800mAh battery

## Tech Stack
- **Framework**: Arduino via PlatformIO
- **Libraries**: M5Unified, M5GFX
- **Language**: C++17

## Build Commands
```bash
pio run                    # Compile
pio run -t upload          # Flash to device
pio device monitor         # Serial monitor
pio run -t upload && pio device monitor  # Flash and monitor
```

## Device Connection
1. Connect M5Paper S3 via USB-C
2. Long-press power button until back LED blinks red (download mode)
3. Upload should auto-detect port

## Display Considerations
- E-ink has slow refresh (~300ms full, ~100ms partial)
- Minimize full refreshes - use partial updates for life changes
- M5GFX handles this via update modes
- Screen is 960×540 in landscape (default rotation 1)

## Touch Handling
```cpp
M5.update();  // Call in loop
if (M5.Touch.getCount() > 0) {
    auto touch = M5.Touch.getDetail();
    int x = touch.x;
    int y = touch.y;
    bool pressed = touch.isPressed();
    bool released = touch.wasReleased();
}
```

## Architecture
- `src/main.cpp` - Entry point, setup/loop
- `src/app/` - Application logic, state management
- `src/ui/` - Screen classes and UI components
- `src/models/` - Data structures (Player, GameState)
- `src/utils/` - Storage, helpers

## Features to Implement
1. **Player Management**: 2-6 players, configurable
2. **Life Tracking**: Starting life (20/40), +/- buttons, tap to edit
3. **Name Editing**: On-screen keyboard or character picker
4. **Persistence**: Save game state to NVS (Preferences library)
5. **Reset/New Game**: Confirm dialog, reset to starting life

## UI Guidelines
- Large touch targets (min 80×80 pixels)
- High contrast for e-ink readability
- Life totals should be LARGE (primary info)
- Player names smaller but readable
- Use partial refresh for life changes, full refresh for screen transitions

## Code Style
- Use `#pragma once` for headers
- Prefer `std::string` over C strings where practical
- Use `enum class` for state machines
- Keep functions small and focused
- Comment complex e-ink refresh logic