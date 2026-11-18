#include "akinator.hpp"

//----------------------------------------------------------------------------
//SCREEN VERSION VVVVVVVVV//
tree_status app_init(app_t* app)
{
    assert(app);

    app -> tree = (tree_t*)calloc(1, sizeof(tree_t));
    tree_status tree_st = preorder_read(DATA_FILE, app -> tree);
    InitWindow(800, 500, "PornOkinator");
    SetTargetFPS(60);
    app -> screen = S_MENU;
    app -> current = (app -> tree) -> root;
    app -> running = true;

    return tree_st;
}

//----------------------------------------------------------------------------
void app_run(app_t* app)
{
    assert(app);

    while(app -> running && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_screen(app);
        EndDrawing();
    }
}

void app_shutdown(app_t* app)
{
    assert(app);

    //unload_picture(app);
    CloseWindow();
    delete_subtree((app -> tree) -> root);
    free(app -> tree);
}

//----------------------------------------------------------------------------
void draw_screen(app_t* app)
{
    assert(app);

    switch(app -> screen)
    {
        case S_MENU:
            draw_menu(app);
            break;
        case S_ASK:
            draw_ask(app);
            break;
        case S_GUESS:
            draw_guess(app);
            break;
        case S_WIN:
            draw_win(app);
            break;
        case S_NAME:
            draw_name(app);
            break;
        case S_DIFF:
            draw_diff(app);
            break;
        default:
            draw_menu(app);
            break;
    }
}

//----------------------------------------------------------------------------
bool button(Rectangle r, const char* text, Color base, Color hover, Color pressed)
{
    bool over = CheckCollisionPointRec(GetMousePosition(), r);

    Color btnColor = base;
    if (over) btnColor = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? pressed : hover;

    DrawRectangleRec(r, btnColor);
    DrawRectangleLinesEx(r, 2, BLACK);

    int fontSize = 20;
    int textWidth  = MeasureText(text, fontSize);
    DrawText(text, (int)(r.x + (r.width - textWidth) / 2), (int)(r.y + (r.height - fontSize) / 2), fontSize, BLACK);

    return over && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

//----------------------------------------------------------------------------
void draw_menu (app_t* app)
{
    assert(app);

    if (button({300, 90, 200, 60}, "Play", LIGHTGRAY, GREEN, DARKGREEN)) start_game(app);
    if (button({300, 160, 200, 60}, "Save", LIGHTGRAY, GREEN, DARKGREEN)) akinator_save(DATA_FILE, app -> tree);
    if (button({300, 230, 200, 60}, "Dump", LIGHTGRAY, GREEN, DARKGREEN)) dump_run(app -> tree, &app -> current);
    if (button({300, 90, 200, 60}, "Exit", LIGHTGRAY, GREEN, DARKGREEN)) app -> running = false;


    // int fontSize = 20;
    // Rectangle play_button = {300, 300, 200, 60};
    // const char* play_text = "Play";

    // Rectangle save_button = {300, 230, 200, 60};
    // const char* save_text = "Save";

    // Rectangle dump_button = {300, 160, 200, 60};
    // const char* dump_text = "Dump";

    // Rectangle exit_button = {300, 90, 200, 60};
    // const char* exit_text = "Exit";
    
    // int textWidth = MeasureText(play_text, fontSize);
    // Vector2 mousePoint = GetMousePosition();
    // Color btnColor = LIGHTGRAY;

    // //Play
    // DrawRectangleRec(play_button, btnColor);
    // DrawRectangleLinesEx(play_button, 2, BLACK);
    // int playX = play_button.x + (play_button.width - textWidth) / 2;
    // int playY = play_button.y + (play_button.height - fontSize) / 2;
    // DrawText(play_text, playX, playY, fontSize, BLACK);

    // //Save
    // DrawRectangleRec(save_button, btnColor);
    // DrawRectangleLinesEx(save_button, 2, BLACK);
    // int saveX = save_button.x + (save_button.width - textWidth) / 2;
    // int saveY = save_button.y + (save_button.height - fontSize) / 2;
    // DrawText(save_text, saveX, saveY, fontSize, BLACK);

    // //Dump
    // DrawRectangleRec(dump_button, btnColor);
    // DrawRectangleLinesEx(dump_button, 2, BLACK);
    // int dumpX = dump_button.x + (dump_button.width - textWidth) / 2;
    // int dumpY = dump_button.y + (dump_button.height - fontSize) / 2;
    // DrawText(dump_text, dumpX, dumpY, fontSize, BLACK);

    // //Exit
    // DrawRectangleRec(exit_button, btnColor);
    // DrawRectangleLinesEx(exit_button, 2, BLACK);
    // int exitX = exit_button.x + (exit_button.width - textWidth) / 2;
    // int exitY = exit_button.y + (exit_button.height - fontSize) / 2;
    // DrawText(exit_text, exitX, exitY, fontSize, BLACK);

    // if(CheckCollisionPointRec(mousePoint, play_button))
    // {
    //     btnColor = GREEN;
    //     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnColor = DARKGREEN;

    //     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     {
    //         start_game(app);
    //     }
    // }

    // if(CheckCollisionPointRec(mousePoint, save_button))
    // {
    //     btnColor = GREEN;
    //     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnColor = DARKGREEN;

    //     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     {
    //         akinator_save(DATA_FILE, app -> tree);
    //     }
    // }

    // if(CheckCollisionPointRec(mousePoint, dump_button))
    // {
    //     btnColor = GREEN;
    //     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnColor = DARKGREEN;

    //     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     {
    //         dump_run(app -> tree, &app -> current);
    //     }
    // }

    // if(CheckCollisionPointRec(mousePoint, exit_button))
    // {
    //     btnColor = GREEN;
    //     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnColor = DARKGREEN;

    //     if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     {
    //         app -> running = false;
    //     }
    // }
}

//----------------------------------------------------------------------------
void draw_ask(app_t* app)
{
    assert(app);

    DrawText(TextFormat("%s?", app -> current -> data), 300, 350, 30, BLACK);

    int fontSize = 20;
    Rectangle yes_button = {300, 250, 70, 40};
    const char* yes_text = "Yes";

    Rectangle no_button = {500, 250, 70, 40};
    const char* no_text = "No";

    int yesWidth = MeasureText(yes_text, fontSize);
    Vector2 mousePoint = GetMousePosition();
    Color yesColor = GREEN;
    Color noColor = RED;

    //Yes
    DrawRectangleRec(yes_button, yesColor);
    DrawRectangleLinesEx(yes_button, 2, BLACK);
    int yesX = yes_button.x + (yes_button.width - yesWidth) / 2;
    int yesY = yes_button.y + (yes_button.height - fontSize) / 2;
    DrawText(yes_text, yesX, yesY, fontSize, BLACK);

    int noWidth = MeasureText(no_text, fontSize);
    //No
    DrawRectangleRec(no_button, noColor);
    DrawRectangleLinesEx(no_button, 2, BLACK);
    int noX = no_button.x + (no_button.width - noWidth) / 2;
    int noY = no_button.y + (no_button.height - fontSize) / 2;
    DrawText(no_text, noX, noY, fontSize, BLACK);

    if(CheckCollisionPointRec(mousePoint, yes_button))
    {
        yesColor = GREEN;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) yesColor = DARKGREEN;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            answer_question(app, true);
        }
    }

    if(CheckCollisionPointRec(mousePoint, no_button))
    {
        noColor = RED;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) noColor = MAROON;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            answer_question(app, false);
        }
    }
}

