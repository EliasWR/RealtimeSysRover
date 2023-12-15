#include "autonomous_driving/autonomous_driving.hpp"
#include "catch2/catch_test_macros.hpp"

// Test class to access private functions
class AutonomousDrivingClassTest : public AutonomousDriving {
public:
  AutonomousDrivingClassTest() = default;

  static void testPrivateFunctionLimitRadius() {
    AutonomousDrivingClassTest AutonomousDriving;
    std::pair<int, int> result = AutonomousDriving::limitRadius(10, 10, 10);
    REQUIRE(result.first == 7);
    REQUIRE(result.second == 7);
  }

  static void testPrivateFunctionCalculateAngle() {
    AutonomousDrivingClassTest AutonomousDriving;
    double result = AutonomousDriving::calculateAngle(10, 10);
    REQUIRE(result == 45);
    result = AutonomousDriving::calculateAngle(10, -10);
    REQUIRE(result == 315);
    result = AutonomousDriving::calculateAngle(-10, 10);
    REQUIRE(result == 135);
    result = AutonomousDriving::calculateAngle(-10, -10);
    REQUIRE(result == 225);
  }

  static void testPrivateFunctionIsAngleWithinLimits() {
    AutonomousDrivingClassTest AutonomousDriving;
    bool result = AutonomousDriving::isAngleWithinLimits(95, 10);
    REQUIRE(result);
    result = AutonomousDriving::isAngleWithinLimits(265, 10);
    REQUIRE(result);
    result = AutonomousDriving::isAngleWithinLimits(60, 10);
    REQUIRE(!result);
    result = AutonomousDriving::isAngleWithinLimits(290, 10);
    REQUIRE(!result);
  }

  static void testPrivateFunctionAdjustAngleToBoundary() {
    AutonomousDrivingClassTest AutonomousDriving;
    double result = AutonomousDriving::adjustAngleToBoundary(90, 10);
    REQUIRE(result == 90);
    result = AutonomousDriving::adjustAngleToBoundary(300, 10);
    REQUIRE(result == 280);
    result = AutonomousDriving::adjustAngleToBoundary(60, 10);
    REQUIRE(result == 80);
    result = AutonomousDriving::adjustAngleToBoundary(250, 10);
    REQUIRE(result == 260);
  }

  static void testPrivateFunctionConvertToCartesian() {
    AutonomousDrivingClassTest AutonomousDriving;
    std::pair<int, int> result = AutonomousDriving::convertToCartesian(10, 45);
    REQUIRE(result.first == 7);
    REQUIRE(result.second == 7);
    result = AutonomousDriving::convertToCartesian(10, 315);
    REQUIRE(result.first == 7);
    REQUIRE(result.second == -7);
    result = AutonomousDriving::convertToCartesian(10, 135);
    REQUIRE(result.first == -7);
    REQUIRE(result.second == 7);
    result = AutonomousDriving::convertToCartesian(10, 225);
    REQUIRE(result.first == -7);
    REQUIRE(result.second == -7);
  }

  static void testPrivateFunctionCalculateBoxCenter() {
    AutonomousDrivingClassTest AutonomousDriving;
    cv::Rect box(10, 10, 10, 10);
    std::pair<int, int> result = AutonomousDriving::calculateBoxCenter(box);
    REQUIRE(result.first == 15);
    REQUIRE(result.second == 15);
  }

  static void testPrivateFunctionIsObjectCentered() {
    AutonomousDrivingClassTest AutonomousDriving;
    std::pair<int, int> frameSize(100, 100);
    bool result = AutonomousDriving::isObjectCentered(50, frameSize, 0.1);
    REQUIRE(result);
    result = AutonomousDriving::isObjectCentered(10, frameSize, 0.1);
    REQUIRE(!result);
    result = AutonomousDriving::isObjectCentered(90, frameSize, 0.1);
    REQUIRE(!result);
  }

  static void testPrivateFunctionIsSizeInRange() {
    AutonomousDrivingClassTest AutonomousDriving;
    std::pair<int, int> frameSize(100, 100);
    bool result = AutonomousDriving::isSizeInRange(10, frameSize, 0.1, 0.9);
    REQUIRE(result);
    result = AutonomousDriving::isSizeInRange(5, frameSize, 0.1, 0.9);
    REQUIRE(!result);
    result = AutonomousDriving::isSizeInRange(95, frameSize, 0.1, 0.9);
    REQUIRE(!result);
  }

  static void testPrivateFunctionCalculateJoystickPosition() {
    AutonomousDrivingClassTest AutonomousDriving;
    std::pair<int, int> frameSize(100, 100);
    std::pair<int, int> result = AutonomousDriving::calculateJoystickPosition(50, 50, frameSize);
    REQUIRE(result.first == 0);
    REQUIRE(result.second == 0);
    result = AutonomousDriving::calculateJoystickPosition(10, 10, frameSize);
    REQUIRE(result.first == -40);
    REQUIRE(result.second == 40);
    result = AutonomousDriving::calculateJoystickPosition(90, 90, frameSize);
    REQUIRE(result.first == 40);
    REQUIRE(result.second == -40);
  }
};

TEST_CASE("Test setting and getting command") {
  AutonomousDrivingClassTest autoDriving;
  std::string command = "test";
  autoDriving.setLatestCommand(command);
  REQUIRE(autoDriving.getLatestCommand() == command);
}

TEST_CASE("Test limitRadius") {
  AutonomousDrivingClassTest::testPrivateFunctionLimitRadius();
}

TEST_CASE("Test calculateAngle") {
  AutonomousDrivingClassTest::testPrivateFunctionCalculateAngle();
}

TEST_CASE("Test isAngleWithinLimits") {
  AutonomousDrivingClassTest::testPrivateFunctionIsAngleWithinLimits();
}

TEST_CASE("Test adjustAngleToBoundary") {
  AutonomousDrivingClassTest::testPrivateFunctionAdjustAngleToBoundary();
}

TEST_CASE("Test convertToCartesian") {
  AutonomousDrivingClassTest::testPrivateFunctionConvertToCartesian();
}

TEST_CASE("Test calculate box center") {
  AutonomousDrivingClassTest::testPrivateFunctionCalculateBoxCenter();
}

TEST_CASE("Test isObjectCentered") {
  AutonomousDrivingClassTest::testPrivateFunctionIsObjectCentered();
}

TEST_CASE("Test isSizeInRange") {
  AutonomousDrivingClassTest::testPrivateFunctionIsSizeInRange();
}

TEST_CASE("Test calculateJoystickPosition") {
  AutonomousDrivingClassTest::testPrivateFunctionCalculateJoystickPosition();
}