#include "../inc/UiController.h"
#include "FileDirectory.h"
#include "imgui.h"
#include "../external//imgui/imgui_impl_glfw.h"
#include "../external//imgui/imgui_impl_opengl3.h"
#include <iostream>

UiController::UiController()
{
    if (!glfwInit())
    {
        std::cout << "error initiating GLFW" << std::endl;
        return;
    }
    window = window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            "FileSearch", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Error creating main window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    is_directory_scanned = false;
}

void UiController::render(FileDirectory& file_directory)
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        set_dockspace();

        ImGui::Begin("File Directory");
    // search file section
        search_bar(file_directory);
    // file directory table
        file_directory_table(file_directory);

        ImGui::End();
       // ImGui::ShowDemoWindow(); // only when i need doc
        // Render
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void UiController::set_dockspace()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    host_window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    host_window_flags |= ImGuiWindowFlags_NoDocking;
    host_window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    host_window_flags |= ImGuiWindowFlags_NoBackground;
    host_window_flags |= ImGuiWindowFlags_MenuBar;

    ImGui::Begin("DockspaceHost", nullptr, host_window_flags);
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
}

void UiController::search_bar(FileDirectory& file_directory)
{
    char buffer[MAX_INPUT_SIZE] = {}; // might need to change input size, or char* str;
    ImGui::SameLine();
    ImGui::InputText("Enter file name here", buffer, sizeof(buffer));
    //ImGui::SameLine();
    if (ImGui::Button("Select Folder"))
    {
        ImGui::BeginPopupModal("Scanning...");
        const char* folder_path = file_directory.open_folder_dialog();
        if (folder_path == nullptr)
        {
            std::cout << "Open Folder dialog window returned nullptr (no string)" << std::endl;
            return;
        }

        // i think here the bool is set to true WAY before the scan is complete
        if (file_directory.scan(folder_path));
            //file_directory.display_tree();
            //is_directory_scanned = true;
    }
}

/// <summary>file_directory is a method of class UiController
/// <para>This method first checks if the startup_finished bool was set to true before running the child class object methods
/// </summary>
void UiController::file_directory_table(FileDirectory& file_directory)
{
    static ImGuiTreeNodeFlags
        tree_node_flags_base =
            ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

    if (ImGui::BeginTable("Directory", 3, table_flags))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableHeadersRow();

        if (is_directory_scanned)
        {
            std::cout << "Attempting to display data structure...\n";
            UiController::display_nodes(file_directory.get_root_node());
        }
        ImGui::EndTable();
    }
}

void UiController::display_nodes(TreeNode* node)
{
    if (!node)
    {
        std::cout << "The passed in node == nullptr\n";
        return;
    }
    static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesFull;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiTreeNodeFlags node_flags = tree_node_flags_base;
    // if (node != &all_nodes[0])
    //     node_flags &= ~ImGuiTreeNodeFlags_LabelSpanAllColumns; // Only demonstrate this on the root node.

    if (node->is_directory)
    {
        bool open = ImGui::TreeNodeEx(node->file_name, node_flags);
        if ((node_flags) == 0)
        {
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(node->file_path);
        }
        if (open)
        {
            display_nodes(node->sub_folder);
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::TreeNodeEx(node->file_name, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
        ImGui::TableNextColumn();
        ImGui::Text("%zu", node->file_size);
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(node->file_path);
    }
}

void UiController::open_file_dialog()
{

}

UiController::~UiController()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "UiRenderer stopped and closed..." << std::endl;
}
