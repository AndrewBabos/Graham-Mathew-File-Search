#ifndef TREENODE_H
#define TREENODE_H

struct TreeNode
{
    TreeNode* parent = nullptr;
    TreeNode* sub_folder = nullptr;
    TreeNode* next_file = nullptr;
    char* file_name = nullptr;
    char* file_path = nullptr;
    size_t file_size = 0;
    bool is_directory;

    // ! Unless I dont understand, this wont work since I need
    // ! to allocate memory to the char*'s
    //TreeNode() : file_name(), file_path(), file_size() {}
};

#endif
