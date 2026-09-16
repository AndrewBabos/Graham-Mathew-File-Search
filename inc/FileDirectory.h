#ifndef FILE_DIRECTORY_H
#define FILE_DIRECTORY_H
#include "../inc/struct/TreeNode.h"
#include "NodePool.hpp"
#include <string>
#include <thread>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

#define STR_SPACE 2

// <summary>This class represents the FileDirectory Data Structure
class FileDirectory
{
private:
    NodePool  node_memory_pool;
private:
    TreeNode* root; // root of directory tree DS
    TreeNode* head; // linked list for linear search
    TreeNode* tail;
    std::thread thr_scan_directory;
    //std::mutex
    int num_of_nodes;
    bool is_scanning;
private:
    void delete_tree_nodes(TreeNode* node);
    TreeNode* scan_directory(TreeNode* parent, fs::path directory_path);
    void set_file_name_and_path(TreeNode* node, std::string file, std::string file_path);
public:
    FileDirectory();
    const char* open_folder_dialog();
    bool scan(fs::path directory_path);

    vector<TreeNode*> get_search_results(const char* search_string);
    TreeNode* get_root_node() const;
    ~FileDirectory();
};

#endif // FILE_DIRECTORY_H
