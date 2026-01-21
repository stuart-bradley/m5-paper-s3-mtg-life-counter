# Development Guide

This guide covers the architecture and development workflow for the M5Paper S3 app platform.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                            │
│  - App registration                                         │
│  - Main loop: touch handling, update, draw                  │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                       Navigation                            │
│  - App launching/switching                                  │
│  - Screen stack management                                  │
│  - State persistence                                        │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                          App                                │
│  - Owns screens and state                                   │
│  - Lifecycle: onLaunch(), onSuspend()                       │
│  - Provides screens via getMainScreen(), getScreen()        │
└───────────────────────────┬─────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────┐
│                        Screen                               │
│  - Lifecycle: onEnter(), onExit()                           │
│  - Frame: update(), draw()                                  │
│  - Input: handleTouch()                                     │
└─────────────────────────────────────────────────────────────┘
```

### Key Concepts

- **App**: Contains related screens and shared state. Apps are long-lived singletons registered at startup.
- **Screen**: A single view within an app. Screens are owned by their App and can be pushed/popped via Navigation.
- **Navigation**: Singleton managing the active app and screen stack. Handles back navigation and state persistence.
- **AppRegistry**: Maintains list of all registered apps for the home screen launcher.

## Creating a New App

### 1. Create the App Directory

```
src/apps/myapp/
├── MyApp.hpp
├── MyApp.cpp
├── MyScreen.hpp
└── MyScreen.cpp
```

### 2. Define the App Class

```cpp
// src/apps/myapp/MyApp.hpp
#pragma once

#include "../../app/App.hpp"
#include "../../assets/icons.hpp"
#include "MyScreen.hpp"

class MyApp : public App {
public:
    MyApp();

    const AppMetadata& metadata() const override { return _metadata; }

    void onLaunch() override;
    void onSuspend() override;

    Screen* getMainScreen() override { return &_mainScreen; }

    // App-specific state accessors
    int& counter() { return _counter; }

private:
    static constexpr AppMetadata _metadata = {
        "myapp",          // id - unique identifier
        "My App",         // name - shown in launcher
        ICON_MYAPP,       // icon - 64x64 bitmap
        true              // showInLauncher
    };

    int _counter = 0;
    MyScreen _mainScreen;
};
```

```cpp
// src/apps/myapp/MyApp.cpp
#include "MyApp.hpp"
#include <Preferences.h>

MyApp::MyApp() : _mainScreen(this) {}

void MyApp::onLaunch() {
    // Load state from NVS
    Preferences prefs;
    if (prefs.begin("myapp", true)) {
        _counter = prefs.getInt("counter", 0);
        prefs.end();
    }
}

void MyApp::onSuspend() {
    // Save state to NVS
    Preferences prefs;
    if (prefs.begin("myapp", false)) {
        prefs.putInt("counter", _counter);
        prefs.end();
    }
}
```

### 3. Define the Screen Class

```cpp
// src/apps/myapp/MyScreen.hpp
#pragma once

#include "../../ui/HeaderScreen.hpp"

class MyApp;

class MyScreen : public HeaderScreen {
public:
    explicit MyScreen(MyApp* app);

    void onEnter() override;

protected:
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onDraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

private:
    MyApp* _app;
    bool _needsContentRedraw = false;
};
```

```cpp
// src/apps/myapp/MyScreen.cpp
#include "MyScreen.hpp"
#include "MyApp.hpp"
#include "../../app/Navigation.hpp"
#include "../../ui/Layout.hpp"

MyScreen::MyScreen(MyApp* app)
    : HeaderScreen("My App"), _app(app) {
    setLeftButton("< Back", []() {
        Navigation::instance().exitApp();
    });
}

void MyScreen::onEnter() {
    setNeedsFullRedraw();
}

void MyScreen::onHeaderFullRedraw(M5GFX* gfx) {
    // Draw static content below header
    gfx->setTextDatum(MC_DATUM);
    gfx->drawString("Counter:", Layout::centerX(), Layout::headerContentY() + 50);
}

bool MyScreen::onDraw(M5GFX* gfx) {
    if (_needsContentRedraw) {
        // Draw dynamic content
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(3);
        gfx->drawNumber(_app->counter(), Layout::centerX(), Layout::centerY());
        gfx->setTextSize(1);
        _needsContentRedraw = false;
        return true;  // Content changed, needs display()
    }
    return false;
}

bool MyScreen::onTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (released) {
        _app->counter()++;
        _needsContentRedraw = true;
        return true;
    }
    return false;
}
```

### 4. Register the App

In `src/main.cpp`:

```cpp
#include "apps/myapp/MyApp.hpp"

static MyApp myApp;

