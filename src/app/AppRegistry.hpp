#pragma once

#include "App.hpp"

class AppRegistry {
   public:
    static AppRegistry& instance();

    void registerApp(App* app);

    // For HomeApp to enumerate launchable apps
    int launchableAppCount() const;
    App* getLaunchableApp(int index);

    // For Navigation
    App* findApp(const char* id);
    App* homeApp() const { return _homeApp; }

   private:
    AppRegistry() = default;

    static constexpr int MAX_APPS = 8;
    App* _apps[MAX_APPS] = {nullptr};
    int _count = 0;
    App* _homeApp = nullptr;
};
