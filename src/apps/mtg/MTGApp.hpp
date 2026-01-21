#pragma once

#include "../../app/App.hpp"
#include "../../assets/icons.hpp"
#include "../../models/GameState.hpp"
#include "MTGLifeScreen.hpp"
#include "MTGSettingsScreen.hpp"

class MTGApp : public App {
   public:
    MTGApp();

    const AppMetadata& metadata() const override { return _metadata; }

    void onLaunch() override;
    void onSuspend() override;

    Screen* getMainScreen() override { return &_lifeScreen; }
    Screen* getScreen(const char* id) override;

    // State access for screens
    GameState& gameState() { return _gameState; }

    // Screen access
    MTGSettingsScreen* settingsScreen() { return &_settingsScreen; }

   private:
    static constexpr AppMetadata _metadata = {
        "mtg", "MTG Life", ICON_MTG,
        true  // Show in launcher
    };

    GameState _gameState;
    MTGLifeScreen _lifeScreen;
    MTGSettingsScreen _settingsScreen;
};
