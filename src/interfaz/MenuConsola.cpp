#include "interfaz/MenuConsola.h"

#include "servicios/Utilidades.h"

#include <algorithm>
#include <iostream>
#include <limits>

MenuConsola::MenuConsola(std::filesystem::path raiz)
    : raiz_(std::move(raiz)), repositorio_(raiz_), bitacora_(raiz_),
      pedidoService_(pedidos_, eventos_, repositorio_, bitacora_),
      conductorService_(conductores_, repositorio_, bitacora_),
      rutaService_(pedidos_, conductores_, rutas_, eventos_, repositorio_, bitacora_),
      reporteService_(raiz_, pedidos_, conductores_, rutas_, eventos_), htmlExporter_(raiz_) {
    repositorio_.inicializarDirectorios();
    pedidos_ = repositorio_.cargarPedidos();
    conductores_ = repositorio_.cargarConductores();
    rutas_ = repositorio_.cargarRutas();
    eventos_ = repositorio_.cargarEventos();

    std::string error;
    if (!std::filesystem::exists(raiz_ / "data" / "pedidos.csv")) repositorio_.guardarPedidos(pedidos_, error);
    if (!std::filesystem::exists(raiz_ / "data" / "conductores.csv")) repositorio_.guardarConductores(conductores_, error);
    if (!std::filesystem::exists(raiz_ / "data" / "rutas.csv")) repositorio_.guardarRutas(rutas_, error);
    if (!std::filesystem::exists(raiz_ / "data" / "eventos.csv")) repositorio_.guardarEventos(eventos_, error);
}

std::string MenuConsola::leerTexto(const std::string& mensaje) {
    std::cout << mensaje;
    std::string valor;
    std::getline(std::cin, valor);
    return recortar(valor);
}

int MenuConsola::leerEntero(const std::string& mensaje, int minimo, int maximo) {
    while (true) {
        const auto texto = leerTexto(mensaje);
        try {
            std::size_t usados = 0;
            const int valor = std::stoi(texto, &usados);
            if (usados == texto.size() && valor >= minimo && valor <= maximo) return valor;
        } catch (...) {
        }
        std::cout << "Entrada inválida. Ingrese un número entre " << minimo << " y " << maximo << ".\n";
    }
}

std::optional<Zona> MenuConsola::seleccionarZona() {
    std::cout << "1. NORTE\n2. SUR\n3. CENTRO\n4. ESTE\n0. Cancelar\n";
    const int opcion = leerEntero("Seleccione zona: ", 0, 4);
    switch (opcion) {
        case 1: return Zona::NORTE;
        case 2: return Zona::SUR;
        case 3: return Zona::CENTRO;
        case 4: return Zona::ESTE;
        default: return std::nullopt;
    }
}

void MenuConsola::mostrarPedido(const Pedido& p) {
    std::cout << p.codigo << " | " << p.cliente << " | " << p.direccion << " | "
              << toString(p.zona) << " | " << toString(p.estado);
    if (!p.codigoConductor.empty()) std::cout << " | Conductor: " << p.codigoConductor;
    if (!p.codigoRuta.empty()) std::cout << " | Ruta: " << p.codigoRuta;
    std::cout << '\n';
}

void MenuConsola::mostrarMenu() const {
    std::cout << "\n========== ROUTETRACK ==========\n"
              << "1. Registrar pedido\n2. Listar pedidos\n3. Buscar pedido\n"
              << "4. Actualizar pedido\n5. Eliminar pedido pendiente\n"
              << "6. Registrar conductor\n7. Listar conductores\n"
              << "8. Crear ruta por zona\n9. Consultar ruta\n"
              << "10. Marcar pedido como entregado\n11. Consultar pedidos\n"
              << "12. Generar reportes CSV\n13. Exportar CSV a HTML\n14. Salir\n";
}

void MenuConsola::registrarPedido() {
    const auto cliente = leerTexto("Cliente: ");
    const auto direccion = leerTexto("Dirección: ");
    const auto zona = seleccionarZona();
    if (!zona) return;
    std::string error;
    const auto codigo = pedidoService_.registrar(cliente, direccion, *zona, error);
    if (codigo.empty()) std::cout << "Error: " << error << '\n';
    else std::cout << "Pedido registrado con código " << codigo << ".\n";
}

void MenuConsola::listarPedidos() const {
    if (pedidos_.empty()) {
        std::cout << "No hay pedidos registrados.\n";
        return;
    }
    for (const auto& pedido : pedidos_) mostrarPedido(pedido);
}

void MenuConsola::buscarPedido() const {
    const auto codigo = leerTexto("Código del pedido: ");
    const auto* pedido = pedidoService_.buscar(codigo);
    if (!pedido) std::cout << "Pedido no encontrado.\n";
    else mostrarPedido(*pedido);
}

void MenuConsola::actualizarPedido() {
    const auto codigo = leerTexto("Código del pedido: ");
    const auto* actual = pedidoService_.buscar(codigo);
    if (!actual) {
        std::cout << "Pedido no encontrado.\n";
        return;
    }
    mostrarPedido(*actual);
    const auto cliente = leerTexto("Nuevo cliente: ");
    const auto direccion = leerTexto("Nueva dirección: ");
    const auto zona = seleccionarZona();
    if (!zona) return;
    std::string error;
    if (pedidoService_.actualizar(codigo, cliente, direccion, *zona, error))
        std::cout << "Pedido actualizado.\n";
    else std::cout << "Error: " << error << '\n';
}

