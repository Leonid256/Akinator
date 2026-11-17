#include "akinator.hpp"

int main()
{
    // //Terminal version
    // tree_t tree;
    // tree.root = NULL;
    // tree.size = 0;

    // preorder_read(DATA_FILE, &tree);
    // tree_node_t* current = tree.root;

    // akinator_run(&tree, &current);

    // //Screen version
    app_t app = {};
    if (app_init(&app) != TREE_OK) return TREE_ERROR;
    app_run(&app);
    app_shutdown(&app);
}