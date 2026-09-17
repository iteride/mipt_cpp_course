#include "../kit/include/l1.2/event_list.h"

nano_edr::EventList::~EventList() {
    ListClear(this);
}

void nano_edr::ListClear(EventList* list) {
    if (list->size != 0) {
        EventNode *vrem = list->head, *t;

        while (vrem != nullptr) {
            t = vrem->next;
            delete vrem;
            vrem = t;
        }
    }
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

void nano_edr::ListPopFront(EventList* list) {
    if (list->size != 0) {
        EventNode* vrem = list->head;
        list->head = list->head->next;
        if (list->size == 1) {
            list->tail = nullptr;
        }
        list->size--;
        delete vrem;
    }
}

void nano_edr::ListPushBack(EventList* list, const Event* event) {
    if (list->size == list->capacity) {
        ListPopFront(list);
    }
    EventNode* node = new EventNode;
    if (list->size == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    node->event = *event;
    list->size++;
}

void nano_edr::EventClear(Event* out) {
    out->pid = "";
    out->ts = "";
    out->type = "";
    out->fields.clear();
}