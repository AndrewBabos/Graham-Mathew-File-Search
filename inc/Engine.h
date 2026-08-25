#ifndef ENGINE_H
#define ENGINE_H
#include "FileDirectory.h"
#include "UiController.h"


class Engine
{
private:
    FileDirectory file_directory;
    UiController ui_controller;
    bool startup_finished = false;
private:
    //void run();
public:
    Engine();
    ~Engine();
    bool startup(); // standard check
    void run(); // final check uses the startup_finished flag before running
};

#endif
