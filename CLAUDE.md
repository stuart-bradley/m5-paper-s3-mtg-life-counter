# M5Paper S3 App Platform

## Project Overview
An app platform for the M5Paper S3 e-ink device featuring a home screen launcher, system settings, and a Magic: The Gathering life counter application.

## Issue Tracking
Use Claude Code's built-in Task system for issue tracking:
- `TaskCreate` to file new tasks with enough context for future sessions
- `TaskList` to see available work
- `TaskUpdate` to mark progress (in_progress, completed)
- `TaskGet` to read task details

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

**Note for Claude**: You cannot upload to the device directly. When you need to upload, pause and ask the user to run the upload command. The user will report back results from the serial monitor.

## Display Considerations
- E-ink has slow refresh (~300ms full, ~100ms partial) but `display()` is async
- `M5.Display.display()` returns immediately - refresh happens in background
- Minimize full refreshes - use partial updates for life changes
- M5GFX handles this via update modes: `epd_quality` (full), `epd_fastest` (partial)
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
- **Player Card (LIFE mode)**: Name header, large life total, -5/-1/+1/+5 buttons, pending delta indicator (5s timeout)
- **Player Card (CMDR mode)**: Compact name header (28px), opponent rows with damage value and -1/+1 buttons, lethal indicator (row inverts at 21+ damage)
- **Header Toggle**: LIFE/CMDR toggle right-justified next to SETTINGS button. ViewMode stored on GameState (not persisted to NVS — always starts in LIFE mode)
- **Commander Damage**: 2D matrix `commanderDamage[6][6]` where `[target][source]` tracks damage from each opponent's commander. Persisted to NVS with `"cd01"`-style keys
- **Settings View**: Player count (2-6), starting life (20/25/30/40), player names, reset options
- **Reset Options**:
  - Reset Life Totals: Reset life and commander damage to 0
  - New Game: Reset everything to defaults (2 players, default names, zero commander damage)

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
1. **Player Management**: 2-6 players, configurable names (tap name to edit via on-screen keyboard)
2. **Life Tracking**: Starting life (20/25/30/40), +/- 1 and +/- 5 buttons
3. **Commander Damage**: Per-opponent damage tracking via CMDR mode toggle, lethal at 21+
4. **Pending Delta Indicator**: Shows cumulative life/damage change for 5 seconds after button press
5. **Reset Options**: Life-only reset (includes commander damage) vs full game reset
6. **Large Display**: Life totals readable from distance
7. **Auto-save**: Periodic save to NVS every 5 seconds

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