#pragma once

#include "modelos/Modelos.h"

#include <filesystem>
#include <string>
#include <vector>

class CsvRepository {
public:
    explicit CsvRepository(std::filesystem::path raiz);

    void inicializarDirectorios() const;

    std::vector<Pedido> cargarPedidos() const;
    std::vector<Conductor> cargarConductores() const;
    std::vector<RutaEntrega> cargarRutas() const;
    std::vector<EventoPedido> cargarEventos() const;

    bool guardarPedidos(const std::vector<Pedido>& pedidos, std::string& error) const;
    bool guardarConductores(const std::vector<Conductor>& conductores, std::string& error) const;
    bool guardarRutas(const std::vector<RutaEntrega>& rutas, std::string& error) const;
    bool guardarEventos(const std::vector<EventoPedido>& eventos, std::string& error) const;

    const std::filesystem::path& raiz() const;

    static std::string escapar(const std::string& valor);
    static std::vector<std::string> separarLinea(const std::string& linea);

private:
    std::filesystem::path raiz_;
    std::filesystem::path datos() const;
};

