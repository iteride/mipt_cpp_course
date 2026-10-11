#include "event_list.h"

namespace nano_edr {

EventList::~EventList() {
    Clear();
}

void EventList::Clear() {
    if (size_ != 0) {
        EventNode* tmp = head_;
        EventNode* next;

        while (tmp != nullptr) {
            next = tmp->next;
            delete tmp;
            tmp = next;
        }
    }
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

void EventList::PopFront() {
    if (size_ != 0) {
        EventNode* tmp = head_;
        head_ = head_->next;
        size_--;
        if (size_ == 0) {
            tail_ = nullptr;
        }
        delete tmp;
    }
}

void EventList::PushBack(const Event& event) {
    if (size_ == capacity_) {
        PopFront();
    }
    EventNode* node = new EventNode(event);
    if (size_ == 0) {
        head_ = node;
        tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }
    size_++;
}

}  // namespace nano_edr