//----------------------------------------------------------------------------
void draw_guess(app_t* app)
{
    assert(app);

    DrawText(TextFormat("It's %s?", app -> current -> data), 300, 350, 30, BLACK);

    int fontSize = 20;
    Rectangle yes_button = {300, 250, 70, 40};
    const char* yes_text = "Yes";

    Rectangle no_button = {500, 250, 70, 40};
    const char* no_text = "No";

    int yesWidth = MeasureText(yes_text, fontSize);
    Vector2 mousePoint = GetMousePosition();
    Color yesColor = GREEN;
    Color noColor = RED;

    //Yes
    DrawRectangleRec(yes_button, yesColor);
    DrawRectangleLinesEx(yes_button, 2, BLACK);
    int yesX = yes_button.x + (yes_button.width - yesWidth) / 2;
    int yesY = yes_button.y + (yes_button.height - fontSize) / 2;
    DrawText(yes_text, yesX, yesY, fontSize, BLACK);

    int noWidth = MeasureText(no_text, fontSize);
    //No
    DrawRectangleRec(no_button, noColor);
    DrawRectangleLinesEx(no_button, 2, BLACK);
    int noX = no_button.x + (no_button.width - noWidth) / 2;
    int noY = no_button.y + (no_button.height - fontSize) / 2;
    DrawText(no_text, noX, noY, fontSize, BLACK);

    if(CheckCollisionPointRec(mousePoint, yes_button))
    {
        yesColor = GREEN;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) yesColor = DARKGREEN;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            app -> screen = S_WIN;
        }
    }

    if(CheckCollisionPointRec(mousePoint, no_button))
    {
        noColor = RED;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) noColor = MAROON;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            app -> screen = S_NAME;
        }
    }
}

