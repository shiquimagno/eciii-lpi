#pragma once

#include "modelos/Modelos.h"

#include <filesystem>
#include <string>
#include <vector>

class ReporteService {
public:
    ReporteService(std::filesystem::path raiz, const std::vector<Pedido>& pedidos,
                   const std::vector<Conductor>& conductores,
                   const std::vector<RutaEntrega>& rutas,
                   const std::vector<EventoPedido>& eventos);

    bool generarTodos(std::string& error) const;

private:
    std::filesystem::path directorio_;
    const std::vector<Pedido>& pedidos_;
    const std::vector<Conductor>& conductores_;
    const std::vector<RutaEntrega>& rutas_;
    const std::vector<EventoPedido>& eventos_;

    bool generarLista(const std::filesystem::path& archivo,
                      std::vector<const Pedido*> pedidos, std::string& error) const;
    bool generarRutas(std::string& error) const;
    bool generarIndividuales(std::string& error) const;
};

