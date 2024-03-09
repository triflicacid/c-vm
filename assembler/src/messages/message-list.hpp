#pragma once

#include <string>
#include <functional>

#include "message.hpp"

namespace assembler {
    class MessageList {
    private:
        std::vector<Message *> messages;

    public:
        /** Get number of messages. */
        size_t size() { return messages.size(); }

        /** Clear messages (deletes every message). */
        void clear();

        /** Add message. */
        void add(Message *message);

        /** Return whether we contain a message of the given type. */
        bool has_message_of(MessageLevel level);

        /** Get first message with the given level. */
        Message *get_message(MessageLevel level);

        /** Go through each message, calling the given function on it **/
        void for_each_message(const std::function<void(Message&)>& func) const;

        /** Go through each message, calling the given function on it. Only include messages which meet the minimum level. **/
        void for_each_message(const std::function<void(Message&)>& func, MessageLevel min_level) const;

        /** Merge given list into this (append). */
        void append(MessageList &other);
    };
}
