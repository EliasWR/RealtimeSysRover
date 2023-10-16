
#ifndef SERIALIZATION_DEMO_JSON_HELPER_HPP
#define SERIALIZATION_DEMO_JSON_HELPER_HPP

#include "Person.hpp"

#include "nlohmann/json.hpp"

    Person fromJson(const std::string& json) {
        auto data = nlohmann::json::parse(json);

        std::string firstName = data["firstName"];
        std::string lastName = data["lastName"];
        std::optional<int> age;
        if (data.contains("age")) {
            age = data["age"].get<int>();
        }

        return {firstName, lastName, age};
    }

    std::string toJson(const Person& p) {

        nlohmann::json data;
        data["firstName"] = p.getFirstName();
        data["lastName"] = p.getLastName();
        auto age = p.getAge();
        if (age) {
            data["age"] = *age;
        }

        return data.dump();
    }

#endif //SERIALIZATION_DEMO_JSON_HELPER_HPP
