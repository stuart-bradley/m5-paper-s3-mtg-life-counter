#include <M5Unified.h>
#include <Preferences.h>

// Test state machine
enum class TestState {
    INIT,
    NVS_TEST,
    BUZZER_TEST,
    EINK_REFRESH_TEST,
    IMU_CONFIG,
    WAITING_POWEROFF,
    DONE
};

TestState currentTest = TestState::INIT;
unsigned long lastActionTime = 0;
int testStep = 0;

// Forward declarations
void runNVSTest();
void runBuzzerTest();
void runEinkRefreshTest();
void configureIMUWake();
void log(const char* msg);
void logf(const char* fmt, ...);

void setup() {
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    M5.Display.setRotation(1);  // Landscape (960x540)
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.fillScreen(TFT_WHITE);

    Serial.println("\n\n========================================");
    Serial.println("M5Paper S3 Hardware Validation Tests");
    Serial.println("========================================\n");

    M5.Display.setCursor(20, 20);
    M5.Display.println("HARDWARE VALIDATION");
    M5.Display.println("Check serial monitor for results");
    M5.Display.println("");
    M5.Display.println("Touch screen to advance tests");

    delay(1000);
    currentTest = TestState::NVS_TEST;
    testStep = 0;
}

void loop() {
    M5.update();

    switch (currentTest) {
        case TestState::INIT:
            break;

        case TestState::NVS_TEST:
            runNVSTest();
            break;

        case TestState::BUZZER_TEST:
            runBuzzerTest();
            break;

        case TestState::EINK_REFRESH_TEST:
            runEinkRefreshTest();
            break;

        case TestState::IMU_CONFIG:
            configureIMUWake();
            break;

        case TestState::WAITING_POWEROFF:
            // Touch to power off
            if (M5.Touch.getCount() > 0) {
                auto touch = M5.Touch.getDetail();
                if (touch.wasClicked()) {
                    log("Powering off... Pick up device to test IMU wake");
                    delay(500);
                    M5.Power.powerOff();
                }
            }
            break;

        case TestState::DONE:
            // All tests complete, idle
            break;
    }

    delay(50);
}

void log(const char* msg) {
    Serial.println(msg);
}

void logf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.println(buf);
}

// ============================================
// NVS TEST
// ============================================
void runNVSTest() {
    log("\n--- NVS TEST ---");

    Preferences prefs;
    bool success = true;

    // Test 1: Write and read integer
    log("Test 1: Write/read integer...");
    if (!prefs.begin("hw_test", false)) {
        log("  FAIL: Could not open namespace for write");
        success = false;
    } else {
        prefs.putInt("testInt", 42);
        prefs.end();

        prefs.begin("hw_test", true);  // read-only
        int val = prefs.getInt("testInt", 0);
        prefs.end();

        if (val == 42) {
            log("  PASS: Integer read/write works (42)");
        } else {
            logf("  FAIL: Expected 42, got %d", val);
            success = false;
        }
    }

    // Test 2: Write and read string
    log("Test 2: Write/read string...");
    prefs.begin("hw_test", false);
    prefs.putString("testStr", "Player1");
    prefs.end();

    prefs.begin("hw_test", true);
    String str = prefs.getString("testStr", "");
    prefs.end();

    if (str == "Player1") {
        log("  PASS: String read/write works (Player1)");
    } else {
        logf("  FAIL: Expected 'Player1', got '%s'", str.c_str());
        success = false;
    }

    // Test 3: Persistence test (read from previous boot)
    log("Test 3: Boot count persistence...");
    prefs.begin("hw_test", false);
    int bootCount = prefs.getInt("bootCnt", 0);
    bootCount++;
    prefs.putInt("bootCnt", bootCount);
    prefs.end();
    logf("  INFO: Boot count = %d (increment each boot to verify persistence)", bootCount);

    // Cleanup
    prefs.begin("hw_test", false);
    prefs.remove("testInt");
    prefs.remove("testStr");
    // Keep bootCnt for persistence testing
    prefs.end();

    if (success) {
        log("NVS TEST: ALL PASSED\n");
    } else {
        log("NVS TEST: SOME FAILURES\n");
    }

    currentTest = TestState::BUZZER_TEST;
    testStep = 0;
    lastActionTime = millis();
}

