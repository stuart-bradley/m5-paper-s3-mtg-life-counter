#pragma once

#include "../../app/App.hpp"
#include "HomeScreen.hpp"

class HomeApp : public App {
   public:
    HomeApp() = default;

    const AppMetadata& metadata() const override { return _metadata; }
    Screen* getMainScreen() override { return &_homeScreen; }

   private:
    static constexpr AppMetadata _metadata = {
        "home", "Home",
        nullptr,  // No icon for home
        false     // Not shown in launcher
    };

    HomeScreen _homeScreen;
};
