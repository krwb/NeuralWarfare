#include <iostream>
#include "RaylibGUI.h"
#include <string>
int main()
{
    float width = 1400;
    float height = 900;

    Color primaryColor = RED;
    Color secondaryColor = { 230,41,55,150 };
    Color textColor = WHITE;
    UIContainer gui(primaryColor, secondaryColor, textColor);
    UITextLine* tl = new UITextLine(&gui, { width / 2, height / 2 }, "test");
    UIFunctionButton<UIButtonRec> testButton(&gui, [&gui, tl]() { gui.remove(tl); }, Rectangle{ 50, 20, 100, 20 });
    //new UITextBox(&gui, { 20, 100 ,200,200}, "feugiat in ante metus dictum at tempor commodo ullamcorper a lacus vestibulum sed arcu non odio euismod lacinia at quis");
    new UITextInput<UITextLine>(&gui, Vec2{ 20, 100});

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