void MenuConsola::eliminarPedido() {
    const auto codigo = leerTexto("Código del pedido pendiente: ");
    const auto confirmacion = aMayusculas(leerTexto("Escriba SI para confirmar: "));
    if (confirmacion != "SI") {
        std::cout << "Operación cancelada.\n";
        return;
    }
    std::string error;
    if (pedidoService_.eliminar(codigo, error)) std::cout << "Pedido eliminado.\n";
    else std::cout << "Error: " << error << '\n';
}

void MenuConsola::registrarConductor() {
    const auto nombre = leerTexto("Nombre del conductor: ");
    const auto zona = seleccionarZona();
    if (!zona) return;
    std::string error;
    const auto codigo = conductorService_.registrar(nombre, *zona, error);
    if (codigo.empty()) std::cout << "Error: " << error << '\n';
    else std::cout << "Conductor registrado con código " << codigo << ".\n";
}

void MenuConsola::listarConductores() const {
    if (conductores_.empty()) {
        std::cout << "No hay conductores registrados.\n";
        return;
    }
    for (const auto& c : conductores_) {
        std::cout << c.codigo << " | " << c.nombre << " | " << toString(c.zonaAsignada)
                  << " | " << (c.activo ? "ACTIVO" : "INACTIVO") << '\n';
    }
}

void MenuConsola::crearRuta() {
    const auto zona = seleccionarZona();
    if (!zona) return;
    const auto disponibles = conductorService_.activosPorZona(*zona);
    if (disponibles.empty()) {
        std::cout << "No hay conductores activos en esa zona.\n";
        return;
    }
    std::cout << "Conductores disponibles:\n";
    for (const auto* c : disponibles) std::cout << c->codigo << " | " << c->nombre << '\n';
    const auto conductor = leerTexto("Código del conductor: ");
    std::string error;
    const auto codigoRuta = rutaService_.crear(*zona, conductor, error);
    if (codigoRuta.empty()) std::cout << "Error: " << error << '\n';
    else std::cout << "Ruta " << codigoRuta << " creada correctamente.\n";
}

void MenuConsola::consultarRuta() const {
    const auto codigo = leerTexto("Código de ruta: ");
    const auto* ruta = rutaService_.buscar(codigo);
    if (!ruta) {
        std::cout << "Ruta no encontrada.\n";
        return;
    }
    std::cout << ruta->codigo << " | Zona: " << toString(ruta->zona)
              << " | Conductor: " << ruta->codigoConductor
              << " | Estado: " << toString(ruta->estado) << '\n';
    for (const auto* pedido : rutaService_.pedidosDeRuta(ruta->codigo)) mostrarPedido(*pedido);
}

void MenuConsola::marcarEntregado() {
    const auto codigo = leerTexto("Código del pedido: ");
    std::string error;
    if (rutaService_.marcarEntregado(codigo, error)) std::cout << "Entrega confirmada.\n";
    else std::cout << "Error: " << error << '\n';
}

void MenuConsola::consultarFiltrado() const {
    std::cout << "1. Por zona\n2. Por estado\n3. Por conductor\n0. Cancelar\n";
    const int opcion = leerEntero("Seleccione consulta: ", 0, 3);
    if (opcion == 0) return;
    bool encontrado = false;
    if (opcion == 1) {
        const auto zona = seleccionarZona();
        if (!zona) return;
        for (const auto& p : pedidos_) if (p.zona == *zona) { mostrarPedido(p); encontrado = true; }
    } else if (opcion == 2) {
        std::cout << "1. PENDIENTE\n2. EN_CAMINO\n3. ENTREGADO\n";
        const int estadoElegido = leerEntero("Seleccione estado: ", 1, 3);
        const EstadoPedido estado = estadoElegido == 1 ? EstadoPedido::PENDIENTE
                                     : estadoElegido == 2 ? EstadoPedido::EN_CAMINO
                                                         : EstadoPedido::ENTREGADO;
        for (const auto& p : pedidos_) if (p.estado == estado) { mostrarPedido(p); encontrado = true; }
    } else {
        const auto codigo = aMayusculas(leerTexto("Código del conductor: "));
        for (const auto& p : pedidos_) if (aMayusculas(p.codigoConductor) == codigo) { mostrarPedido(p); encontrado = true; }
    }
    if (!encontrado) std::cout << "La consulta no produjo resultados.\n";
}

void MenuConsola::generarReportes() const {
    std::string error;
    if (reporteService_.generarTodos(error))
        std::cout << "Reportes CSV generados en " << (raiz_ / "reportes" / "csv").string() << "\n";
    else std::cout << "Error: " << error << '\n';
}

void MenuConsola::exportarHtml() const {
    std::string error;
    if (htmlExporter_.exportarTodos(error))
        std::cout << "Reportes HTML generados. Abra "
                  << (raiz_ / "reportes" / "html" / "index.html").string() << "\n";
    else std::cout << "Error: " << error << '\n';
}

void MenuConsola::ejecutar() {
    while (true) {
        mostrarMenu();
        const int opcion = leerEntero("Opción: ", 1, 14);
        switch (opcion) {
            case 1: registrarPedido(); break;
            case 2: listarPedidos(); break;
            case 3: buscarPedido(); break;
            case 4: actualizarPedido(); break;
            case 5: eliminarPedido(); break;
            case 6: registrarConductor(); break;
            case 7: listarConductores(); break;
            case 8: crearRuta(); break;
            case 9: consultarRuta(); break;
            case 10: marcarEntregado(); break;
            case 11: consultarFiltrado(); break;
            case 12: generarReportes(); break;
            case 13: exportarHtml(); break;
            case 14: std::cout << "Hasta luego.\n"; return;
        }
    }
}

