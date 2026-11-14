#ifndef AKINATOR_HPP
#define AKINATOR_HPP

//----------------------------------------------------------------------------
typedef char* tree_elem_t;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>

#include "colours.hpp"

//----------------------------------------------------------------------------
const char* const GRAPH_FILE = "tree.dot";
const char* const SVG_FILE   = "tree.svg";
const char* const DATA_FILE = "data_akinator.txt";

const size_t BUFFER_SIZE = 256;

//----------------------------------------------------------------------------
enum tree_status
{
    TREE_OK     = 0,
    TREE_ERROR  = 1,
};

enum akinator_mode_t
{
    MODE_UNKNOWN = 0,
    MODE_SAVE    = 1,
    MODE_PLAY    = 2,
    MODE_DUMP    = 3,
    MODE_EXIT    = 4,
};

enum user_answer
{
    ANSWER_UNKNOWN = -1,
    ANSWER_NO      = 0,
    ANSWER_YES     = 1,
};

struct tree_node_t
{
    tree_elem_t data;
    tree_node_t* left;
    tree_node_t* right;
    tree_node_t* parent;
};

struct tree_t
{
    tree_node_t* root;
    size_t size;
};

//----------------------------------------------------------------------------
void close_graph_file(FILE* file);
void graph_dump_node(FILE* file, tree_node_t* node, int depth, tree_node_t* current);
void dump_tree(FILE* file, tree_t* tree, tree_node_t* current);

void delete_node(tree_node_t* node);
void delete_subtree(tree_node_t* node);
size_t len_of_file(const char* name);
tree_status preorder_read(const char* const pre_file, tree_t* tree);
tree_node_t* pre_new_node(char* buffer_file, tree_node_t* parent, size_t* size, int* i);
void preorder_print(const tree_node_t* node, FILE* file);
void call_graphic_dump(void);

tree_status akinator_save(const char* const file_name, tree_t* tree);
akinator_mode_t get_user_mode();
void dump_run(tree_t* tree, tree_node_t** current);
void akinator_run(tree_t* tree, tree_node_t** current);
user_answer get_user_answer();
void akinator_play(tree_node_t* node, tree_node_t** current);
void add_new_character(tree_node_t* node, tree_node_t** current);

//----------------------------------------------------------------------------
#endif