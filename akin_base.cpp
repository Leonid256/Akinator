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
