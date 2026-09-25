#include "../inc/FileDirectory.h"
#include "NodePool.hpp"
//#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string.h>

FileDirectory::FileDirectory()
{
    root = nullptr;
    head = nullptr;
    tail = nullptr;
    num_of_nodes = 0;
    is_scanning = false;

    // threads
    threads_count = std::thread::hardware_concurrency();

}

bool FileDirectory::scan(fs::path directory_path)
{
    if (!fs::exists(directory_path) || !fs::is_directory(directory_path))
    {
        std::cerr << "Directory path doesnt exist\n";
        return false;
    }
    if (is_scanning)
    {
        std::cout << "Already Scanning...\n";
        return false;
    }
    if (thr_scan_directory.joinable())
        thr_scan_directory.join();
    head = nullptr;
    tail = nullptr;
    is_scanning = true;
    thr_scan_directory = std::thread([this, directory_path]()
    {
        std::cout << "Now scanning...\n";
        if (root)
        {
            delete_tree_nodes(root);
            root = nullptr;
            std::cout << "Tree was not empty, freed memory and re-scanning...\n";
        }
        num_of_nodes = 0; // reset counter
        auto start = std::chrono::steady_clock::now();
        root = scan_directory(nullptr, directory_path);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "Total time to scan: " << directory_path << " | " << elapsed.count() / 1000 << "s\n";
        std::cout << num_of_nodes << " total nodes allocated in RAM\n";
        is_scanning = false;
        return true;
    });
    return true;
}

// TODO:
// ?    Spawn multiple threads to scan the directory path more efficiently
TreeNode* FileDirectory::scan_directory(TreeNode* parent, fs::path directory_path)
{
    TreeNode* first_child = nullptr;
    TreeNode* last_child = nullptr;

    try
    {
        for (const auto& entry : fs::directory_iterator(directory_path,
            std::filesystem::directory_options::skip_permission_denied))
        {
            TreeNode* new_node = static_cast<TreeNode*>(node_memory_pool.allocate_node());
            if (!new_node)
            {
                std::cerr << "Memory allocation from node pool failed...\n";

            }
            new_node->parent = parent;
            new_node->file_name = nullptr;
            new_node->file_path = nullptr;
            new_node->is_directory = false;

            if (!this->head)
                this->head = new_node;
            else
                tail->next_all = new_node;
            tail = new_node;

            std::string name = entry.path().filename().string();
            std::string path = entry.path().string();
            set_file_name_and_path(new_node, std::move(name), std::move(path));


            if (entry.is_regular_file())
                new_node->is_directory = false;

            else if (entry.is_directory())
            {
                new_node->is_directory = true;
                TreeNode* sub_tree_root = scan_directory(new_node, entry.path());
                new_node->sub_folder = sub_tree_root;
            }
            else
            {
                delete new_node->file_name;
                delete new_node->file_path;
                //delete new_node;
                continue;
            }

            if (first_child == nullptr)
                first_child = new_node; // First item in the list
            else
                last_child->next_file = new_node; // Link to previous
            last_child = new_node;

            num_of_nodes++;
        }
        return first_child;
    }
    catch (fs::filesystem_error& file_error)
    {
        if (file_error.code() == std::errc::io_error ||
            file_error.code() == std::errc::connection_reset ||
            file_error.code() == std::errc::network_unreachable)
        {
            std::cout << "Network error encountered: " << file_error.what() << "\n";
            return nullptr;
        }
        else
        {
            std::cout << "Permission or access error: " << file_error.what() << "\n";
            return first_child; // Return what we have so far
        }
    }
    catch (...)
    {
        std::cout << "Unexpected error during scan.\n";
        is_scanning = false;
        return nullptr;
    }
}

void FileDirectory::set_file_name_and_path(TreeNode* node, std::string file_name, std::string file_path)
{
    node->file_name = new char[file_name.length() + 1];
    node->file_path = new char[file_path.length() + 1];
    std::strcpy(node->file_name, file_name.c_str());
    std::strcpy(node->file_path, file_path.c_str());
}

vector<TreeNode*> FileDirectory::get_search_results(const char* search_string)
{
    vector<TreeNode*> vec_results{};
    if (!head)
        return vec_results; // return empty

    TreeNode* current = head;
    int num_of_occurences = 0;

    while (current != nullptr)
    {
        if (strstr(current->file_name, search_string) != nullptr)
        {
            std::cout << "Found " << current->file_name << "\n";
            num_of_occurences++;
            vec_results.push_back(current);
        }
        current = current->next_all;
    }
    return vec_results;
}

// TODO: replace with open folder dialog eventually :D
const char* FileDirectory::open_folder_dialog()
{
    //return "C:/Users/Andrew/Documents/GitHub/C++/filesearch/scan-test-folder";
    //return "C:/Users/Andrew/Documents/GitHub/C++";
    //return "C:/Users/Andrew/Documents/GitHub";
    return "C:/"; // ABSOLUTE LIMIT TEST
    //return "Z:/Nathan Graham/"; // prod testing
}

void FileDirectory::delete_tree_nodes(TreeNode* node)
{
    if (!node)
        return;

    if (node->file_name)
        free(node->file_name);
    if (node->file_path)
        free(node->file_path);

    TreeNode* child = node->sub_folder;
    while (child)
    {
        TreeNode* next = child->next_file;
        delete_tree_nodes(child);
        child = next;
    }
    delete node;
}

TreeNode* FileDirectory::get_root_node() const
{
    return root;
}

FileDirectory::~FileDirectory()
{
    //node_memory_pool.~NodePool();
    if (thr_scan_directory.joinable())
        thr_scan_directory.join();
    else
        std::cout << "Thread was not used, \n";
    if (root != nullptr)
    {
        node_memory_pool.reset();
        //delete_tree_nodes(root);
        root = nullptr;
        std::cout << "All tree nodes are deleted\n";
    }
    else
        std::cout << "Tree already empty, closing program\n";
}
