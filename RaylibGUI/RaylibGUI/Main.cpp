#include <iostream>
#include "RaylibGUI.h"
#include <string>
int main()
{
    Color primaryColor = RED;
    Color secondaryColor = { 230,41,55,155 };
    Color textColor = BLACK;
    UIContainer gui(primaryColor, secondaryColor, textColor);
    UIFunctionButton<UIButtonRec> testButton(&gui, []() { std::cout << "Hello World!\n"; }, Rectangle{ 50, 20, 100, 20 });

    float width = 1400;
    float height = 900;
    InitWindow(width, height, "GUItest");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        gui.update();
        BeginDrawing();
        ClearBackground(BLACK);
        gui.draw();
        EndDrawing();
    }
}
