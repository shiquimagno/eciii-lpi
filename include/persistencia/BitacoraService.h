#pragma once

#include <filesystem>
#include <string>

class BitacoraService {
public:
    explicit BitacoraService(std::filesystem::path raiz);
    bool registrar(const std::string& accion, const std::string& codigo,
                   const std::string& descripcion, std::string& error) const;

private:
    std::filesystem::path archivo_;
};

