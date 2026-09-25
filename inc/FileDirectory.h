#ifndef FILE_DIRECTORY_H
#define FILE_DIRECTORY_H
#include "../inc/struct/TreeNode.h"
<<<<<<< Updated upstream
=======
#include "NodePool.hpp"
#include <cstdint>
#include <queue>
#include <string>
>>>>>>> Stashed changes
#include <thread>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

#define STR_SPACE 2

// <summary>This class represents the FileDirectory Data Structure
class FileDirectory
{
private:
    TreeNode* root; // root of directory tree DS
    TreeNode* head; // linked list for linear search
    TreeNode* tail;
    std::vector<std::thread> thr_workers;
    std::queue<std::thread> queue_of_workers;
    std::thread thr_scan_directory;
<<<<<<< Updated upstream
=======
    static uint16_t threads_count;

    //std::mutex
>>>>>>> Stashed changes
    int num_of_nodes;
    bool is_scanning;
private:
    void delete_tree_nodes(TreeNode* node);
    TreeNode* scan_directory(TreeNode* parent, fs::path directory_path);
public:
    FileDirectory();
    const char* open_folder_dialog();
    bool scan(fs::path directory_path);

    vector<TreeNode*> get_search_results(const char* search_string);

    TreeNode* get_root_node() const;
    ~FileDirectory();
};

#endif // FILE_DIRECTORY_H
