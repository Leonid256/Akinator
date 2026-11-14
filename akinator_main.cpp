#include "akinator.hpp"

int main()
{
    tree_t tree;
    tree.root = NULL;
    tree.size = 0;

    preorder_read(DATA_FILE, &tree);
    tree_node_t* current = tree.root;

    akinator_run(&tree, &current);
}