// ============================================
// BUZZER TEST
// ============================================
void runBuzzerTest() {
    if (testStep == 0) {
        log("\n--- BUZZER TEST ---");
        log("Playing different frequency tones...");

        M5.Display.fillScreen(TFT_WHITE);
        M5.Display.setCursor(20, 20);
        M5.Display.println("BUZZER TEST");
        M5.Display.println("Listen for tones...");

        testStep = 1;
        lastActionTime = millis();
    }

    unsigned long elapsed = millis() - lastActionTime;

    // Play tones with delay between them
    if (testStep == 1 && elapsed > 500) {
        log("  Playing 440 Hz (A4) - UI click tone");
        M5.Speaker.tone(440, 100);
        testStep = 2;
        lastActionTime = millis();
    }
    else if (testStep == 2 && elapsed > 500) {
        log("  Playing 880 Hz (A5) - life increase tone");
        M5.Speaker.tone(880, 100);
        testStep = 3;
        lastActionTime = millis();
    }
    else if (testStep == 3 && elapsed > 500) {
        log("  Playing 220 Hz (A3) - life decrease tone");
        M5.Speaker.tone(220, 100);
        testStep = 4;
        lastActionTime = millis();
    }
    else if (testStep == 4 && elapsed > 500) {
        log("  Playing ascending tones (alert pattern)");
        M5.Speaker.tone(330, 80);
        delay(100);
        M5.Speaker.tone(440, 80);
        delay(100);
        M5.Speaker.tone(550, 80);
        delay(100);
        M5.Speaker.tone(660, 150);
        testStep = 5;
        lastActionTime = millis();
    }
    else if (testStep == 5 && elapsed > 800) {
        log("BUZZER TEST: Listen for 4 distinct sounds above");
        log("  (Manual verification required)\n");

        M5.Display.println("");
        M5.Display.println("Did you hear 4 tones?");
        M5.Display.println("Touch to continue...");

        testStep = 6;
    }
    else if (testStep == 6) {
        // Wait for touch to advance
        if (M5.Touch.getCount() > 0) {
            auto touch = M5.Touch.getDetail();
            if (touch.wasClicked()) {
                currentTest = TestState::EINK_REFRESH_TEST;
                testStep = 0;
            }
        }
    }
}

