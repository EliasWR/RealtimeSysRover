#include <iostream>

#include "proto_helper.hpp"
#include "json_helper.hpp"

void testProto(const std::vector<Person> &persons) {

    for (const auto &p: persons) {
        auto serialized = toProto(p);
        auto deserialized = fromProto(serialized);
        std::cout << deserialized << std::endl;
    }

}

void testJson(const std::vector<Person> &persons) {

    for (const auto &p: persons) {
        auto serialized = toJson(p);
        auto deserialized = fromJson(serialized);
        std::cout << deserialized << std::endl;
    }

}

int main() {

    std::vector<Person> testPersons{
            Person("Per", "Nilsen", 76),
            Person("Conny", "Olafsen")
    };

    std::cout << "Testing proto:" << std::endl;
    testProto(testPersons);

    std::cout << "Testing json:" << std::endl;
    testJson(testPersons);

    std::cout << "------------\n" << std::endl;
    std::cout << "Above should be equal to:" << std::endl;

    for (const auto &p: testPersons) {
        std::cout << p << "\n";
    }

    return 0;
}

