#include "list.hpp"

namespace message {
    void List::clear() {
        for (auto message : messages) {
            delete message;
        }

        messages.clear();
    }

    bool List::has_message_of(Level level) {
        return std::any_of(messages.begin(), messages.end(), [&level](Message *message) {
            return message->get_level() == level;
        });
    }

    Message *List::get_message(Level level) {
        auto it = std::find_if(messages.begin(), messages.end(), [&level](Message *message) {
            return message->get_level() == level;
        });

        return it == messages.end() ? nullptr : *it;
    }

    void List::for_each_message(const std::function<void(Message&)>& func) const {
        for (auto message : messages) {
            func(*message);
        }
    }

    void List::for_each_message(const std::function<void(Message&)>& func, Level min_level) const {
        for (auto message : messages) {
            if (message->get_level() >= min_level) {
                func(*message);
            }
        }
    }

    void List::add(Message *message) {
        messages.push_back(message);
    }

    void List::append(List &other) {
        messages.insert(messages.end(), other.messages.begin(), other.messages.end());
    }

    bool print_and_check(List& list) {
        list.for_each_message([] (Message &msg) {
            msg.print();
        });

        bool is_error = list.has_message_of(Level::Error);

        list.clear();

        return is_error;
    }
}
