#pragma once

#include "modelos/Modelos.h"

#include <string>
#include <vector>

class CsvRepository;
class BitacoraService;

class PedidoService {
public:
    PedidoService(std::vector<Pedido>& pedidos, std::vector<EventoPedido>& eventos,
                  CsvRepository& repositorio, BitacoraService& bitacora);

    std::string registrar(const std::string& cliente, const std::string& direccion,
                          Zona zona, std::string& error);
    Pedido* buscar(const std::string& codigo);
    const Pedido* buscar(const std::string& codigo) const;
    bool actualizar(const std::string& codigo, const std::string& cliente,
                    const std::string& direccion, Zona zona, std::string& error);
    bool eliminar(const std::string& codigo, std::string& error);
    const std::vector<Pedido>& listar() const;

private:
    std::vector<Pedido>& pedidos_;
    std::vector<EventoPedido>& eventos_;
    CsvRepository& repositorio_;
    BitacoraService& bitacora_;
};

