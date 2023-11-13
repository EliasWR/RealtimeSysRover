#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <string>

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void handleMessage(const std::string& message) = 0;
};

#endif // MESSAGE_HANDLER_HPP
