#pragma once

#include "modelos/Modelos.h"

#include <string>
#include <vector>

class CsvRepository;
class BitacoraService;

class ConductorService {
public:
    ConductorService(std::vector<Conductor>& conductores, CsvRepository& repositorio,
                     BitacoraService& bitacora);

    std::string registrar(const std::string& nombre, Zona zona, std::string& error);
    Conductor* buscar(const std::string& codigo);
    const Conductor* buscar(const std::string& codigo) const;
    std::vector<const Conductor*> activosPorZona(Zona zona) const;
    const std::vector<Conductor>& listar() const;

private:
    std::vector<Conductor>& conductores_;
    CsvRepository& repositorio_;
    BitacoraService& bitacora_;
};

