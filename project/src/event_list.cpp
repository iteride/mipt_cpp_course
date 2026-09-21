#include "event_list.h"

namespace nano_edr {

EventList::~EventList() {
    ListClear(this);
}

void ListClear(EventList* list) {
    if (list->size != 0) {
        EventNode* tmp = list->head;
        EventNode* next;

        while (tmp != nullptr) {
            next = tmp->next;
            delete tmp;
            tmp = next;
        }
    }
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

void ListPopFront(EventList* list) {
    if (list->size != 0) {
        EventNode* tmp = list->head;
        list->head = list->head->next;
        list->size--;
        if (list->size == 0) {
            list->tail = nullptr;
        }
        delete tmp;
    }
}

void ListPushBack(EventList* list, const Event* event) {
    if (list->size == list->capacity) {
        ListPopFront(list);
    }
    EventNode* node = new EventNode{.event = *event};
    if (list->size == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

}  // namespace nano_edr