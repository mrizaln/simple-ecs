#include "nexus.hpp"

int main()
{
    auto nexus = nexus::Nexus{ "simple-ecs (nexus)", 1280, 720 };
    nexus.run();
}
