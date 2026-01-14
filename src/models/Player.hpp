#pragma once

#include <cstdint>
#include <cstring>

struct Player {
    char name[16] = "Player";
    int16_t life = 20;

    void reset(int16_t startingLife) { life = startingLife; }

    void adjustLife(int16_t delta) {
        int32_t newLife = static_cast<int32_t>(life) + delta;
        if (newLife < -999)
            newLife = -999;
        if (newLife > 9999)
            newLife = 9999;
        life = static_cast<int16_t>(newLife);
    }

    void setName(const char* newName) {
        strncpy(name, newName, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
};