void setup() {
    // ... existing setup ...

    auto& registry = AppRegistry::instance();
    registry.registerApp(&homeApp);
    registry.registerApp(&myApp);  // Add your app
    registry.registerApp(&mtgApp);
    registry.registerApp(&settingsApp);

    // ...
}
```

### 5. Add an Icon

1. Create a 64x64 PNG or BMP image in `src/assets/`
2. Run `python tools/icons/convert_icons.py` or build the project
3. Reference as `ICON_FILENAME` in your app metadata

## Screen Hierarchy

```
Screen (base)
├── ToolbarScreen
│   └── HeaderScreen
│       ├── MTGLifeScreen
│       ├── MTGSettingsScreen
│       ├── SystemSettingsScreen
│       └── WiFiScreen
└── HomeScreen (direct Screen subclass)
```

### Screen Base Class

```cpp
class Screen {
public:
    virtual void onEnter() {}   // Called when screen becomes active
    virtual void onExit() {}    // Called when screen is deactivated
    virtual void update() {}    // Called every frame
    virtual void draw(M5GFX*) = 0;
    virtual bool handleTouch(int16_t x, int16_t y, bool pressed, bool released);

    void setNeedsFullRedraw(bool needs = true);
    bool needsFullRedraw() const;
};
```

### ToolbarScreen

Adds the system toolbar (WiFi, battery, time) automatically:

- `onUpdate()` - Frame update logic
- `onFullRedraw(gfx)` - Draw static content
- `onDraw(gfx)` - Draw dynamic content, return true if changed

### HeaderScreen

Adds a title bar with optional left/right buttons:

- `onHeaderFullRedraw(gfx)` - Draw static content below header
- `onTouch(x, y, pressed, released)` - Handle touch in content area
- `setLeftButton(label, callback)` - Configure left button
- `setRightButton(label, callback)` - Configure right button

## State Management

### App Owns State

State lives in the App class, not in screens:

```cpp
class MTGApp : public App {
    GameState _gameState;     // State owned by app
    MTGLifeScreen _lifeScreen; // Screens owned by app
public:
    GameState& gameState() { return _gameState; }
};
```

### Screens Access State via App

```cpp
class MTGLifeScreen : public HeaderScreen {
    MTGApp* _app;  // Pointer to owning app

    GameState& gameState() {
        return _app->gameState();
    }
};
```

### Persistence

Use `Preferences` library for NVS storage:

```cpp
void MTGApp::onSuspend() {
    Preferences prefs;
    if (prefs.begin("mtg", false)) {
        _gameState.save(prefs);
        prefs.end();
    }
}
```

## Navigation

### Launch an App

```cpp
Navigation::instance().launchApp("mtg");
// or
Navigation::instance().launchApp(&mtgApp);
```

### Push/Pop Screens

```cpp
// From within an app
Navigation::instance().pushScreen(&_settingsScreen);
Navigation::instance().popScreen();
```

### Exit to Home

```cpp
Navigation::instance().goHome();
// or
Navigation::instance().exitApp();  // Returns to previous app or home
```

## Layout System

Use `Layout::` constants for device-independent positioning:

```cpp
#include "ui/Layout.hpp"

// Screen dimensions
Layout::screenW()    // 960
Layout::screenH()    // 540

// Standard heights
Layout::TOOLBAR_H    // 32
Layout::HEADER_H     // 44

// Content areas
Layout::contentY()        // Y below toolbar only
Layout::contentH()        // Height below toolbar
Layout::headerContentY()  // Y below toolbar + header
Layout::headerContentH()  // Height below both

// Centers
Layout::centerX()    // 480
Layout::centerY()    // 270

// Margins
Layout::MARGIN_S     // 4
Layout::MARGIN_M     // 8
Layout::MARGIN_L     // 20

// Touch
Layout::MIN_TOUCH    // 44
```

## Adding Icons

1. **Create the image**: 64x64 pixels, PNG or BMP
   - Black pixels = foreground
   - White pixels = background

2. **Place in assets**: `src/assets/my-icon.png`

3. **Generate header**: Run build or `python tools/icons/convert_icons.py`

4. **Use in code**:
   ```cpp
   #include "assets/icons.hpp"

   // In AppMetadata
   static constexpr AppMetadata _metadata = {
       "myapp",
       "My App",
       ICON_MY_ICON,  // Filename converted to ICON_<NAME>
       true
   };

   // Or draw manually
   drawIcon(&M5.Display, ICON_MY_ICON, x, y, TFT_BLACK);
   ```

## Testing

### Native Unit Tests

Run tests on host machine (no device needed):

```bash
pio test -e native
```

Tests are in `test/test_native/`. Use `#ifdef NATIVE_TEST` for test-specific code.

### On-Device Testing Checklist

After changes, verify on hardware:

```
[ ] Home screen renders with app icons
[ ] App launches from home screen
[ ] Touch interactions work correctly
[ ] Back navigation returns to expected screen
[ ] State persists across sleep/wake
[ ] No visual artifacts after screen transitions
```

## Build Commands

```bash
# Compile (no upload)
pio run

# Compile and upload
pio run -t upload

# Serial monitor
pio device monitor

# Upload and monitor
pio run -t upload && pio device monitor

# Clean build
pio run -t clean

# Run native tests
pio test -e native
```

## E-Ink Display Considerations

- **Refresh is slow**: ~300ms full, ~100ms partial
- **Prefer partial updates**: Only redraw changed content
- **`display()` is async**: Returns immediately while refresh happens
- **Minimize full refreshes**: Use for screen transitions only
- **Track dirty state**: Return true from `onDraw()` only when content changed
