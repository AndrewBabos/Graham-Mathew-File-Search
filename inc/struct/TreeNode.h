#ifndef TREENODE_H
#define TREENODE_H
//#include <string>

struct TreeNode
{
    TreeNode* parent = nullptr;
    TreeNode* sub_folder = nullptr;
    TreeNode* next_file = nullptr;
    TreeNode* next_all = nullptr; // for linear search (LinkedList DS)
    //std::string filename;
    //std::string filepath;
    char* file_name = nullptr;
    char* file_path = nullptr;
    bool is_directory = false;
};

#endif
