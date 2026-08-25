#include "../inc/Engine.h"

int main(int argc, char* argv[])
{
    Engine engine;
    if (!engine.startup())
        return -1;

    engine.run();

    return 0;
}
