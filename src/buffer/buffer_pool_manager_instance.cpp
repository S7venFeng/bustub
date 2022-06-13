//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager_instance.h"

#include "common/macros.h"

namespace bustub {

BufferPoolManagerInstance::BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager,
                                                     LogManager *log_manager)
    : BufferPoolManagerInstance(pool_size, 1, 0, disk_manager, log_manager) {}

BufferPoolManagerInstance::BufferPoolManagerInstance(size_t pool_size, uint32_t num_instances, uint32_t instance_index,
                                                     DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size),
      num_instances_(num_instances),
      instance_index_(instance_index),
      next_page_id_(instance_index),
      disk_manager_(disk_manager),
      log_manager_(log_manager) {
  BUSTUB_ASSERT(num_instances > 0, "If BPI is not part of a pool, then the pool size should just be 1");
  BUSTUB_ASSERT(
      instance_index < num_instances,
      "BPI index cannot be greater than the number of BPIs in the pool. In non-parallel case, index should just be 1.");
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManagerInstance::~BufferPoolManagerInstance() {
  delete[] pages_;
  delete replacer_;
}

Page* BufferPoolManagerInstance::GetPage(frame_id_t frame_id) {
    Page* pages = GetPages();
    Page* page = pages + frame_id * PAGE_SIZE;
    return page;
}

bool BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
    frame_id_t frameId = page_table_[page_id];
    Page* p = GetPage(frameId);
    const char *page_data = p->GetData();
    // PIN page First
//    replacer_->Pin(frameId);
    disk_manager_->WritePage(page_id, page_data);
    return true;
}

void BufferPoolManagerInstance::FlushAllPgsImp() {
  // You can do it!
  Page* pages = GetPages();
  int poolSize = GetPoolSize();
  for (int i = 0; i < poolSize; ++i) {
      Page* cur = pages + i * PAGE_SIZE;
      const char *page_data = cur->GetData();
      disk_manager_->WritePage(cur->page_id_, page_data);
  }
}

//Page* BufferPoolManagerInstance::GetFreeBufId()
//{
//    Pages* pages = GetPages();
//    int poolSize = GetPoolSize();
//    for (int i = 0; i < poolSize; ++i) {
//        Pages* cur = pages + i * PAGE_SIZE;
//        const char *page_data = cur->GetData();
//        disk_manager_->WritePage(page_id, page_data);
//    }
//}
Page *BufferPoolManagerInstance::NewPgImp(page_id_t *page_id) {
  // 0.   Make sure you call AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
    page_id_t nextPage = AllocatePage();
    Page* pages = GetPages();
    int len = GetPoolSize();
    bool hasFreeSlot = false;
//    int slot = -1;
    for (int i = 0; i < len; ++i) {
        Page* curPage = pages + i * PAGE_SIZE;
        if (curPage->GetPinCount()) {
            hasFreeSlot = true;
//            slot = i;
            break;
        }
    }
    if (!hasFreeSlot) return nullptr;
    if (free_list_.empty()) {
        frame_id_t freeOne;
        bool evcit = replacer_->Victim(&freeOne);
        if (!evcit) {
            return nullptr;
        }
        page_table_[nextPage] = freeOne;
        Page* freePage = GetPage(freeOne);
        freePage->ResetMemory();
        *page_id = nextPage;
        return freePage;
    } else {
        frame_id_t slot = *free_list_.begin();
        page_table_[nextPage] = slot;
        free_list_.pop_front();
        Page* freePage = GetPage(freeOne);
        freePage->ResetMemory();
        *page_id = nextPage;
        return freePage;
    }
}

Page *BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  if (page_table_.count(page_id) > 0) {
      frame_id_t frameId = page_table_[page_id];
      Page* p = GetPage(frameId);
      replacer_->Pin(frameId);
      return p;
  } else {
//      bool hasFree = true;
      frame_id_t frameId;
      if (free_list_.empty()) {
//          hasFree = false;
          bool evcit = replacer_->Victim(&frameId);
          if (!evcit) {
              return nullptr;
          }
          Page* r = GetPage(frameId);
          if (r->is_dirty_) {
              FlushPgImp(r->GetPageId());
          }
          page_table_.erase(r->GetPageId());
          page_table_[page_id] = frameId;
          r->ResetMemory();
          r->page_id_ = page_id;
          disk_manager_->ReadPage(page_id, r->GetData());
          return r;
      } else {
          auto iter = free_list_.begin();
          frameId = *iter;
          Page* r = GetPage(frameId);
          if (r->IsDirty()) {
              FlushPgImp(r->GetPageId());
          }
          page_table_.erase(r->GetPageId());
          page_table_[page_id] = frameId;
          r->ResetMemory();
          r->page_id_ = page_id;
          disk_manager_->ReadPage(page_id, r->GetData());
          return r;
      }
  }

  return nullptr;
}

bool BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) {
  // 0.   Make sure you call DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
    if (page_table_.count(page_id) > 0) {
        frame_id_t slot = page_table_[page_id];
        Page* p = GetPage(page_id);
        if (p->GetPinCount()) return false;
        page_table_.erase(page_id);
        p->ResetMemory();
        free_list_.push_back(slot);
    } else {
        return true;
    }
  return false;
}

bool BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) {
    if (is_dirty) {
        FlushPgImp(page_id);
    }
    frame_id_t frameId = page_table_[page_id];
    replacer_->Unpin(frameId);
    return true;
}

page_id_t BufferPoolManagerInstance::AllocatePage() {
  const page_id_t next_page_id = next_page_id_;
  next_page_id_ += num_instances_;
  ValidatePageId(next_page_id);
  return next_page_id;
}

void BufferPoolManagerInstance::ValidatePageId(const page_id_t page_id) const {
  assert(page_id % num_instances_ == instance_index_);  // allocated pages mod back to this BPI
}

}  // namespace bustub
