#include <string>
#include <optional>
#include <utility>
#include <ostream>

class Person {

public:

    Person(std::string firstName, std::string lastName, std::optional<int> age = std::nullopt)
            : firstName_(std::move(firstName)),
              lastName_(std::move(lastName)),
              age_(age) {}

    [[nodiscard]] std::string getFirstName() const {
        return firstName_;
    }

    [[nodiscard]] std::string getLastName() const {
        return lastName_;
    }

    [[nodiscard]] std::string getFullName() const {
        return firstName_ + " " + lastName_;
    }

    [[nodiscard]] std::optional<int> getAge() const {
        return age_;
    }

    friend std::ostream &operator<<(std::ostream &os, const Person &p) {
        os << "Person(name=" << p.getFullName() << ", age=";
        if (p.age_) {
            os << *p.age_;
        } else {
            os << "-";
        }
        os << ")";
        return os;
    }

private:
    std::string firstName_;
    std::string lastName_;
    std::optional<int> age_;
};