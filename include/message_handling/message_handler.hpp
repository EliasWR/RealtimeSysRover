#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <vector>

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void openWindow (){};
    virtual void handleMessage(const std::vector<char>& message, size_t len){};
    virtual void handleMessage(const std::vector<char>& message, std::vector<char>& response, size_t len){};
};

#endif // MESSAGE_HANDLER_HPP
