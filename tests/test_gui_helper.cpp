#include "helpers/gui_helper.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test SplitString") {
  std::string testString = "This is a test string";
  std::vector<std::string> testVector = {"This", "is", "a", "test", "string"};
  REQUIRE(GUI::splitString(testString, ' ') == testVector);

  testString = "This,is,a,test,string";
  testVector = {"This", "is", "a", "test", "string"};
  REQUIRE(GUI::splitString(testString, ',') == testVector);
}

TEST_CASE("Test Direction") {
  int test_value = 0;
  REQUIRE(GUI::direction_from_velocity(test_value) == 0);

  test_value = 136;
  REQUIRE(GUI::direction_from_velocity(test_value) == 1);

  test_value = -56;
  REQUIRE(GUI::direction_from_velocity(test_value) == 2);
}

/*
TEST_CASE("Test Joystick to raw motors") {
    int joy_x = 0;
    int joy_y = 0;
    std::string test_string = "0_0_0_0";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = 0;
    joy_y = 100;
    test_string = "1_255_1_255";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = 0;
    joy_y = -100;
    test_string = "2_255_2_255";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = 100;
    joy_y = 0;
    test_string = "1_255_2_255";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = -100;
    joy_y = 0;
    test_string = "2_255_1_255";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = 50;
    joy_y = 50;
    test_string = "1_127_1_127";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);

    joy_x = -50;
    joy_y = 50;
    test_string = "2_127_1_127";
    REQUIRE(joystick_to_raw_motors(joy_x, joy_y) == test_string);
}
 */

/*
TEST_CASE("Test Joystick to Heading and Speed") {
  int joy_x = 0;
  int joy_y = 0;
  std::string test_string = "0_0_0_0";
  REQUIRE(GUI::joystick_to_raw_motors(joy_x, joy_y) == test_string);

  joy_x = 0;
  joy_y = 100;
  test_string = "0_255";
  REQUIRE(GUI::joystick_to_heading_and_speed(joy_x, joy_y) == test_string);

  joy_x = 0;
  joy_y = -100;
  test_string = "180_255";
  REQUIRE(GUI::joystick_to_heading_and_speed(joy_x, joy_y) == test_string);

  joy_x = 100;
  joy_y = 0;
  test_string = "90_255";
  REQUIRE(GUI::joystick_to_heading_and_speed(joy_x, joy_y) == test_string);

  joy_x = -100;
  joy_y = 0;
  test_string = "270_255";
  REQUIRE(GUI::joystick_to_heading_and_speed(joy_x, joy_y) == test_string);

  joy_x = 50;
  joy_y = 50;
  test_string = "45_180";
  REQUIRE(GUI::joystick_to_heading_and_speed(joy_x, joy_y) == test_string);
}
*/

TEST_CASE("Test Message handler") {
  std::string test_command = "move_forward";
}