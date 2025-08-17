#include <iostream>
#include "engine/ApplicationFramework.h"

int main(int argc, char* argv[]) {
    ApplicationFramework app;
    
    if (!app.init(argc, argv)) {
        std::cerr << "Failed to initialize application framework!" << std::endl;
        return 1;
    }
        
    bool running = true;
    while (running) {
        int updateResult = app.update();
        if (updateResult == 0) {
            running = false;
            break;
        }
        
        app.display(16.0f / 1000.0f);
        
#ifdef _WIN32
        Sleep(16);
#endif
    }
    
    app.destroy();
    
    return 0;
}