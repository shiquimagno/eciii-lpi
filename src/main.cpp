#include "interfaz/MenuConsola.h"

#include <exception>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
#ifdef ROUTETRACK_ROOT
        const std::filesystem::path raiz = ROUTETRACK_ROOT;
#else
        const std::filesystem::path raiz = std::filesystem::current_path();
#endif
        MenuConsola menu(raiz);
        menu.ejecutar();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error fatal: " << ex.what() << '\n';
        return 1;
    }
}
