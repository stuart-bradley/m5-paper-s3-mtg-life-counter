# M5Paper S3 App Platform

## Project Overview
An app platform for the M5Paper S3 e-ink device featuring a home screen launcher, system settings, and a Magic: The Gathering life counter application.

## Issue Tracking
Use **beads** (`bd` command) for issue tracking. Key commands:
```bash
bd ready              # Show issues ready to work on
bd list --status=open # All open issues
bd create --title="..." --type=task|bug|feature --priority=2
bd update <id> --status=in_progress
bd close <id>
bd sync               # Sync with git remote
```

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

## UI Design

**Design Reference**: `ui-mockup/index.html` - Interactive HTML prototype (pixel-accurate to device)

### Visual Style: UNIX/ASCII Hacker Aesthetic
- Monospace fonts throughout (IBM Plex Mono style)
- Box-drawing characters for borders and frames
- Pixel art icons on home screen
- High contrast black on light gray
- Terminal-inspired UI patterns
- 16-level grayscale palette only

### Screen Structure

**System Toolbar** (32px height, top of every screen)
- Left: Device identifier `[M5] PAPER-S3`
- Right: WiFi indicator `((●))`, Sound `♪`, Battery `[████░░] 67%`, Time

**Home Screen** (App Launcher)
- Grid of 180×180px app icons with pixel art graphics black filled
- Current apps: Settings (6 spoke gear icon), MTG Life (mtg logo)
- Tap to launch, ESC/back to return

### Apps

**Settings App**
- WiFi connection (network list with signal strength)
- Sound toggle (system sounds, button feedback)
- Display settings (auto refresh, refresh rate)
- About device info

**MTG Life Counter App**
- **Main View**: Player cards in grid layout
  - 2 players: 1×2 horizontal
  - 3 players: 1×3 horizontal
  - 4 players: 2×2 grid
  - 5 players: 3+2 layout
  - 6 players: 2×3 grid
- **Player Card**: Name header, large life total (VT323 font), -5/-1/+1/+5 buttons
- **Settings View**: Player count (2-6), starting life (20/25/30/40), player names, reset options
- **Reset Options**:
  - Reset Life Totals: Keep players, reset to starting life
  - New Game: Reset everything to defaults (2 players, default names)

### Layout Specifications
- Screen: 960×540 pixels
- Toolbar: 32px height
- Touch targets: Minimum 44×36px (life buttons), 80×80px preferred (app icons)
- Life totals: 120px font (2-4 players), 80px font (5-6 players)
- Margins: 8-20px depending on context

## Features

### System Features
1. **App Launcher**: Home screen with icon grid
2. **System Toolbar**: WiFi, battery, sound status, clock
3. **Settings App**: WiFi, sound, display configuration
4. **Persistence**: Save state to NVS (Preferences library)

### MTG Life Counter Features
1. **Player Management**: 2-6 players, configurable names
2. **Life Tracking**: Starting life (20/25/30/40), +/- 1 and +/- 5 buttons
3. **Reset Options**: Life-only reset vs full game reset
4. **Large Display**: Life totals readable from distance

## UI Guidelines
- Large touch targets (min 44px, prefer 80px)
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