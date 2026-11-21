#include "akinator.hpp"

//----------------------------------------------------------------------------
//SCREEN VERSION VVVVVVVVV//
tree_status app_init(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    app -> tree = (tree_t*)calloc(1, sizeof(tree_t));
    app -> name = (tree_elem_t)calloc(BUFFER_SIZE, 1);
    app -> diff = (tree_elem_t)calloc(BUFFER_SIZE, 1);

    tree_status tree_st = preorder_read(DATA_FILE, app -> tree);

    InitWindow(1600, 1000, "PornOkinator");

    InitAudioDevice();
    app -> snd_win = LoadSound(SND_WIN_FILE);
    SetSoundVolume(app -> snd_win, 0.5f);

    SetTargetFPS(60);
    app -> screen = S_MENU;
    app -> current = (app -> tree) -> root;
    app -> running = true;
    app -> pic = {};

    int codepointsCount = 0;
    int *codepoints = LoadCodepoints(
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя",
        &codepointsCount
    );
    
    *font = LoadFontEx(FONT_FILE, 32, codepoints, codepointsCount);
    UnloadCodepoints(codepoints);

    return tree_st;
}

//----------------------------------------------------------------------------
void app_run(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    Sound snd = LoadSound(SND_LOGO_FILE);
    SetSoundVolume(snd, 0.5f);
    PlaySound(snd);

    Texture2D background = LoadTexture("assets/images/logo.png");

    while(app -> running && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsTextureValid(background)) DrawTextureEx(background, {0, -200}, 0, 1.56f, WHITE);

        draw_screen(app, font);
        EndDrawing();
    }

    UnloadSound(snd);
    UnloadTexture(background);
}

void app_shutdown(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    UnloadSound(app -> snd_win);
    CloseAudioDevice();

    UnloadFont(*font);
    CloseWindow();
    delete_subtree((app -> tree) -> root);

    free(app -> tree);
    free(app -> name);
    free(app -> diff);
}

//----------------------------------------------------------------------------
void draw_screen(app_t* app, Font* font)
{
    assert(app);

    switch(app -> screen)
    {
        case S_MENU:
            draw_menu(app);
            break;
        case S_ASK:
            draw_ask(app, font);
            break;
        case S_GUESS:
            draw_guess(app, font);
            break;
        case S_WIN:
            draw_win(app, font);
            break;
        case S_NAME:
            draw_name(app, font);
            break;
        case S_DIFF:
            draw_diff(app, font);
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
void draw_centered(const char* text, int y, int fs, Font* font)
{
    assert(text);
    assert(font);

    Vector2 position = {(1600.0f - MeasureText(text, fs)) / 2.0f, (float)y};
    DrawTextEx(*font, text, position, (float)fs, 0.0, BLACK);
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
void draw_ask(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    draw_centered(TextFormat("%s?", app -> current -> data), 300, 60, font);

    if(button({520, 500, 200, 100}, "Yes", GREEN, LIME, DARKGREEN)) answer_question(app, true);
    if(button({880, 500, 200, 100}, "No", RED, PINK, MAROON)) answer_question(app, false);
}

//----------------------------------------------------------------------------
void draw_guess(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    draw_centered(TextFormat("Это %s?", app -> current -> data), 300, 60, font);

    if(button({520, 500, 200, 100}, "Yes", GREEN, LIME, DARKGREEN))
    {
        PlaySound(app -> snd_win);

        UnloadTexture(app -> pic);
        app -> pic = LoadTexture(TextFormat("assets/images/%s.png", app -> current -> data));
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
void draw_win(app_t* app, Font* font)
{
    assert(app);
    assert(font);

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
        draw_centered("(no picture)", 400, 48, font);
    }

    if (button({700, 840, 200, 100}, "Menu", LIGHTGRAY, GREEN, DARKGREEN))
    {
        UnloadTexture(app -> pic);
        app -> pic = {};
        app -> screen = S_MENU;
    }
}
//----------------------------------------------------------------------------
bool text_input(char* buf, size_t cap, int max_width, int font_size, Font* font)
{
    size_t len = strlen(buf);

    int c = 0;
    while ((c = GetCharPressed()) > 0)
    {
        if (c < 32 || c == 127) continue;

        int n = 0;
        const char* data = CodepointToUTF8(c, &n);

        if (len + (size_t)n < cap)
        {
            memcpy(buf + len, data, (size_t)n);
            buf[len + (size_t)n] = '\0';

            if (MeasureTextEx(*font, buf, (float)font_size, 0.0f).x <= (float)max_width) len += (size_t)n;
            else buf[len] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
    {
        while (len > 0)
        {
            len--;
            if (((unsigned char)buf[len] & 0xC0) != 0x80) break;
        }
        buf[len] = '\0';
    }

    return IsKeyPressed(KEY_ENTER) && len > 0;
}

//----------------------------------------------------------------------------
void draw_name(app_t* app, Font* font)
{
    assert(app);
    assert(font);

    DrawRectangle(200, 450, 1200, 300, WHITE);

    Vector2 position = {600.0f, 530.0f};
    DrawTextEx(*font, "Кто это?", position, 60, 0.0f, BLACK);
    DrawRectangle(400, 600, 800, 100, WHITE);
    DrawRectangleLines(400, 600, 800, 100, BLACK);

    position = {420.0f, 624.0f};
    DrawTextEx(*font, app -> name, position, 56, 0.0, BLACK);

    if (text_input(app -> name, BUFFER_SIZE, 760, 56, font)) app -> screen = S_DIFF;
}

//----------------------------------------------------------------------------
void draw_diff(app_t* app, Font* font)
{
    assert(app);
    assert(font);
    
    DrawRectangle(200, 450, 1200, 300, WHITE);

    Vector2 position = {250.0f, 530.0f};
    DrawTextEx(*font, TextFormat("%s отличается от %s...", app -> current -> data, app -> name), position, 60, 0.0f, BLACK);
    DrawRectangle(400, 600, 800, 100, WHITE);
    DrawRectangleLines(400, 600, 800, 100, BLACK);

    position = {420.0f, 624.0f};
    DrawTextEx(*font, app -> diff, position, 56, 0.0, BLACK);

    if (text_input(app -> diff, BUFFER_SIZE, 760, 56, font)) commit_character(app);
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
