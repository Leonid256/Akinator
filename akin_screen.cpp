#include "akinator.hpp"

//----------------------------------------------------------------------------
//SCREEN VERSION VVVVVVVVV//
tree_status app_init(app_t* app)
{
    assert(app);

    app -> tree = (tree_t*)calloc(1, sizeof(tree_t));
    app -> name = (tree_elem_t)calloc(BUFFER_SIZE, 1);
    app -> diff = (tree_elem_t)calloc(BUFFER_SIZE, 1);

    tree_status tree_st = preorder_read(DATA_FILE, app -> tree);

    InitWindow(1600, 1000, "PornOkinator");
    SetTargetFPS(60);
    app -> screen = S_MENU;
    app -> current = (app -> tree) -> root;
    app -> running = true;
    app -> pic = {};

    return tree_st;
}

//----------------------------------------------------------------------------
void app_run(app_t* app)
{
    assert(app);

    Texture2D background = LoadTexture("images/logo.png");

    while(app -> running && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsTextureValid(background)) DrawTextureEx(background, {0, -200}, 0, 1.56f, WHITE);

        draw_screen(app);
        EndDrawing();
    }

    UnloadTexture(background);
}

void app_shutdown(app_t* app)
{
    assert(app);

    //unload_picture(app);
    CloseWindow();
    delete_subtree((app -> tree) -> root);

    free(app -> tree);
    free(app -> name);
    free(app -> diff);
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
    DrawRectangleLinesEx(r, 4, BLACK);

    int fontSize = 40;
    int textWidth  = MeasureText(text, fontSize);
    DrawText(text, (int)(r.x + (r.width - (float)textWidth) / 2), (int)(r.y + (r.height - (float)fontSize) / 2), fontSize, BLACK);

    return over && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

//----------------------------------------------------------------------------
void draw_centered(const char* text, int y, int fs)
{
    DrawText(text, (1600 - MeasureText(text, fs)) / 2, y, fs, BLACK);
}

//----------------------------------------------------------------------------
void draw_menu (app_t* app)
{
    assert(app);

    if (button({600, 180, 400, 120}, "Play", LIGHTGRAY, GREEN, DARKGREEN)) start_game(app);
    if (button({600, 320, 400, 120}, "Save", LIGHTGRAY, GREEN, DARKGREEN)) akinator_save(DATA_FILE, app -> tree);
    if (button({600, 460, 400, 120}, "Dump", LIGHTGRAY, GREEN, DARKGREEN)) dump_run(app -> tree, &app -> current);
    if (button({600, 600, 400, 120}, "Exit", LIGHTGRAY, GREEN, DARKGREEN)) app -> running = false;
}

//----------------------------------------------------------------------------
void draw_ask(app_t* app)
{
    assert(app);

    draw_centered(TextFormat("%s?", app -> current -> data), 300, 60);

    if(button({520, 500, 200, 100}, "Yes", GREEN, LIME, DARKGREEN)) answer_question(app, true);
    if(button({880, 500, 200, 100}, "No", RED, PINK, MAROON)) answer_question(app, false);
}

//----------------------------------------------------------------------------
void draw_guess(app_t* app)
{
    assert(app);

    draw_centered(TextFormat("It's %s?", app -> current -> data), 300, 60);

    if(button({520, 500, 200, 100}, "Yes", GREEN, LIME, DARKGREEN))
    {
        UnloadTexture(app -> pic);
        app -> pic = LoadTexture(TextFormat("images/%s.png", app -> current -> data));
        app -> screen = S_WIN;
    }

    if(button({880, 500, 200, 100}, "No", RED, PINK, MAROON))
    {
        app -> name[0] = '\0';
        app -> diff[0] = '\0';
        app -> screen = S_NAME;
    }
}

//----------------------------------------------------------------------------
void draw_win(app_t* app)
{
    assert(app);

    if (IsTextureValid(app -> pic))
    {
        float box   = 760.0f;
        float scale = box / (float)fmax(app -> pic.width, app -> pic.height);
        float w = (float)app -> pic.width  * scale;
        float h = (float)app -> pic.height * scale;

        Rectangle src = {0, 0, (float)app -> pic.width, (float)app -> pic.height};
        Rectangle dst = {(1600.0f - w) / 2.0f, 40.0f + (box - h) / 2.0f, w, h};
        DrawTexturePro(app -> pic, src, dst, {0, 0}, 0, WHITE);
    }
    else
    {
        draw_centered("(no picture)", 400, 48);
    }

    if (button({700, 840, 200, 100}, "Menu", LIGHTGRAY, GREEN, DARKGREEN))
    {
        UnloadTexture(app -> pic);
        app -> pic = {};
        app -> screen = S_MENU;
    }
}
//----------------------------------------------------------------------------
bool text_input(char* buf, size_t cap, int max_width, int font_size)
{
    size_t len = strlen(buf);

    int c = 0;
    while ((c = GetCharPressed()) > 0)
    {
        if (c >= 32 && c < 127 && len + 1 < cap)
        {
            buf[len] = (char)c;
            buf[len + 1] = '\0';

            if (MeasureText(buf, font_size) <= max_width) len++;
            else buf[len] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && len > 0)
        buf[--len] = '\0';

    return IsKeyPressed(KEY_ENTER) && len > 0;
}

//----------------------------------------------------------------------------
void draw_name(app_t* app)
{
    DrawText("Who is it?", 600, 530, 60, DARKBLUE);
    DrawRectangle(400, 600, 800, 100, BLUE);
    DrawRectangleLines(400, 600, 800, 100, DARKBLUE);
    DrawText(app -> name, 420, 624, 56, MAROON);

    if (text_input(app -> name, BUFFER_SIZE, 760, 56)) app -> screen = S_DIFF;
}

//----------------------------------------------------------------------------
void draw_diff(app_t* app)
{
    DrawText(TextFormat("%s differs from %s...", app -> current -> data, app -> name), 100, 400, 40, DARKBLUE);
    DrawRectangle(400, 600, 800, 100, BLUE);
    DrawRectangleLines(400, 600, 800, 100, DARKBLUE);
    DrawText(app -> diff, 420, 624, 56, MAROON);

    if (text_input(app -> diff, BUFFER_SIZE, 760, 56)) commit_character(app);
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
