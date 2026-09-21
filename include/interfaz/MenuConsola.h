#pragma once

#include "modelos/Modelos.h"
#include "persistencia/BitacoraService.h"
#include "persistencia/CsvRepository.h"
#include "reportes/CsvHtmlExporter.h"
#include "reportes/ReporteService.h"
#include "servicios/ConductorService.h"
#include "servicios/PedidoService.h"
#include "servicios/RutaService.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class MenuConsola {
public:
    explicit MenuConsola(std::filesystem::path raiz);
    void ejecutar();

private:
    std::filesystem::path raiz_;
    CsvRepository repositorio_;
    BitacoraService bitacora_;
    std::vector<Pedido> pedidos_;
    std::vector<Conductor> conductores_;
    std::vector<RutaEntrega> rutas_;
    std::vector<EventoPedido> eventos_;
    PedidoService pedidoService_;
    ConductorService conductorService_;
    RutaService rutaService_;
    ReporteService reporteService_;
    CsvHtmlExporter htmlExporter_;

    static std::string leerTexto(const std::string& mensaje);
    static int leerEntero(const std::string& mensaje, int minimo, int maximo);
    static std::optional<Zona> seleccionarZona();
    static void mostrarPedido(const Pedido& pedido);

    void mostrarMenu() const;
    void registrarPedido();
    void listarPedidos() const;
    void buscarPedido() const;
    void actualizarPedido();
    void eliminarPedido();
    void registrarConductor();
    void listarConductores() const;
    void crearRuta();
    void consultarRuta() const;
    void marcarEntregado();
    void consultarFiltrado() const;
    void generarReportes() const;
    void exportarHtml() const;
};

