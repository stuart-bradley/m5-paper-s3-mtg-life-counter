#pragma once

#include <cstdint>

class Screen;

struct AppMetadata {
    const char* id;       // "home", "mtg", "settings"
    const char* name;     // "Home", "MTG Life", "Settings"
    const uint8_t* icon;  // 64x64 icon data (nullptr for HomeApp)
    bool showInLauncher;  // false for HomeApp, true for others
};

class App {
   public:
    virtual ~App() = default;

    // Metadata
    virtual const AppMetadata& metadata() const = 0;

    // Lifecycle
    virtual void onLaunch() {}   // Called when app becomes active
    virtual void onSuspend() {}  // Called when switching away from app

    // Screen access - screens are cached, not created each time
    virtual Screen* getMainScreen() = 0;
    virtual Screen* getScreen(const char* screenId) {
        (void)screenId;
        return nullptr;
    }

    // Internal navigation - return true if handled, false to pop/exit
    virtual bool handleBack() { return false; }
};
