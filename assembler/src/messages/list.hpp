#pragma once

#include <functional>

#include "message.hpp"

namespace assembler::message {
    class List {
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
        bool has_message_of(Level level);

        /** Get first message with the given level. */
        Message *get_message(Level level);

        /** Go through each message, calling the given function on it **/
        void for_each_message(const std::function<void(Message&)>& func) const;

        /** Go through each message, calling the given function on it. Only include messages which meet the minimum level. **/
        void for_each_message(const std::function<void(Message&)>& func, Level min_level) const;

        /** Merge given list into this (append). */
        void append(List &other);
    };
}
