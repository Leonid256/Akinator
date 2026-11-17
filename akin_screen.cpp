#include "akinator.hpp"

//----------------------------------------------------------------------------
//SCREEN VERSION VVVVVVVVV//
tree_status app_init(app_t* app)
{
    assert(app);

    tree_status tree_st = preorder_read(DATA_FILE, app->tree);
    InitWindow(800, 500, "PornOkinator");
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

    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_screen(app);
    EndDrawing();;
}

void app_shutdown(app_t* app)
{
    assert(app);

    unload_picture(app);
    CloseWindow();
    delete_subtree((app -> tree) -> root);
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
void draw_menu (app_t* app)
{

}