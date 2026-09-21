#include "reportes/ReporteService.h"

#include "persistencia/CsvRepository.h"

#include <algorithm>
#include <fstream>
#include <unordered_map>

namespace {
bool abrir(const std::filesystem::path& archivo, std::ofstream& salida, std::string& error) {
    salida.open(archivo, std::ios::binary | std::ios::trunc);
    if (!salida) {
        error = "No se pudo generar el reporte: " + archivo.string();
        return false;
    }
    return true;
}
}

ReporteService::ReporteService(std::filesystem::path raiz, const std::vector<Pedido>& pedidos,
                               const std::vector<Conductor>& conductores,
                               const std::vector<RutaEntrega>& rutas,
                               const std::vector<EventoPedido>& eventos)
    : directorio_(std::move(raiz) / "reportes" / "csv"), pedidos_(pedidos),
      conductores_(conductores), rutas_(rutas), eventos_(eventos) {}

bool ReporteService::generarLista(const std::filesystem::path& archivo,
                                  std::vector<const Pedido*> pedidos,
                                  std::string& error) const {
    std::ofstream salida;
    if (!abrir(archivo, salida, error)) return false;
    salida << "codigo,cliente,direccion,zona,estado,conductor,ruta,fecha_registro,fecha_despacho,fecha_entrega\n";
    for (const auto* p : pedidos) {
        salida << CsvRepository::escapar(p->codigo) << ',' << CsvRepository::escapar(p->cliente)
               << ',' << CsvRepository::escapar(p->direccion) << ',' << toString(p->zona) << ','
               << toString(p->estado) << ',' << CsvRepository::escapar(p->codigoConductor) << ','
               << CsvRepository::escapar(p->codigoRuta) << ','
               << CsvRepository::escapar(p->fechaRegistro) << ','
               << CsvRepository::escapar(p->fechaDespacho) << ','
               << CsvRepository::escapar(p->fechaEntrega) << '\n';
    }
    return static_cast<bool>(salida);
}

bool ReporteService::generarRutas(std::string& error) const {
    std::ofstream salida;
    if (!abrir(directorio_ / "rutas_detalle.csv", salida, error)) return false;
    salida << "ruta,zona,estado_ruta,conductor,pedido,cliente,estado_pedido,fecha_creacion\n";
    for (const auto& ruta : rutas_) {
        for (const auto& codigoPedido : ruta.codigosPedidos) {
            const auto it = std::find_if(pedidos_.begin(), pedidos_.end(), [&](const Pedido& p) {
                return p.codigo == codigoPedido;
            });
            salida << CsvRepository::escapar(ruta.codigo) << ',' << toString(ruta.zona) << ','
                   << toString(ruta.estado) << ',' << CsvRepository::escapar(ruta.codigoConductor)
                   << ',' << CsvRepository::escapar(codigoPedido) << ','
                   << CsvRepository::escapar(it == pedidos_.end() ? "" : it->cliente) << ','
                   << (it == pedidos_.end() ? "NO_DISPONIBLE" : toString(it->estado)) << ','
                   << CsvRepository::escapar(ruta.fechaCreacion) << '\n';
        }
    }
    return static_cast<bool>(salida);
}

bool ReporteService::generarIndividuales(std::string& error) const {
    for (const auto& pedido : pedidos_) {
        std::ofstream salida;
        if (!abrir(directorio_ / ("pedido_" + pedido.codigo + ".csv"), salida, error)) return false;
        salida << "codigo,cliente,direccion,zona,estado_actual,fecha_evento,estado_anterior,estado_nuevo,descripcion\n";
        bool tieneEventos = false;
        for (const auto& evento : eventos_) {
            if (evento.codigoPedido != pedido.codigo) continue;
            tieneEventos = true;
            salida << CsvRepository::escapar(pedido.codigo) << ','
                   << CsvRepository::escapar(pedido.cliente) << ','
                   << CsvRepository::escapar(pedido.direccion) << ',' << toString(pedido.zona) << ','
                   << toString(pedido.estado) << ',' << CsvRepository::escapar(evento.fecha) << ','
                   << CsvRepository::escapar(evento.estadoAnterior) << ','
                   << CsvRepository::escapar(evento.estadoNuevo) << ','
                   << CsvRepository::escapar(evento.descripcion) << '\n';
        }
        if (!tieneEventos) {
            salida << CsvRepository::escapar(pedido.codigo) << ','
                   << CsvRepository::escapar(pedido.cliente) << ','
                   << CsvRepository::escapar(pedido.direccion) << ',' << toString(pedido.zona) << ','
                   << toString(pedido.estado) << ",,,,\n";
        }
    }
    return true;
}

bool ReporteService::generarTodos(std::string& error) const {
    std::filesystem::create_directories(directorio_);
    std::vector<const Pedido*> base;
    for (const auto& pedido : pedidos_) base.push_back(&pedido);
    if (!generarLista(directorio_ / "pedidos_general.csv", base, error)) return false;

    auto porZona = base;
    std::stable_sort(porZona.begin(), porZona.end(), [](const Pedido* a, const Pedido* b) {
        return toString(a->zona) < toString(b->zona);
    });
    if (!generarLista(directorio_ / "pedidos_por_zona.csv", porZona, error)) return false;

    auto porEstado = base;
    std::stable_sort(porEstado.begin(), porEstado.end(), [](const Pedido* a, const Pedido* b) {
        return toString(a->estado) < toString(b->estado);
    });
    if (!generarLista(directorio_ / "pedidos_por_estado.csv", porEstado, error)) return false;

    auto porConductor = base;
    std::stable_sort(porConductor.begin(), porConductor.end(), [](const Pedido* a, const Pedido* b) {
        return a->codigoConductor < b->codigoConductor;
    });
    if (!generarLista(directorio_ / "pedidos_por_conductor.csv", porConductor, error)) return false;

    return generarRutas(error) && generarIndividuales(error);
}