//----------------------------------------------------------------------------
void draw_win(app_t* app)
{
    assert(app);

    Texture2D texture = LoadTexture(TextFormat("%s.png", app -> current));
    if (IsTextureValid(texture))
        DrawTexture(texture, 100, 50, WHITE);
    
    int fontSize = 20;
    Rectangle menu_button = {300, 250, 70, 40};
    const char* menu_text = "Menu";
    int Width = MeasureText(menu_text, fontSize);
    Vector2 mousePoint = GetMousePosition();
    Color menuColor = LIGHTGRAY;

    DrawRectangleRec(menu_button, menuColor);
    DrawRectangleLinesEx(menu_button, 2, BLACK);
    int X = menu_button.x + (menu_button.width - Width) / 2;
    int Y = menu_button.y + (menu_button.height - fontSize) / 2;
    DrawText(menu_text, X, Y, fontSize, BLACK);

    if(CheckCollisionPointRec(mousePoint, menu_button))
    {
        menuColor = GREEN;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) menuColor = DARKGREEN;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            app -> screen = S_MENU;
        }
    }

    UnloadTexture(texture);
}

//----------------------------------------------------------------------------
void draw_name(app_t* app)
{
    assert(app);

    char* name = (char*)calloc(BUFFER_SIZE, 1);
    int letterCount = 0;

    Rectangle textBox = {300, 300, 200, 50};
    bool mouseOnText = false;
    int framesCounter = 0;

    if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
    else mouseOnText = false;

    if (mouseOnText)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();

        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (letterCount < BUFFER_SIZE))
            {
                name[letterCount] = (char)key;
                name[letterCount + 1] = '\0';
                letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        }
    }
    else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    if (mouseOnText) framesCounter++;
    else framesCounter = 0;

    DrawText("Тогда кто это?", 280, 260, 20, GRAY);
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (mouseOnText) 
        DrawRectangleLines((int)textBox.x, (int)textBox.y, 
            (int)textBox.width, (int)textBox.height, RED);
    else 
        DrawRectangleLines((int)textBox.x, (int)textBox.y, 
            (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(name, (int)textBox.x + 10, (int)textBox.y + 12, 28, MAROON);

    if (mouseOnText && (letterCount < BUFFER_SIZE))
    {
        if (((framesCounter / 20) % 2) == 0)
        {
            DrawText("_", (int)textBox.x + 12 + MeasureText(name, 28), (int)textBox.y + 14, 28, MAROON);
        }
    }

    app -> name = name;
    free(name);
}

//----------------------------------------------------------------------------
void draw_diff(app_t* app)
{
    assert(app);

    char* diff = (char*)calloc(BUFFER_SIZE, 1);
    int letterCount = 0;

    Rectangle textBox = {300, 300, 200, 50};
    bool mouseOnText = false;
    int framesCounter = 0;

    if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
    else mouseOnText = false;

    if (mouseOnText)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();

        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (letterCount < BUFFER_SIZE))
            {
                diff[letterCount] = (char)key;
                diff[letterCount + 1] = '\0';
                letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            diff[letterCount] = '\0';
        }
    }
    else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    if (mouseOnText) framesCounter++;
    else framesCounter = 0;

    DrawText(TextFormat("%s отличается от %s..", app -> current -> data, app -> name), 280, 260, 20, GRAY);
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (mouseOnText) 
        DrawRectangleLines((int)textBox.x, (int)textBox.y, 
            (int)textBox.width, (int)textBox.height, RED);
    else 
        DrawRectangleLines((int)textBox.x, (int)textBox.y, 
            (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(diff, (int)textBox.x + 10, (int)textBox.y + 12, 28, MAROON);

    if (mouseOnText && (letterCount < BUFFER_SIZE))
    {
        if (((framesCounter / 20) % 2) == 0)
        {
            DrawText("_", (int)textBox.x + 12 + MeasureText(diff, 28), (int)textBox.y + 14, 28, MAROON);
        }
    }

    app -> diff = diff;

    commit_character(app);

    app -> screen = S_MENU;
    free(diff);
}

//----------------------------------------------------------------------------
void start_game(app_t* app)
{
    assert(app);

    app -> current = app -> tree -> root;
    app -> screen = S_ASK;
}

//----------------------------------------------------------------------------
void answer_question(app_t* app, bool yes)
{
    assert(app);

    app -> current = yes ? app -> current -> left : app -> current -> right;
    update_screen_for_node(app);
}

//----------------------------------------------------------------------------
void commit_character(app_t* app)
{
    assert(app);

    tree_node_t* node_yes = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    tree_node_t* node_no = (tree_node_t*) calloc(1, sizeof(tree_node_t));

    node_yes -> parent = app -> current;
    node_yes -> data   = app -> current -> data;
    node_yes -> left   = NULL;
    node_yes -> right  = NULL;

    node_no -> parent = app -> current;
    node_no -> data   = strdup(app -> name);
    node_no -> left   = NULL;
    node_no -> right  = NULL;

    app -> current -> data  = strdup(app -> diff);
    app -> current -> left  = node_yes;
    app -> current -> right = node_no;

    app -> tree -> size += 2;
    app -> screen = S_MENU;
}

//----------------------------------------------------------------------------
void update_screen_for_node(app_t* app)
{
    assert(app);

    if (app -> current -> left != NULL)
        app -> screen = S_ASK;
    else
        app -> screen = S_GUESS;
}
