#pragma once

struct App
{
    void pause();
    void unpause();
    bool paused = false;
};

extern App app;
