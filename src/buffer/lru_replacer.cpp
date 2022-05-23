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
    lru_list.clear();
    bufPinCnt.clear();
    max_page_size_ = num_pages;
}

LRUReplacer::~LRUReplacer() {
    max_page_size_ = 0;
}

bool LRUReplacer::Victim(frame_id_t *id) {
    if (lru_list.empty()) return false;
    frame_id_t ecvit = *lru_list.begin();
    PIN(ecvit);
    *id = ecvit;
    return true;
//    for (auto iter = lru_list.begin(); iter != lru_list.end(); ++iter) {
//        if (GetPinCount(*iter) == 0) {
//            lruMutex.lock();
//            *id = *iter;
//            lru_list.erase(iter);
//            lruMutex.unlock();
//            ReleasePinLock();
//            return true;
//        }
//    }
//    return false;
}

void LRUReplacer::Pin(frame_id_t id) {
//    AddPinCount(id);
    if (!bufPinCnt.count(id)) return;
    for (auto iter = lru_list.begin(); iter != lru_list.end(); ++iter) {
        if (*iter == id) {
            lruMutex.lock();
            lru_list.erase(iter);
            pinLatch_.lock();
            bufPinCnt.erase(id);
            pinLatch_.unlock();
            lruMutex.unlock();
            break;
        }
    }
    return;
}

void LRUReplacer::Unpin(frame_id_t id) {
    if (lru_list.size() == max_page_size_) return;
    if (bufPinCnt.count(id)) return;
    lruMutex.lock();
    pinLatch_.lock();
    bufPinCnt[id] = 0;
    pinLatch_.unlock();
    lru_list.push_back(id);
    lruMutex.unlock();

//    SetPinCount(id);
//    if (GetPinCount(id) <= 0) {
//        lruMutex.lock();
//        lru_list.push_back(id);
//        lruMutex.unlock();
//    }
////    pinLatch_.unlock();
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
