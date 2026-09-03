#ifndef TREENODE_H
#define TREENODE_H

struct TreeNode
{
    TreeNode* parent = nullptr;
    TreeNode* sub_folder = nullptr;
    TreeNode* next_file = nullptr;
    TreeNode* next_all = nullptr; // for linear search (LinkedList DS)
    char* file_name = nullptr;
    char* file_path = nullptr;
    bool is_directory = false;
};
#endif
