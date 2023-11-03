//
// Created by EliasRefsdalWoie on 10/9/2023.
//

#ifndef REALTIMESYSROVER_PROTO_HELPER_HPP
#define REALTIMESYSROVER_PROTO_HELPER_HPP


#include "Person.pb.h"
#include "person.hpp"

Person fromProto(const std::string &protoMsg) {

  proto::Person p;
  p.ParseFromString(protoMsg);

  auto &firstName = p.firstname();
  auto &lastName = p.lastname();
  std::optional<int> age;
  if (p.has_age()) age = p.age();

  return {firstName, lastName, age};
}

std::string toProto(const Person &p) {
  proto::Person pp;
  pp.set_firstname(p.getFirstName());
  pp.set_lastname(p.getLastName());
  auto age = p.getAge();
  if (age) {
    pp.set_age(*age);
  }

  return pp.SerializeAsString();
}

#endif//REALTIMESYSROVER_PROTO_HELPER_HPP
