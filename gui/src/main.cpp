#include "frontend/app.h"

int main(int argc, char **argv) {
    GDiskDestroyer::App app(&argc, &argv);
    app.init();
    app();
    return 0;
}
