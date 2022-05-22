//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
//    free_list = (FreeList)malloc((num_pages + 1) * sizeof(freeList));
//    free_list->pinCount = -1;
//    free_list->prevSlot = NULL;
//    free_list->nextSlot = NULL;
//    free_list->pageId = INVALID_PAGE_ID;
//    lruSize_ = 0;
    max_page_size_ = num_pages;
}

LRUReplacer::~LRUReplacer() {
//    free(lru_list);
    max_page_size_ = 0;
}

bool LRUReplacer::Victim(frame_id_t *id) {
//    int len = lru_list.size();
//    int evcitIdx = -1;
//    for (int i = 0; i < len; ++i) {
//        if (GetPinCount(i) == 0) {
//            pinLatch_.unlock();
//            evcitIdx = i;
//            break;
//        }
//    }
//    if (evcitIdx == -1) {
//        // all pinned
//        return false;
//    }
    for (auto iter = lru_list.begin(); iter != lru_list.end(); ++iter) {
        if (GetPinCount(*iter) == 0 && *iter == evcitIdx) {
            lruMutex.lock();
            lru_list.erase(iter);
            lruMutex.unlock();
            return true;
        }
    }
    return false;
}

void LRUReplacer::Pin(frame_id_t id) {
    AddPinCount(id);
    for (auto iter = lru_list.begin(); iter != lru_list.end(); ++iter) {
        if (*iter == id) {
            lruMutex.lock();
            lru_list.erase(iter);
            lruMutex.unlock();
            break;
        }
    }
    return;
}

void LRUReplacer::Unpin(frame_id_t id) {
    if (lru_list.size() == max_page_size_) return;
    SetPinCount(id);
    if (GetPinCount(id) <= 0) {
        lruMutex.lock();
        lru_list.push_back(iter);
        lruMutex.unlock();
    }
//    pinLatch_.unlock();
//    ReleasePinLock();
    return;
}

void LRUReplacer::GetPinLock()
{
    pinLatch_.lock();
}

void LRUReplacer::ReleasePinLock()
{
    pinLatch_.unlock();
}
int LRUReplacer::GetPinCount(frame_id_t id) {
//    pinLatch_.lock();
    GetPinLock();
//    pinLatch_.unlock();
    return bufPinCnt[id];
}

void LRUReplacer::SetPinCount(frame_id_t id) {
    GetPinLock();
    bufPinCnt[id] = 0;
    ReleasePinLock();
}

void LRUReplacer::AddPinCount(frame_id_t id) {
    GetPinLock();
    bufPinCnt[id]++;
    ReleasePinLock();
}
size_t LRUReplacer::Size() {
    return lru_list.size();
}

}  // namespace bustub
