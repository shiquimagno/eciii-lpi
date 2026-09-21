#pragma once

#include "modelos/Modelos.h"

#include <string>
#include <vector>

class CsvRepository;
class BitacoraService;

class RutaService {
public:
    RutaService(std::vector<Pedido>& pedidos, std::vector<Conductor>& conductores,
                std::vector<RutaEntrega>& rutas, std::vector<EventoPedido>& eventos,
                CsvRepository& repositorio, BitacoraService& bitacora);

    std::string crear(Zona zona, const std::string& codigoConductor, std::string& error);
    bool marcarEntregado(const std::string& codigoPedido, std::string& error);
    RutaEntrega* buscar(const std::string& codigo);
    const RutaEntrega* buscar(const std::string& codigo) const;
    std::vector<const Pedido*> pedidosDeRuta(const std::string& codigoRuta) const;
    const std::vector<RutaEntrega>& listar() const;

private:
    std::vector<Pedido>& pedidos_;
    std::vector<Conductor>& conductores_;
    std::vector<RutaEntrega>& rutas_;
    std::vector<EventoPedido>& eventos_;
    CsvRepository& repositorio_;
    BitacoraService& bitacora_;

    bool persistir(std::string& error) const;
    void actualizarEstadoRuta(RutaEntrega& ruta);
};

