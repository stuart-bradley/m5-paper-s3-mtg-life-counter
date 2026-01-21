#pragma once

#include <M5Unified.h>

#ifdef DEBUG
#define LOG_E(fmt, ...) M5.Log.printf("[E] " fmt "\n", ##__VA_ARGS__)
#define LOG_W(fmt, ...) M5.Log.printf("[W] " fmt "\n", ##__VA_ARGS__)
#define LOG_I(fmt, ...) M5.Log.printf("[I] " fmt "\n", ##__VA_ARGS__)
#define LOG_D(fmt, ...) M5.Log.printf("[D] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_E(fmt, ...) M5.Log.printf("[E] " fmt "\n", ##__VA_ARGS__)
#define LOG_W(fmt, ...) ((void)0)
#define LOG_I(fmt, ...) ((void)0)
#define LOG_D(fmt, ...) ((void)0)
#endif
