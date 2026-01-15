#pragma once

#include <cstdint>

namespace Power {

void init();
void resetInactivityTimer();
bool shouldSleep(uint16_t timeoutSecs);
void powerOff();

}  // namespace Power
