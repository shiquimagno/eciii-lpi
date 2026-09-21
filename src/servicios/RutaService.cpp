#include "servicios/RutaService.h"

#include "persistencia/BitacoraService.h"
#include "persistencia/CsvRepository.h"
#include "servicios/Utilidades.h"

#include <algorithm>

RutaService::RutaService(std::vector<Pedido>& pedidos, std::vector<Conductor>& conductores,
                         std::vector<RutaEntrega>& rutas, std::vector<EventoPedido>& eventos,
                         CsvRepository& repositorio, BitacoraService& bitacora)
    : pedidos_(pedidos), conductores_(conductores), rutas_(rutas), eventos_(eventos),
      repositorio_(repositorio), bitacora_(bitacora) {}

bool RutaService::persistir(std::string& error) const {
    return repositorio_.guardarPedidos(pedidos_, error) &&
           repositorio_.guardarRutas(rutas_, error) &&
           repositorio_.guardarEventos(eventos_, error);
}

std::string RutaService::crear(Zona zona, const std::string& codigoConductor, std::string& error) {
    const auto codigoNormalizado = aMayusculas(recortar(codigoConductor));
    const auto conductor = std::find_if(conductores_.begin(), conductores_.end(),
                                        [&](const Conductor& c) {
        return aMayusculas(c.codigo) == codigoNormalizado;
    });
    if (conductor == conductores_.end() || !conductor->activo) {
        error = "El conductor no existe o está inactivo.";
        return {};
    }
    if (conductor->zonaAsignada != zona) {
        error = "El conductor no pertenece a la zona seleccionada.";
        return {};
    }
    std::vector<Pedido*> seleccionados;
    for (auto& pedido : pedidos_) {
        if (pedido.zona == zona && pedido.estado == EstadoPedido::PENDIENTE) {
            seleccionados.push_back(&pedido);
        }
    }
    if (seleccionados.empty()) {
        error = "No hay pedidos pendientes en la zona seleccionada.";
        return {};
    }
    std::vector<std::string> codigosRutas;
    for (const auto& r : rutas_) codigosRutas.push_back(r.codigo);
    RutaEntrega ruta;
    ruta.codigo = siguienteCodigo("RUT", codigosRutas);
    ruta.zona = zona;
    ruta.codigoConductor = conductor->codigo;
    ruta.fechaCreacion = fechaHoraActual();
    for (auto* pedido : seleccionados) {
        pedido->estado = EstadoPedido::EN_CAMINO;
        pedido->codigoConductor = conductor->codigo;
        pedido->codigoRuta = ruta.codigo;
        pedido->fechaDespacho = ruta.fechaCreacion;
        ruta.codigosPedidos.push_back(pedido->codigo);
        eventos_.push_back({pedido->codigo, ruta.fechaCreacion, "PENDIENTE", "EN_CAMINO",
                            "Asignado a " + ruta.codigo + " con " + conductor->codigo});
    }
    rutas_.push_back(ruta);
    if (!persistir(error)) return {};
    std::string errorBitacora;
    bitacora_.registrar("CREAR_RUTA", ruta.codigo,
                        "Zona " + toString(zona) + ", conductor " + conductor->codigo +
                            ", pedidos " + std::to_string(ruta.codigosPedidos.size()),
                        errorBitacora);
    return ruta.codigo;
}

bool RutaService::marcarEntregado(const std::string& codigoPedido, std::string& error) {
    const auto codigoNormalizado = aMayusculas(recortar(codigoPedido));
    const auto pedido = std::find_if(pedidos_.begin(), pedidos_.end(), [&](const Pedido& p) {
        return aMayusculas(p.codigo) == codigoNormalizado;
    });
    if (pedido == pedidos_.end()) {
        error = "Pedido no encontrado.";
        return false;
    }
    if (pedido->estado != EstadoPedido::EN_CAMINO) {
        error = "Solo un pedido EN_CAMINO puede marcarse como ENTREGADO.";
        return false;
    }
    pedido->estado = EstadoPedido::ENTREGADO;
    pedido->fechaEntrega = fechaHoraActual();
    eventos_.push_back({pedido->codigo, pedido->fechaEntrega, "EN_CAMINO", "ENTREGADO",
                        "Entrega confirmada"});
    RutaEntrega* ruta = buscar(pedido->codigoRuta);
    if (ruta) actualizarEstadoRuta(*ruta);
    if (!persistir(error)) return false;
    std::string errorBitacora;
    bitacora_.registrar("ENTREGAR_PEDIDO", pedido->codigo,
                        "Entrega confirmada en ruta " + pedido->codigoRuta, errorBitacora);
    return true;
}

void RutaService::actualizarEstadoRuta(RutaEntrega& ruta) {
    const bool completa = std::all_of(ruta.codigosPedidos.begin(), ruta.codigosPedidos.end(),
                                      [&](const std::string& codigo) {
        const auto pedido = std::find_if(pedidos_.begin(), pedidos_.end(), [&](const Pedido& p) {
            return p.codigo == codigo;
        });
        return pedido != pedidos_.end() && pedido->estado == EstadoPedido::ENTREGADO;
    });
    if (completa) ruta.estado = EstadoRuta::COMPLETADA;
}

RutaEntrega* RutaService::buscar(const std::string& codigo) {
    const auto normalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(rutas_.begin(), rutas_.end(), [&](const RutaEntrega& r) {
        return aMayusculas(r.codigo) == normalizado;
    });
    return it == rutas_.end() ? nullptr : &*it;
}

const RutaEntrega* RutaService::buscar(const std::string& codigo) const {
    const auto normalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(rutas_.cbegin(), rutas_.cend(), [&](const RutaEntrega& r) {
        return aMayusculas(r.codigo) == normalizado;
    });
    return it == rutas_.cend() ? nullptr : &*it;
}

std::vector<const Pedido*> RutaService::pedidosDeRuta(const std::string& codigoRuta) const {
    std::vector<const Pedido*> resultado;
    const auto* ruta = buscar(codigoRuta);
    if (!ruta) return resultado;
    for (const auto& codigoPedido : ruta->codigosPedidos) {
        const auto it = std::find_if(pedidos_.cbegin(), pedidos_.cend(), [&](const Pedido& p) {
            return p.codigo == codigoPedido;
        });
        if (it != pedidos_.cend()) resultado.push_back(&*it);
    }
    return resultado;
}

const std::vector<RutaEntrega>& RutaService::listar() const { return rutas_; }
