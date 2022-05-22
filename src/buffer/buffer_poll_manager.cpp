
#include "buffer/buffer_poll_manager.h"

BufferPoolManager:BufferPoolManager():{
//    bufferPool_ = malloc(sizeof(char) * BUFFER_POOL_SIZE);
    bufferPool_.clear();
}


BufferPoolManager:~BufferPoolManager():{
//    free(bufferPool_);
}

Page BufferPoolManager::*FetchPgImp(page_id_t page_id) {
    size_t bufSize = GetPageId
    size_t bufSize = GetPoolSize();
    bool found = false;
    for (int i = 0; i < bufSize; ++i) {
        if (bufferPool_[i] == page_id) {
            found = true;
            break;
        }
    }
    if (found) {
        if ()
    } else {

    }
}

bool BufferPoolManager::UnpinPgImp(page_id_t page_id, bool is_dirty) {

}

bool BufferPoolManager::FlushPgImp(page_id_t page_id) {

}

Page BufferPoolManager::*NewPgImp(page_id_t *page_id) {

}

bool BufferPoolManager::DeletePgImp(page_id_t page_id) {

}

void BufferPoolManager::FlushAllPgsImp() {

}

size_t BufferPoolManager::GetPoolSize() {
    return BUFFER_POOL_SIZE;
}
