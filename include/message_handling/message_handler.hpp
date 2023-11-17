#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <string>
#include <iostream>

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void handleMessage(const std::vector<char>& message, size_t& len);
};
#endif // MESSAGE_HANDLER_HPP

/*
#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <string>

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void handleMessage(const std::vector<char>& message, size_t& len) = 0;
};

#endif // MESSAGE_HANDLER_HPP
