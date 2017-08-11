#include <iostream>
#include "server.h"
#include "utils.h"

int main() {
    auto server = Server("0.0.0.0", 80);
    server.start();
    return 0;
}