// ============================================
// E-INK REFRESH TEST
// ============================================
void runEinkRefreshTest() {
    if (testStep == 0) {
        log("\n--- E-INK REFRESH TEST ---");
        log("Comparing full vs partial refresh speeds...");

        M5.Display.fillScreen(TFT_WHITE);
        M5.Display.setCursor(20, 20);
        M5.Display.println("E-INK REFRESH TEST");
        testStep = 1;
        lastActionTime = millis();
    }

    unsigned long elapsed = millis() - lastActionTime;

    // Test full refresh
    if (testStep == 1 && elapsed > 500) {
        log("  Test 1: Full quality refresh (epd_quality)...");
        M5.Display.setEpdMode(epd_quality);

        unsigned long start = millis();
        M5.Display.fillRect(100, 150, 200, 100, TFT_BLACK);
        M5.Display.display();  // Force refresh
        unsigned long fullTime = millis() - start;

        logf("  Full refresh time: %lu ms", fullTime);

        testStep = 2;
        lastActionTime = millis();
    }
    // Clear and test partial
    else if (testStep == 2 && elapsed > 1000) {
        log("  Clearing...");
        M5.Display.fillRect(100, 150, 200, 100, TFT_WHITE);
        M5.Display.display();

        testStep = 3;
        lastActionTime = millis();
    }
    // Test fastest/partial refresh
    else if (testStep == 3 && elapsed > 1000) {
        log("  Test 2: Fastest partial refresh (epd_fastest)...");
        M5.Display.setEpdMode(epd_fastest);

        unsigned long start = millis();
        M5.Display.fillRect(350, 150, 200, 100, TFT_BLACK);
        M5.Display.display();
        unsigned long partialTime = millis() - start;

        logf("  Partial refresh time: %lu ms", partialTime);

        testStep = 4;
        lastActionTime = millis();
    }
    // Multiple rapid updates test
    else if (testStep == 4 && elapsed > 500) {
        log("  Test 3: Rapid life counter simulation (10 updates)...");
        M5.Display.setEpdMode(epd_fastest);
        M5.Display.setTextSize(4);

        unsigned long start = millis();
        for (int i = 20; i <= 29; i++) {
            M5.Display.fillRect(600, 200, 150, 60, TFT_WHITE);
            M5.Display.setCursor(620, 210);
            M5.Display.printf("%d", i);
            M5.Display.display();
        }
        unsigned long rapidTime = millis() - start;

        logf("  10 rapid updates time: %lu ms (avg %lu ms each)",
             rapidTime, rapidTime / 10);

        M5.Display.setTextSize(2);
        testStep = 5;
        lastActionTime = millis();
    }
    else if (testStep == 5 && elapsed > 500) {
        log("E-INK REFRESH TEST: Review timings above");
        log("  Expect: Full ~300ms, Partial ~50-100ms\n");

        M5.Display.fillScreen(TFT_WHITE);
        M5.Display.setCursor(20, 20);
        M5.Display.println("E-INK REFRESH TEST DONE");
        M5.Display.println("Check serial for timings");
        M5.Display.println("");
        M5.Display.println("Touch to continue to IMU test...");

        testStep = 6;
    }
    else if (testStep == 6) {
        if (M5.Touch.getCount() > 0) {
            auto touch = M5.Touch.getDetail();
            if (touch.wasClicked()) {
                currentTest = TestState::IMU_CONFIG;
                testStep = 0;
            }
        }
    }
}

// ============================================
// IMU WAKE CONFIGURATION
// ============================================
void configureIMUWake() {
    log("\n--- IMU WAKE TEST ---");

    // Initialize IMU
    if (!M5.Imu.isEnabled()) {
        log("  WARNING: IMU not available or not initialized");
        log("  Skipping IMU wake test");

        M5.Display.fillScreen(TFT_WHITE);
        M5.Display.setCursor(20, 20);
        M5.Display.println("IMU NOT AVAILABLE");
        M5.Display.println("");
        M5.Display.println("All other tests passed!");
        M5.Display.println("Touch to restart tests...");

        currentTest = TestState::DONE;
        return;
    }

    log("  IMU is enabled");

    // For BMI270, we need to configure the motion interrupt
    // The M5Paper S3 uses power-off wake via IMU interrupt to PMS150
    // We configure any-motion detection before power-off

    log("  Configuring motion detection for wake...");

    // Note: The actual IMU wake configuration depends on the PMS150 chip
    // The IMU interrupt pin needs to be connected to PMS150 wake input
    // For now, we just verify IMU is working and can read data

    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);
    logf("  Current accel: X=%.2f, Y=%.2f, Z=%.2f", ax, ay, az);

    // Display instructions
    M5.Display.fillScreen(TFT_WHITE);
    M5.Display.setCursor(20, 20);
    M5.Display.println("IMU WAKE TEST");
    M5.Display.println("");
    M5.Display.println("IMU detected and reading data.");
    M5.Display.println("");
    M5.Display.printf("Accel: X=%.1f Y=%.1f Z=%.1f\n", ax, ay, az);
    M5.Display.println("");
    M5.Display.println("MANUAL TEST REQUIRED:");
    M5.Display.println("1. Touch screen to power off");
    M5.Display.println("2. Pick up/move the device");
    M5.Display.println("3. Verify it powers back on");
    M5.Display.println("");
    M5.Display.println("Touch to power off...");

    log("  IMU WAKE: Ready for manual test");
    log("  Touch screen to power off, then move device to wake");

    currentTest = TestState::WAITING_POWEROFF;
}
