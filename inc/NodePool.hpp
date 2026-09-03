#ifndef NODE_POOL_H
#define NODE_POOL_H
//#include "struct/TreeNode.h"


class NodePool
{
private:
    static constexpr int BYTES_PER_KIBIBYTE = 1024;
    struct MemoryBlock // 64MB
    {
        char data[64 * BYTES_PER_KIBIBYTE];
        MemoryBlock* next = nullptr;
        size_t free_index = 0;
    };
    MemoryBlock* head_block;
    size_t total_allocated_memory_blocks;
public:
    NodePool();
    void* allocate_node();
    constexpr size_t get_total_allocated_blocks();
    void reset();
    ~NodePool();
};


#endif
