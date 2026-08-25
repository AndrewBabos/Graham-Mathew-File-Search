#include "../inc/FileDirectory.h"
//#include <cstddef>
#include <cstdlib>
//#include <exception>
#include <filesystem>
#include <iostream>
#include <cstring>
#include <thread>

void display_node(TreeNode* node, int depth);

FileDirectory::FileDirectory()
{
    folder_test_path = "C:/Users/Andrew/Documents/GitHub/C++/filesearch/test-folder";
    root = nullptr;
    num_of_nodes = 0;
    is_scanning = false;
}

bool FileDirectory::scan(fs::path directory_path)
{
    if (is_scanning)
    {
        std::cout << "Already Scanning...\n";
        return false;
    }
    if (thr_scan_directory.joinable())
        thr_scan_directory.join();

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


// ? -----------------------------------------------------
// TODO:
// ?     Currently this method scans recursively through
// ?     the filepath given (currently open_folder_dialog()
// ?     returns a hardcoded filepath). Eventually optimize
// ?     where it only scans the folder specified, and THEN
// ?     scan the subfolder when the user opens that folder.
// ? -----------------------------------------------------
TreeNode* FileDirectory::scan_directory(TreeNode* parent, fs::path directory_path)
{
    try
    {
        if (!fs::exists(directory_path) || !fs::is_directory(directory_path))
        {
            std::cerr << "Directory path doesnt exist\n";
            return nullptr;
        }
        // TreeNode* current_node = new TreeNode();
        // current_node->parent = parent;
        TreeNode* first_child = nullptr;
        TreeNode* last_child = nullptr;

        for (const auto& entry : fs::directory_iterator(directory_path,
            std::filesystem::directory_options::skip_permission_denied))
        {
            TreeNode* new_node = new TreeNode();
            new_node->parent = parent;
            new_node->sub_folder = nullptr;
            new_node->next_file = nullptr;

            std::string name = entry.path().filename().string();
            std::string path = entry.path().string();
            new_node->file_name = (char*)malloc(name.length() + 1);
            new_node->file_path = (char*)malloc(path.length() + 1);
            std::strcpy((char*)new_node->file_name, name.c_str());
            std::strcpy((char*)new_node->file_path, path.c_str());

            if (entry.is_regular_file())
            {
                new_node->is_directory = false;
                new_node->file_size = entry.file_size();
            }

            else if (entry.is_directory())
            {
                new_node->is_directory = true;
                new_node->file_size = 0;
                TreeNode* sub_tree_root = scan_directory(new_node, entry.path());
                new_node->sub_folder = sub_tree_root;
            }
            else
            {
                free(new_node->file_name);
                free(new_node->file_path);
                delete new_node;
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
        is_scanning = false;
        std::cout <<"Error reading file, someones accessed it: " << file_error.what();
        return nullptr;
    };
}

// TreeNode* FileDirectory::scan_complete_filepath(TreeNode* parent, fs::path directory_path)
// {
//     //TreeNode* node = new TreeNode();
//     return new TreeNode();
// }

void FileDirectory::display_tree()
{
    if (!root)
    {
        std::cout << "Tree is empty. Run scan() first." << std::endl;
        return;
    }
    // ? might have to have a check where if the scan
    // ? thread is JOINABLE, THEN display the tree
    // # if (thr_scan_directory.joinable())
    // #   display_node(root, 0);
    std::cout << "--- File Directory Structure ---\n";
    display_node(root, 0); // Start at root, depth 0
    std::cout << "--------------------------------\n";
}

void display_node(TreeNode* node, int depth)
{
    if (!node) return;

    TreeNode* current = node;
    while (current != nullptr)
    {
        // indentation
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";

        // check if file name is null
        if (current->file_name)
        {
            std::cout << current->file_name;
            if (!current->is_directory)
                std::cout << " : " << current->file_path;
        }
        else
        {// WARN: theres a complete null node somewhere being added in
            std::cout << "no filename\n";
        }

        if (current->is_directory)
            std::cout << "/ (Dir)\n";
        else
            std::cout << " (" << current->file_size << " bytes)\n";

        // found a folder? (directory)
        if (current->is_directory || current->sub_folder)
            display_node(current->sub_folder, depth + 1);

        current = current->next_file; // move to the next file
    }
}

const char* FileDirectory::open_folder_dialog()
{
    return "C:/Users/Andrew/Documents/GitHub/C++/filesearch/scan-test-folder";
    //return "C:/Users/Andrew/Documents/GitHub/C++";
    //return "C:/Users/Andrew/Documents/GitHub";
    //return "C:/Users/Andrew"; // limit testing LOL
    //return "C:/"; // ABSOLUTE LIMIT TEST LMFAO
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
    if (thr_scan_directory.joinable())
        thr_scan_directory.join();
    else
        std::cout << "Thread was not used, \n";
    if (root != nullptr)
    {
        delete_tree_nodes(root);
        root = nullptr;
        std::cout << "All tree nodes are deleted\n";
    }
    else
        std::cout << "Tree already empty, closing program\n";
}
