#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "../external/glad/include/glad/glad.h"
#include "../external/glfw/include/GLFW/glfw3.h"
#include "../inc/FileDirectory.h"
#include "../inc/struct/TreeNode.h"
#include <cstdint>

class UiController
{
private:
    GLFWwindow* window;
    static constexpr uint16_t WINDOW_HEIGHT = 800;
    static constexpr uint16_t WINDOW_WIDTH = 800;
    static constexpr uint8_t MAX_INPUT_SIZE = 75;
    bool is_directory_scanned;
    //bool rendered;
private:
    void display_nodes(TreeNode* node);
public:
    UiController();
    void render(FileDirectory& file_directory);// area where all ui stuff is ran
    void set_dockspace();// settings method

    // ui methods
    void search_bar(FileDirectory& file_directory);
    void file_directory_table(FileDirectory& file_directory); // TODO: might need to pass in &FileDirectory class
    void open_file_dialog(); // TODO: probably needs to be a bool
    ~UiController();
};

#endif
