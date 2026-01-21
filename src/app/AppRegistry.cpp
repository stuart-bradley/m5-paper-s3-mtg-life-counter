#include "AppRegistry.hpp"
#include <cstring>
#include "../utils/Log.hpp"

AppRegistry& AppRegistry::instance() {
    static AppRegistry instance;
    return instance;
}

void AppRegistry::registerApp(App* app) {
    if (!app) {
        LOG_W("AppRegistry: Attempted to register null app");
        return;
    }
    if (_count >= MAX_APPS) {
        LOG_E("AppRegistry: Cannot register app '%s' - MAX_APPS (%d) exceeded", app->metadata().id,
              MAX_APPS);
        return;
    }

    _apps[_count++] = app;

    // Track home app (non-launchable) separately
    if (!app->metadata().showInLauncher) {
        _homeApp = app;
    }
}

int AppRegistry::launchableAppCount() const {
    int count = 0;
    for (int i = 0; i < _count; i++) {
        if (_apps[i]->metadata().showInLauncher) {
            count++;
        }
    }
    return count;
}

App* AppRegistry::getLaunchableApp(int index) {
    int seen = 0;
    for (int i = 0; i < _count; i++) {
        if (_apps[i]->metadata().showInLauncher) {
            if (seen == index) {
                return _apps[i];
            }
            seen++;
        }
    }
    return nullptr;
}

App* AppRegistry::findApp(const char* id) {
    for (int i = 0; i < _count; i++) {
        if (strcmp(_apps[i]->metadata().id, id) == 0) {
            return _apps[i];
        }
    }
    return nullptr;
}
