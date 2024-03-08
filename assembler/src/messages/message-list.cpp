#include "message-list.hpp"

namespace assembler {
    void MessageList::clear() {
        for (auto message : messages) {
            delete message;
        }

        messages.clear();
    }

    bool MessageList::has_message_of(MessageLevel level) {
        return std::any_of(messages.begin(), messages.end(), [&level](Message *message) {
            return message->get_level() == level;
        });
    }

    Message *MessageList::get_message(MessageLevel level) {
        auto it = std::find_if(messages.begin(), messages.end(), [&level](Message *message) {
            return message->get_level() == level;
        });

        return it == messages.end() ? nullptr : *it;
    }

    void MessageList::for_each_message(const std::function<void(Message&)>& func) const {
        for (auto message : messages) {
            func(*message);
        }
    }

    void MessageList::for_each_message(const std::function<void(Message&)>& func, MessageLevel min_level) const {
        for (auto message : messages) {
            if (message->get_level() >= min_level) {
                func(*message);
            }
        }
    }

    void MessageList::add(Message *message) {
        messages.push_back(message);
    }
}
