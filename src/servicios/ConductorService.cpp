#include "servicios/ConductorService.h"

#include "persistencia/BitacoraService.h"
#include "persistencia/CsvRepository.h"
#include "servicios/Utilidades.h"

#include <algorithm>

ConductorService::ConductorService(std::vector<Conductor>& conductores,
                                   CsvRepository& repositorio, BitacoraService& bitacora)
    : conductores_(conductores), repositorio_(repositorio), bitacora_(bitacora) {}

std::string ConductorService::registrar(const std::string& nombre, Zona zona, std::string& error) {
    const auto nombreLimpio = recortar(nombre);
    if (nombreLimpio.empty()) {
        error = "El nombre del conductor es obligatorio.";
        return {};
    }
    std::vector<std::string> codigos;
    for (const auto& c : conductores_) codigos.push_back(c.codigo);
    Conductor conductor{siguienteCodigo("CON", codigos), nombreLimpio, zona, true};
    conductores_.push_back(conductor);
    if (!repositorio_.guardarConductores(conductores_, error)) {
        conductores_.pop_back();
        return {};
    }
    std::string errorBitacora;
    bitacora_.registrar("REGISTRAR_CONDUCTOR", conductor.codigo,
                        "Conductor asignado a zona " + toString(zona), errorBitacora);
    return conductor.codigo;
}

Conductor* ConductorService::buscar(const std::string& codigo) {
    const auto normalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(conductores_.begin(), conductores_.end(), [&](const Conductor& c) {
        return aMayusculas(c.codigo) == normalizado;
    });
    return it == conductores_.end() ? nullptr : &*it;
}

const Conductor* ConductorService::buscar(const std::string& codigo) const {
    const auto normalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(conductores_.cbegin(), conductores_.cend(), [&](const Conductor& c) {
        return aMayusculas(c.codigo) == normalizado;
    });
    return it == conductores_.cend() ? nullptr : &*it;
}

std::vector<const Conductor*> ConductorService::activosPorZona(Zona zona) const {
    std::vector<const Conductor*> resultado;
    for (const auto& c : conductores_) {
        if (c.activo && c.zonaAsignada == zona) resultado.push_back(&c);
    }
    return resultado;
}

const std::vector<Conductor>& ConductorService::listar() const { return conductores_; }

