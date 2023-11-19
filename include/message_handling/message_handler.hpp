#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void openWindow () = 0;
    virtual void handleMessage(const std::vector<char>& message, size_t& len) = 0;
};

#endif // MESSAGE_HANDLER_HPP
