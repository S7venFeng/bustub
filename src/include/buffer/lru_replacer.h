//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>

#include "buffer/replacer.h"
#include "common/config.h"
#include "storage/page/page.h"

namespace bustub {

//    typedef struct LRUlist {
//        frame_id_t pageId;
//        int64_t pinCount;
//        LRUlist* prevSlot;
//        LRUlist* nextSlot;
//    }lruList;
//
//    using LRUList = lruList*;
/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

  int getPinCount(frame_id_t id);
 private:
  // TODO(student): implement me!
  size_t lruSize_ = 0;
  std::unordered_map<frame_id_t, int> bufPinCnt;
  std::mutex pinLatch_;
  std::mutex lruMutex;
  std::list<frame_id_t> lru_list;
};

}  // namespace bustub
