#include <unity.h>
#include "models/Player.hpp"
#include "utils/Rect.hpp"

// Player::adjustLife() bounds tests

void test_player_adjust_life_basic() {
    Player p;
    p.life = 20;
    p.adjustLife(5);
    TEST_ASSERT_EQUAL(25, p.life);
}

void test_player_adjust_life_negative() {
    Player p;
    p.life = 20;
    p.adjustLife(-7);
    TEST_ASSERT_EQUAL(13, p.life);
}

void test_player_adjust_life_upper_bound() {
    Player p;
    p.life = 9990;
    p.adjustLife(100);
    TEST_ASSERT_EQUAL(9999, p.life);  // Clamped to max
}

void test_player_adjust_life_lower_bound() {
    Player p;
    p.life = -990;
    p.adjustLife(-100);
    TEST_ASSERT_EQUAL(-999, p.life);  // Clamped to min
}

void test_player_adjust_life_at_max() {
    Player p;
    p.life = 9999;
    p.adjustLife(1);
    TEST_ASSERT_EQUAL(9999, p.life);  // Stays at max
}

void test_player_adjust_life_at_min() {
    Player p;
    p.life = -999;
    p.adjustLife(-1);
    TEST_ASSERT_EQUAL(-999, p.life);  // Stays at min
}

void test_player_reset() {
    Player p;
    p.life = 42;
    p.reset(20);
    TEST_ASSERT_EQUAL(20, p.life);
}

void test_player_set_name() {
    Player p;
    p.setName("Alice");
    TEST_ASSERT_EQUAL_STRING("Alice", p.name);
}

void test_player_set_name_truncation() {
    Player p;
    p.setName("ThisNameIsWayTooLongToFit");
    // Name should be truncated to 15 chars (16 - 1 for null)
    TEST_ASSERT_EQUAL(15, strlen(p.name));
}

// Rect::contains() geometry tests

void test_rect_contains_inside() {
    Rect r(10, 20, 100, 50);                // x=10, y=20, w=100, h=50
    TEST_ASSERT_TRUE(r.contains(50, 40));   // Center
    TEST_ASSERT_TRUE(r.contains(10, 20));   // Top-left corner
    TEST_ASSERT_TRUE(r.contains(109, 69));  // Bottom-right (just inside)
}

void test_rect_contains_outside() {
    Rect r(10, 20, 100, 50);
    TEST_ASSERT_FALSE(r.contains(9, 25));    // Left of rect
    TEST_ASSERT_FALSE(r.contains(50, 19));   // Above rect
    TEST_ASSERT_FALSE(r.contains(110, 40));  // Right of rect (x+w is exclusive)
    TEST_ASSERT_FALSE(r.contains(50, 70));   // Below rect (y+h is exclusive)
}

void test_rect_contains_edges() {
    Rect r(0, 0, 100, 100);
    TEST_ASSERT_TRUE(r.contains(0, 0));     // Origin
    TEST_ASSERT_TRUE(r.contains(99, 99));   // Last point inside
    TEST_ASSERT_FALSE(r.contains(100, 0));  // Right edge (exclusive)
    TEST_ASSERT_FALSE(r.contains(0, 100));  // Bottom edge (exclusive)
}

void test_rect_contains_zero_size() {
    Rect r(10, 10, 0, 0);
    TEST_ASSERT_FALSE(r.contains(10, 10));  // Zero-size rect contains nothing
}

void test_rect_default_constructor() {
    Rect r;
    TEST_ASSERT_EQUAL(0, r.x);
    TEST_ASSERT_EQUAL(0, r.y);
    TEST_ASSERT_EQUAL(0, r.w);
    TEST_ASSERT_EQUAL(0, r.h);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();

    // Player tests
    RUN_TEST(test_player_adjust_life_basic);
    RUN_TEST(test_player_adjust_life_negative);
    RUN_TEST(test_player_adjust_life_upper_bound);
    RUN_TEST(test_player_adjust_life_lower_bound);
    RUN_TEST(test_player_adjust_life_at_max);
    RUN_TEST(test_player_adjust_life_at_min);
    RUN_TEST(test_player_reset);
    RUN_TEST(test_player_set_name);
    RUN_TEST(test_player_set_name_truncation);

    // Rect tests
    RUN_TEST(test_rect_contains_inside);
    RUN_TEST(test_rect_contains_outside);
    RUN_TEST(test_rect_contains_edges);
    RUN_TEST(test_rect_contains_zero_size);
    RUN_TEST(test_rect_default_constructor);

    UNITY_END();
    return 0;
}
