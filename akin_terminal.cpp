#include "akinator.hpp"

//TERMINAL VERSION VVVVVVVVV//
//----------------------------------------------------------------------------
akinator_mode_t get_user_mode()     //terminal version
{
    printf("Выберете дальнейшее действие:\n");
    printf("1 - Сохранить базу ответов\n");
    printf("2 - Режим угадывания\n");
    printf("3 - Графический дамп текущего дерева\n");
    printf("4 - Выход\n");

    int mode = MODE_UNKNOWN;
    if (scanf(" %d", &mode) != 1)
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        return MODE_UNKNOWN;
    }

    switch(mode)
    {
        case MODE_SAVE:
        case MODE_PLAY:
        case MODE_DUMP:
        case MODE_EXIT:
            return (akinator_mode_t) mode;
        default:
            return MODE_UNKNOWN;
    }
}

//----------------------------------------------------------------------------
void dump_run(tree_t* tree, tree_node_t** current)       //terminal version
{
    FILE* dump_file = fopen(GRAPH_FILE, "w");
    if (!dump_file)
    {
        printf(RED "Error: can't open %s\n" CRESET, GRAPH_FILE);
        return;
    }

    dump_tree(dump_file, tree, *current);
    close_graph_file(dump_file);

    if (system("dot -Tsvg tree.dot -o tree.svg") != 0)
    {
        printf(RED "Error: dot failed\n" CRESET);
        return;
    }

    call_graphic_dump();
}

//----------------------------------------------------------------------------
void akinator_run(tree_t* tree, tree_node_t** current)       //terminal version
{
    assert(tree);

    akinator_mode_t mode = MODE_UNKNOWN;

    while (mode != MODE_EXIT)
    {
        mode = get_user_mode();

        switch(mode)
        {
            case MODE_SAVE:
                akinator_save(DATA_FILE, tree);
                break;
            case MODE_PLAY:
                akinator_play(tree -> root, current);
                break;
            case MODE_DUMP:
                dump_run(tree, current);
                break;
            case MODE_EXIT:
                return;
            case MODE_UNKNOWN:
            default:
                printf(RED "Error: wrong mode, try again\n" CRESET);
                break;
        }
    }
}

//----------------------------------------------------------------------------
user_answer get_user_answer()        //terminal version
{
    char mode = '\0';
    while (mode != 'y' && mode != 'n')
    {
        if (scanf(" %c", &mode) != 1)
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }
        if (mode == 'y') return ANSWER_YES;
        else if (mode == 'n') return ANSWER_NO;
        else continue;
    }

    printf(RED "Error in get_user_answer\n" CRESET);
    return ANSWER_NO;
}

//----------------------------------------------------------------------------
void akinator_play(tree_node_t* node, tree_node_t** current)  //terminal version
{
    assert(node);
    assert(current);

    *current = node;
    user_answer mode = ANSWER_NO;

    if (node -> left == NULL)
    {
        printf("It's %s?\n", node -> data);
        mode = get_user_answer();
        switch(mode)
        {
            case ANSWER_YES:
                printf("Of course! I looked through whole PornHub))\n\n");
                return;
            case ANSWER_NO:
                add_new_character(node, current);
                return;
            case ANSWER_UNKNOWN:
            default:
                printf(RED "Error in akinator_play\n" CRESET);
                return;
        }
    }
    else
    {
        printf("%s?\n", node -> data);
        mode = get_user_answer();
        switch(mode)
        {
            case ANSWER_YES:
                akinator_play(node -> left, current);
                break;
            case ANSWER_NO:
                akinator_play(node -> right, current);
                break;
            case ANSWER_UNKNOWN:
            default:
                printf(RED "Error in akinator_play\n" CRESET);
                break;
        }
    }
}

//----------------------------------------------------------------------------
void add_new_character(tree_node_t* node, tree_node_t** current) //terminal version
{
    assert(node);
    assert(current);

    char data[BUFFER_SIZE] = {0};
    printf("Who is it?\n");
    while (getchar() != '\n');
    if (fgets(data, sizeof(data), stdin) != NULL)
    {
        size_t len = strlen(data);
        if (len > 0 && data[len - 1] == '\n') 
        {
            data[len - 1] = '\0';
        }
    }

    char difference[BUFFER_SIZE] = {0};
    printf("%s different from %s: she is...", node -> data, data);
    if (fgets(difference, sizeof(difference), stdin) != NULL)
    {
        size_t len = strlen(difference);
        if (len > 0 && difference[len - 1] == '\n') 
        {
            difference[len - 1] = '\0';
        }
    }

    tree_node_t* node_yes = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    tree_node_t* node_no = (tree_node_t*) calloc(1, sizeof(tree_node_t));

    node_yes -> parent = node;
    node_yes -> data   = node -> data;
    node_yes -> left   = NULL;
    node_yes -> right  = NULL;

    node_no -> parent = node;
    node_no -> data   = strdup(data);
    node_no -> left   = NULL;
    node_no -> right  = NULL;

    node -> data = strdup(difference);
    node -> left = node_yes;
    node -> right = node_no;

    *current = node_no;
}

//----------------------------------------------------------------------------
void call_graphic_dump()     //terminal version
{
    FILE* html = fopen("tree.html", "w");
    if (html == NULL) return;

    fprintf(html,
        "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n<meta charset=\"utf-8\">\n"
        "<title>List dump</title>\n"
        "<style>body{font-family:sans-serif;margin:20px}"
        ".wrap{overflow:auto}svg{width:100%%;height:auto}</style>\n"
        "</head>\n<body>\n<div class=\"wrap\">\n");

    FILE* svg = fopen(SVG_FILE, "r");
    if (svg != NULL)
    {
        char buf[4096];
        bool started = false;
        while (fgets(buf, sizeof buf, svg))
        {
            if (!started && strstr(buf, "<svg")) started = true;
            if (started) fputs(buf, html);
        }
        fclose(svg);
    }

    fprintf(html, "</div>\n</body>\n</html>\n");
    fclose(html);

    system("open tree.html");
}