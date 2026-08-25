#include "../inc/Engine.h"

Engine::Engine()
{

}

/// <summary>startup is a method of the Engine class
/// <para>This method initializes all variables and child classes, then sets a bool to true.
/// </summary>
bool Engine::startup()
{
    startup_finished = true;
    return true;
}
/// <summary>run is a method of the Engine class
/// <para>This method first checks if the startup_finished bool was set to true before running the child class object methods
/// </summary>
void Engine::run()
{
    if (!startup_finished)
        return void();

    ui_controller.render(file_directory);
}

Engine::~Engine()
{

}
