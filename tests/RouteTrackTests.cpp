#include "persistencia/BitacoraService.h"
#include "persistencia/CsvRepository.h"
#include "reportes/CsvHtmlExporter.h"
#include "reportes/ReporteService.h"
#include "servicios/ConductorService.h"
#include "servicios/PedidoService.h"
#include "servicios/RutaService.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void exigir(bool condicion, const std::string& mensaje) {
    if (!condicion) throw std::runtime_error(mensaje);
}

std::string leerCompleto(const std::filesystem::path& archivo) {
    std::ifstream entrada(archivo, std::ios::binary);
    return {std::istreambuf_iterator<char>(entrada), std::istreambuf_iterator<char>()};
}
}

int main() {
    const auto raiz = std::filesystem::temp_directory_path() / "routetrack_pruebas_automatizadas";
    std::filesystem::remove_all(raiz);
    try {
        CsvRepository repositorio(raiz);
        repositorio.inicializarDirectorios();
        BitacoraService bitacora(raiz);
        std::vector<Pedido> pedidos;
        std::vector<Conductor> conductores;
        std::vector<RutaEntrega> rutas;
        std::vector<EventoPedido> eventos;
        PedidoService pedidoService(pedidos, eventos, repositorio, bitacora);
        ConductorService conductorService(conductores, repositorio, bitacora);
        RutaService rutaService(pedidos, conductores, rutas, eventos, repositorio, bitacora);

        std::string error;
        const auto conductorNorte = conductorService.registrar("Ana Torres", Zona::NORTE, error);
        const auto conductorSur = conductorService.registrar("Luis Pérez", Zona::SUR, error);
        exigir(conductorNorte == "CON-0001" && conductorSur == "CON-0002",
               "La generación de códigos de conductor falló.");

        const auto pedido1 = pedidoService.registrar("Cliente Uno", "Av. Norte, 123", Zona::NORTE, error);
        const auto pedido2 = pedidoService.registrar("Cliente <Dos>", "Jr. Prueba \"Puerta A\"", Zona::NORTE, error);
        const auto pedido3 = pedidoService.registrar("Cliente Tres", "Calle Sur 10", Zona::SUR, error);
        const auto temporal = pedidoService.registrar("Temporal", "Dirección temporal", Zona::ESTE, error);
        exigir(pedido1 == "PED-0001" && temporal == "PED-0004", "La generación de códigos de pedido falló.");
        exigir(pedidoService.buscar(pedido2) != nullptr, "La búsqueda de pedido falló.");
        exigir(pedidoService.actualizar(pedido3, "Cliente Tres Actualizado", "Calle Sur 20", Zona::SUR, error),
               "No se pudo actualizar un pedido pendiente.");
        exigir(pedidoService.eliminar(temporal, error), "No se pudo eliminar un pedido pendiente.");
        exigir(pedidoService.buscar(temporal) == nullptr, "El pedido eliminado continúa en memoria.");
        const auto posterior = pedidoService.registrar("Posterior", "Calle posterior", Zona::ESTE, error);
        exigir(posterior == "PED-0005", "Se reutilizó el código de un pedido eliminado.");
        exigir(pedidoService.eliminar(posterior, error), "No se pudo limpiar el pedido posterior.");

        exigir(rutaService.crear(Zona::NORTE, conductorSur, error).empty(),
               "Se aceptó un conductor de otra zona.");
        const auto ruta = rutaService.crear(Zona::NORTE, conductorNorte, error);
        exigir(ruta == "RUT-0001", "No se creó la ruta esperada.");
        exigir(rutaService.pedidosDeRuta(ruta).size() == 2,
               "La ruta no seleccionó exactamente los pedidos de su zona.");
        exigir(pedidoService.buscar(pedido1)->estado == EstadoPedido::EN_CAMINO,
               "El pedido no cambió a EN_CAMINO.");
        exigir(!pedidoService.eliminar(pedido1, error), "Se eliminó un pedido que ya estaba en camino.");
        exigir(rutaService.crear(Zona::NORTE, conductorNorte, error).empty(),
               "Se creó una ruta sin pedidos pendientes.");

        exigir(rutaService.marcarEntregado(pedido1, error), "No se confirmó la primera entrega.");
        exigir(rutaService.buscar(ruta)->estado == EstadoRuta::CREADA,
               "La ruta terminó antes de entregar todos sus pedidos.");
        exigir(rutaService.marcarEntregado(pedido2, error), "No se confirmó la segunda entrega.");
        exigir(rutaService.buscar(ruta)->estado == EstadoRuta::COMPLETADA,
               "La ruta no se completó automáticamente.");
        exigir(!rutaService.marcarEntregado(pedido2, error),
               "Se permitió entregar dos veces el mismo pedido.");

        const auto recargados = repositorio.cargarPedidos();
        const auto rutasRecargadas = repositorio.cargarRutas();
        exigir(recargados.size() == 3 && rutasRecargadas.size() == 1,
               "La recarga desde CSV no conservó los datos.");
        exigir(rutasRecargadas.front().codigosPedidos.size() == 2,
               "La recarga no reconstruyó los detalles de la ruta.");

        ReporteService reportes(raiz, pedidos, conductores, rutas, eventos);
        exigir(reportes.generarTodos(error), "No se generaron los reportes CSV: " + error);
        CsvHtmlExporter exportador(raiz);
        exigir(exportador.exportarTodos(error), "No se exportaron los reportes HTML: " + error);
        const auto html = leerCompleto(raiz / "reportes" / "html" / "pedidos_general.html");
        exigir(html.find("Cliente &lt;Dos&gt;") != std::string::npos,
               "El exportador HTML no escapó los caracteres especiales.");
        exigir(std::filesystem::exists(raiz / "reportes" / "html" / "index.html"),
               "No se creó el índice HTML.");

        std::cout << "Todas las pruebas de RouteTrack finalizaron correctamente.\n";
        std::filesystem::remove_all(raiz);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "PRUEBA FALLIDA: " << ex.what() << '\n';
        return 1;
    }
}
