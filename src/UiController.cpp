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
        search_bar(file_directory);
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
    char search_string[MAX_INPUT_SIZE] = {}; // might need to change input size, or char* str;
    ImGui::SameLine();

    if (ImGui::InputText("Enter file name here", search_string, sizeof(search_string)))
    {
        if (ImGui::IsKeyDown(ImGuiKey_Enter))
        {
            is_searching = true;
            auto start = std::chrono::steady_clock::now();
            vec_search_results = file_directory.get_search_results(search_string);
            auto end = std::chrono::steady_clock::now();
            if (vec_search_results.empty())
            {
                std::cout << "No direct reference to a location was specified, no scan started nor finished...\n";
                return;
            }
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << "Total time to search through linked list: " << elapsed.count() / 1000 << "s\n";
        }
    }

    if (ImGui::Button("Select Folder"))
    {
        is_searching = false;
        is_directory_scanned = false;
        const char* folder_path = file_directory.open_folder_dialog();
        if (folder_path == nullptr)
        {
            std::cout << "Open Folder dialog window returned nullptr (no string)" << std::endl;
            return;
        }
        is_directory_scanned = file_directory.scan(folder_path);
    }
    if (is_directory_scanned)
    {
        ImGui::SameLine();
        if (ImGui::Button("Reset Display"))
        {
            if (is_directory_scanned && is_searching)
                is_searching = false;
            else
                std::cout << "cant reset display if nothing was scanned...\n";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Display"))
    {
        if (is_directory_scanned)
            is_searching = false;
        else
            std::cout << "cant reset display if nothing was scanned...\n";
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

        if (is_searching)
        {
            static int id = 0;
            for (const TreeNode* node : vec_search_results)
            {                                 // ICON_FA_FILE & FOLDER are the same size
                const size_t size = strlen(node->file_name) + sizeof(ICON_FA_FILE) + STR_SPACE;
                char node_name[size];
                if (ImGui::IsMouseClicked(RMB))
                    std::cout << node->file_path << "\n";
                if (node->is_directory)
                {
                    ImGui::PushID(id);
                    snprintf(node_name, size, "%s %s", ICON_FA_FOLDER, node->file_name);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TreeNodeEx(node_name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
                    ImGui::TableNextRow();
                    ImGui::PopID();
                }
                else
                {
                    ImGui::PushID(id);
                    snprintf(node_name, size, "%s %s", ICON_FA_FILE, node->file_name);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TreeNodeEx(node_name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
                    ImGui::TableNextRow();
                    ImGui::PopID();
                }
                id++;
            }
        }
        else
        {
            if (is_directory_scanned)
                UiController::display_nodes(file_directory.get_root_node());
        }
        ImGui::EndTable();
    }
}

void UiController::display_nodes(TreeNode* node)
{
    if (!node)
        return;
    std::cout << "Node was not null\n";
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAllColumns  | ImGuiTreeNodeFlags_DrawLinesFull;;
    static int selection = (1 << 2);
    while (node != nullptr)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        const size_t size = strlen(node->file_name) + sizeof(ICON_FA_FOLDER) + STR_SPACE;
        char node_name[size];

        if (node->is_directory)
        {
            std::cout << "Node is a directory\n";
            snprintf(node_name, size, "%s %s", ICON_FA_FOLDER, node->file_name);
            if (ImGui::TreeNodeEx(node_name, node_flags))
            {
                // this doesnt work for some reason
                if (ImGui::IsMouseClicked(RMB))
                    std::cout << node->file_path << "\n";
                    //file_info_popup();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Folder");
                display_nodes(node->sub_folder);
                ImGui::TreePop();
            }
        }
        else
        {
            std::cout << "Node is a file\n";
            snprintf(node_name, size, "%s %s", ICON_FA_FILE, node->file_name);
            ImGui::TreeNodeEx(node_name, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            if (ImGui::IsMouseClicked(RMB))
                std::cout << node->file_path << "\n";
                //file_info_popup();
            ImGui::TableNextColumn();
            if (node->is_directory)
                ImGui::TextUnformatted("Folder");
            else
                ImGui::TextUnformatted("File");
        }
        std::cout << "onto the next file\n";
        node = node->next_file;
    }
}

// TODO:
// ?    Figure out how to open a tiny window for the user to
// ?    open file explorer TO the filepath
void UiController::file_info_popup()
{
    if (ImGui::BeginPopup("File Properties"))
    {
        ImGui::Text("Hello from popup!");
        ImGui::Button("This is a dummy button..");
        ImGui::EndPopup();
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
