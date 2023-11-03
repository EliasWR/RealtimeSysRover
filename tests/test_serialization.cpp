#include <catch2/catch_test_macros.hpp>

#include "json_helper.hpp"
#include "proto_helper.hpp"

std::vector<Person> testProto(const std::vector<Person> &persons) {
  std::vector<Person> list{};
  for (const auto &p : persons) {
    auto serialized = toProto(p);
    auto deserialized = fromProto(serialized);
    list.push_back(deserialized);
  }
  return list;
}

std::vector<Person> testJson(const std::vector<Person> &persons) {
  std::vector<Person> list{};
  for (const auto &p : persons) {
    auto serialized = toJson(p);
    auto deserialized = fromJson(serialized);
    list.push_back(deserialized);
  }
  return list;
}

TEST_CASE("jsonTest") {
  Person Person1 = Person("Martin", "Simengård", 24);
  Person Person2 = Person("Elias", "Refsdal", 23);
  std::vector<Person> testPersons{Person1, Person2};

  bool result = true;
  auto serializedPersons = testJson(testPersons);

  for (int i = 0; i < testPersons.size(); i++) {
    result = result && (serializedPersons[i] == testPersons[i]);
  }

  REQUIRE(result);
}

TEST_CASE("protoTest") {
  Person Person1 = Person("Martin", "Simengård", 24);
  Person Person2 = Person("Elias", "Refsdal", 23);
  std::vector<Person> testPersons{Person1, Person2};

  bool result = true;
  auto serializedPersons = testProto(testPersons);

  for (int i = 0; i < testPersons.size(); i++) {
    result = result && (serializedPersons[i] == testPersons[i]);
  }

  REQUIRE(result);
}