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

    // ! THINK
    // do I really need this?
    // Afterall the user is jujst gonna want to be
    // brought to the file location in Windows Explorer...
    //size_t file_size = 0;

    bool is_directory;
};
#endif
