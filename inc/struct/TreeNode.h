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
};
#endif
