#include "helpers/gui_helper.hpp"
#include "catch2/catch_test_macros.hpp"

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

TEST_CASE("Test Velocity") {
  int test_value = 0;
  REQUIRE(GUI::direction_from_velocity(test_value) == 0);

  test_value = 136;
  REQUIRE(GUI::direction_from_velocity(test_value) == 1);

  test_value = -56;
  REQUIRE(GUI::direction_from_velocity(test_value) == 2);
}

TEST_CASE("Test Map") {
  int test_value = 0;
  REQUIRE(GUI::map(test_value, 0, 100, 0, 255) == 0);

  test_value = 50;
  REQUIRE(GUI::map(test_value, 0, 100, 0, 255) == 127);

  test_value = 100;
  REQUIRE(GUI::map(test_value, 0, 100, 0, 255) == 255);
}

TEST_CASE("Test RawMotors") {
  GUI::sRawMotors testRawMotors = {1, 2, 3, 4};
  REQUIRE(testRawMotors.left_direction == 1);
  REQUIRE(testRawMotors.left_velocity == 2);
  REQUIRE(testRawMotors.right_direction == 3);
  REQUIRE(testRawMotors.right_velocity == 4);
}

TEST_CASE("Test Motors") {
  GUI::sMotors testMotors = {1, 2};
  REQUIRE(testMotors.heading == 1);
  REQUIRE(testMotors.speed == 2);
}

TEST_CASE("Test Camera") {
  GUI::sCamera testCamera = {1, 2};
  REQUIRE(testCamera.tilt == 1);
  REQUIRE(testCamera.pan == 2);
}