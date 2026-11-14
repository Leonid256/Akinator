#include "akinator.hpp"

//----------------------------------------------------------------------------
void delete_node(tree_node_t* node)
{
    assert(node != NULL);

    free(node -> data);
    node -> data = NULL;
    node -> parent = NULL;
    node -> left = NULL;
    node -> right = NULL;
    free(node);
}

//----------------------------------------------------------------------------
void delete_subtree(tree_node_t* node)
{
    if (!node) return;

    delete_subtree(node -> left);
    delete_subtree(node -> right);

    free(node -> data);
    free(node);
}

//----------------------------------------------------------------------------
size_t len_of_file(const char* name)
{
    struct stat file_info;
    stat(name, &file_info);

    if (file_info.st_size <= 0) return 0;
    else return (size_t)file_info.st_size;
}

//----------------------------------------------------------------------------
tree_status preorder_read(const char* const pre_file, tree_t* tree)
{
    assert(pre_file);
    assert(tree);

    FILE* file = fopen(pre_file, "r");
    if (!file)
    {
        printf(RED "Error in preorder_read\n" CRESET);
        return TREE_ERROR;
    }

    size_t file_size = len_of_file(pre_file);
    char* buffer_file = (char*)calloc(file_size + 2, 1);
    fread(buffer_file, 1, file_size, file);
    fclose(file);

    int i = 0;
    while (isspace((unsigned char)buffer_file[i])) i++;

    if (buffer_file[i++] != '(')
    {
        printf(RED "Error in preorder_read\n" CRESET);
        free(buffer_file);
        return TREE_ERROR;
    }
    
    size_t size = 0;
    tree_node_t* root = pre_new_node(buffer_file, NULL, &size, &i);
    if (root)
        tree -> root = root;
    else
    {
        printf(RED "Error in preorder_read\n" CRESET);
        free(buffer_file);
        return TREE_ERROR;
    }
    
    tree -> size = size;

    free(buffer_file);
    printf(GRN "Data downloaded successfully\n" CRESET);
    return TREE_OK;
}

//----------------------------------------------------------------------------
tree_node_t* pre_new_node(char* buffer_file, tree_node_t* parent, size_t* size, int* i)
{
    tree_node_t* node = (tree_node_t*) calloc(1, sizeof(tree_node_t));
    if (node == NULL)
    {
        printf(RED "Error in pre_new_node\n" CRESET);
        return NULL;
    }

    char* buffer_data = (char*)calloc(BUFFER_SIZE, 1);

    int buf_i = 0;
    while(buffer_file[*i] != '\0' && buffer_file[*i] != '(' && buffer_file[*i] != ')')
    {
        buffer_data[buf_i++] = buffer_file[*i];
        (*i)++;
    }
    buffer_data[buf_i] = '\0';

    node -> data = buffer_data;
    node -> parent = parent;
    (*size)++;

    while(buffer_file[*i] != '\0' && buffer_file[*i] != '(' && buffer_file[*i] != ')') (*i)++;
    if (buffer_file[*i] == ')')
    {
        node -> left = NULL;
        node -> right = NULL;
        (*i)++;
        return node;
    }
    else if (buffer_file[*i] == '(')
    {
        (*i)++;
        node -> left = pre_new_node(buffer_file, node, size, i);
        if (!node -> left)
        {
            delete_subtree(node);
            return NULL;
        }
    }
    else
    {
        printf(RED "Error in pre_new_node\n" CRESET);
        return NULL;
    }

    while(buffer_file[*i] != '\0' && buffer_file[*i] != '(' && buffer_file[*i] != ')') (*i)++;
    if (buffer_file[*i] == ')')
    {
        printf(RED "Error in pre_new_node: node has only one child\n" CRESET);
        delete_subtree(node);
        return NULL;
    }
    else if (buffer_file[*i] == '(')
    {
        (*i)++;
        node -> right = pre_new_node(buffer_file, node, size, i);
        if (!node -> right)
        {
            delete_subtree(node);
            return NULL;
        }
    }
    else
    {
        printf(RED "Error in pre_new_node\n" CRESET);
        return NULL;
    }

    if (buffer_file[*i] != ')')
    {
        printf(RED "Error in pre_new_node\n" CRESET);
        return NULL;
    }
    
    (*i)++;
    return node;
}

//----------------------------------------------------------------------------
void preorder_print(const tree_node_t* node, FILE* file)
{
    assert(node);
    assert(file);


    fprintf(file, "(");
    fprintf(file, "%s", node -> data);
    if (node -> left) preorder_print(node -> left, file);
    if (node -> right) preorder_print(node -> right, file);
    fprintf(file, ")");
}

//----------------------------------------------------------------------------
tree_status akinator_save(const char* const file_name, tree_t* tree)
{
    assert(file_name);
    assert(tree);

    FILE* file = fopen(file_name, "w");
    if (!file)
    {
        printf(RED "Error in save_tree\n" CRESET);
        return TREE_ERROR;
    }

    preorder_print(tree -> root, file);
    fclose(file);

    printf(GRN "Data saved successfully\n" CRESET);
    return TREE_OK;
}

//----------------------------------------------------------------------------
akinator_mode_t get_user_mode()
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
void dump_run(tree_t* tree, tree_node_t** current)
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
void akinator_run(tree_t* tree, tree_node_t** current)
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
user_answer get_user_answer()
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
void akinator_play(tree_node_t* node, tree_node_t** current)
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
void add_new_character(tree_node_t* node, tree_node_t** current)
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
    // while (getchar() != '\n');
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
void call_graphic_dump()
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

//----------------------------------------------------------------------------