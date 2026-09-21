#include "persistencia/BitacoraService.h"

#include "servicios/Utilidades.h"

#include <fstream>

BitacoraService::BitacoraService(std::filesystem::path raiz)
    : archivo_(std::move(raiz) / "logs" / "bitacora.txt") {}

bool BitacoraService::registrar(const std::string& accion, const std::string& codigo,
                                const std::string& descripcion, std::string& error) const {
    std::filesystem::create_directories(archivo_.parent_path());
    std::ofstream salida(archivo_, std::ios::binary | std::ios::app);
    if (!salida) {
        error = "No se pudo escribir la bitácora: " + archivo_.string();
        return false;
    }
    salida << fechaHoraActual() << " | " << accion << " | " << codigo << " | "
           << descripcion << '\n';
    return static_cast<bool>(salida);
}
