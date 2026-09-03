#include "../inc/NodePool.hpp"
#include "struct/TreeNode.h"
#include <new> // line 24 will cry if this isnt included
#include <iostream>

NodePool::NodePool()
{
    head_block = new MemoryBlock();
    total_allocated_memory_blocks = 0;
}

void* NodePool::allocate_node()
{
    if (head_block->free_index + sizeof(TreeNode) > sizeof(head_block->data))
    {
        MemoryBlock* new_block = new MemoryBlock();
        new_block->next = nullptr;
        new_block->free_index = 0;

        head_block->next = new_block;
        head_block = new_block;
        total_allocated_memory_blocks++;
    }
    void* raw_memory = &head_block->data[head_block->free_index];
    head_block->free_index += sizeof(TreeNode);
    return new (raw_memory) TreeNode(); // pointer assignment
}

constexpr size_t NodePool::get_total_allocated_blocks()
{
    return total_allocated_memory_blocks;
}

void NodePool::reset()
{
    head_block->free_index = 0;
}

NodePool::~NodePool()
{
    MemoryBlock* current = head_block;
    while (current != nullptr)
    {
        std::cout << "deleted\n";
        MemoryBlock* next = current->next;
        delete current;
        current = next;
    }
}
