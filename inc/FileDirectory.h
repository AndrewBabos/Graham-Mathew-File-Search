#ifndef FILE_DIRECTORY_H
#define FILE_DIRECTORY_H
#include "../inc/struct/TreeNode.h"
//#include <cstdint> // uncomment if addded inside TreeNode.h
#include <thread>
#include <filesystem>
//#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// <summary>This class represents the FileDirectory Data Structure
class FileDirectory
{
private:
    TreeNode* root; // root of directory tree DS
    int num_of_nodes;
    const char* folder_test_path;
    std::thread thr_scan_directory;
    bool is_scanning;

private:
    void delete_tree_nodes(TreeNode* node);
    TreeNode* scan_directory(TreeNode* parent, fs::path directory_path);
    TreeNode* scan_complete_filepath(TreeNode* parent, fs::path directory_path);
    //void display_tree();
public:
    FileDirectory();
    void display_tree();
    const char* open_folder_dialog();
    bool scan(fs::path directory_path);

    TreeNode* get_root_node();
    ~FileDirectory();
};

#endif // FILE_DIRECTORY_H
