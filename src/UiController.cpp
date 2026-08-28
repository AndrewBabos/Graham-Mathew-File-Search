#include "../inc/UiController.h"
#include "FileDirectory.h"
#include "imgui.h"
#include "../external//imgui/imgui_impl_glfw.h"
#include "../external//imgui/imgui_impl_opengl3.h"
#include "../inc/IconsFontAwesome6.h"
#include <iostream>

UiController::UiController()
{
    if (!glfwInit())
    {
        std::cout << "error initiating GLFW\n";
        return;
    }
    window = window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            "FileSearch", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Error creating main window\n";
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
        std::cout << "Failed to initialize GLAD\n";
        return;
    }
    if (!set_imgui_font())
    {
        std::cout << "Could not load font\n";
        return;
    }
    vec_search_results = {};
    is_directory_scanned = false;
    is_searching = false;
}

bool UiController::set_imgui_font()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    ImFontConfig icon_config;
    icon_config.MergeMode = true;
    icon_config.PixelSnapH = true;
    icon_config.GlyphMinAdvanceX = 14.0f;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF(
        "../external/fonts/fa-solid-900.ttf",
        16.0f,
        &icon_config,
        icon_ranges
    );
    io.Fonts->Build();
    return true;
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

        //ImGui::ShowDemoWindow(); // only when i need doc
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

    if (ImGui::InputText("Enter file name here", buffer, sizeof(buffer)))
    {
        if (ImGui::IsKeyDown(ImGuiKey_Enter))
        {
            is_searching = true;
            auto start = std::chrono::steady_clock::now();
            vec_search_results = file_directory.get_search_results(buffer);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << "Total time to search through linked list: " << elapsed.count() / 1000 << "s\n";
        }
    }


    //ImGui::SameLine();
    if (ImGui::Button("Select Folder"))
    {
        is_searching = false;
        is_directory_scanned = false;
        //ImGui::BeginPopupModal("Scanning...");
        const char* folder_path = file_directory.open_folder_dialog();
        if (folder_path == nullptr)
        {
            std::cout << "Open Folder dialog window returned nullptr (no string)" << std::endl;
            return;
        }
        is_directory_scanned = file_directory.scan(folder_path);
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

    if (!is_searching)
    {
        if (ImGui::BeginTable("Directory", 3, table_flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableHeadersRow();

            if (is_directory_scanned)
                UiController::display_nodes(file_directory.get_root_node());
            ImGui::EndTable();
        }
    }
    else
    {
        //std::cout << "Display search results here\n";
        for (const char* file_name : vec_search_results)
        {
            // figre out how to display search results
        }
    }
}

void UiController::display_nodes(TreeNode* node)
{
    if (!node)
        return;

    static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns  | ImGuiTreeNodeFlags_DrawLinesFull;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiTreeNodeFlags node_flags = tree_node_flags_base;
    while (node != nullptr)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (node->is_directory)
        {
            size_t size = sizeof(node->file_name) + sizeof(ICON_FA_FOLDER) + 16;
            char node_name[size];
            snprintf(node_name, size, "%s %s", ICON_FA_FOLDER, node->file_name);

            //if (ImGui::TreeNodeEx(node->file_name, node_flags))
            if (ImGui::TreeNodeEx(node_name, node_flags))
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Folder");
                display_nodes(node->sub_folder);
                ImGui::TreePop();
            }
        }
        else
        { // fix this +16 stuff not sure why it clips the names
            size_t size = sizeof(node->file_name) + sizeof(ICON_FA_FILE) + 16;
            char node_name[size];
            snprintf(node_name, size, "%s %s", ICON_FA_FILE, node->file_name);
            ImGui::TreeNodeEx(node_name, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            //ImGui::TreeNodeEx(node->file_name, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            ImGui::TableNextColumn();
            if (node->is_directory)
                ImGui::TextUnformatted("Folder");
            else
                ImGui::TextUnformatted("File");
            //ImGui::TableNextColumn();
            //ImGui::Text("%zu (Bytes)", node->file_size);
        }
        node = node->next_file;
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
