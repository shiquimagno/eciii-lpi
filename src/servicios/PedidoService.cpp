#include "servicios/PedidoService.h"

#include "persistencia/BitacoraService.h"
#include "persistencia/CsvRepository.h"
#include "servicios/Utilidades.h"

#include <algorithm>

PedidoService::PedidoService(std::vector<Pedido>& pedidos, std::vector<EventoPedido>& eventos,
                             CsvRepository& repositorio, BitacoraService& bitacora)
    : pedidos_(pedidos), eventos_(eventos), repositorio_(repositorio), bitacora_(bitacora) {}

std::string PedidoService::registrar(const std::string& cliente, const std::string& direccion,
                                     Zona zona, std::string& error) {
    const auto clienteLimpio = recortar(cliente);
    const auto direccionLimpia = recortar(direccion);
    if (clienteLimpio.empty() || direccionLimpia.empty()) {
        error = "El cliente y la dirección son obligatorios.";
        return {};
    }
    std::vector<std::string> codigos;
    for (const auto& p : pedidos_) codigos.push_back(p.codigo);
    Pedido pedido;
    pedido.codigo = siguienteCodigo("PED", codigos);
    pedido.cliente = clienteLimpio;
    pedido.direccion = direccionLimpia;
    pedido.zona = zona;
    pedido.fechaRegistro = fechaHoraActual();
    pedidos_.push_back(pedido);
    eventos_.push_back({pedido.codigo, pedido.fechaRegistro, "", "PENDIENTE", "Pedido registrado"});
    if (!repositorio_.guardarPedidos(pedidos_, error) ||
        !repositorio_.guardarEventos(eventos_, error)) {
        pedidos_.pop_back();
        eventos_.pop_back();
        return {};
    }
    std::string errorBitacora;
    bitacora_.registrar("REGISTRAR_PEDIDO", pedido.codigo, "Pedido creado en zona " + toString(zona), errorBitacora);
    return pedido.codigo;
}

Pedido* PedidoService::buscar(const std::string& codigo) {
    const auto codigoNormalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(pedidos_.begin(), pedidos_.end(), [&](const Pedido& p) {
        return aMayusculas(p.codigo) == codigoNormalizado;
    });
    return it == pedidos_.end() ? nullptr : &*it;
}

const Pedido* PedidoService::buscar(const std::string& codigo) const {
    const auto codigoNormalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(pedidos_.cbegin(), pedidos_.cend(), [&](const Pedido& p) {
        return aMayusculas(p.codigo) == codigoNormalizado;
    });
    return it == pedidos_.cend() ? nullptr : &*it;
}

bool PedidoService::actualizar(const std::string& codigo, const std::string& cliente,
                               const std::string& direccion, Zona zona, std::string& error) {
    Pedido* pedido = buscar(codigo);
    if (!pedido) {
        error = "Pedido no encontrado.";
        return false;
    }
    if (pedido->estado != EstadoPedido::PENDIENTE) {
        error = "Solo se pueden modificar pedidos pendientes.";
        return false;
    }
    const auto clienteLimpio = recortar(cliente);
    const auto direccionLimpia = recortar(direccion);
    if (clienteLimpio.empty() || direccionLimpia.empty()) {
        error = "El cliente y la dirección son obligatorios.";
        return false;
    }
    const Pedido anterior = *pedido;
    pedido->cliente = clienteLimpio;
    pedido->direccion = direccionLimpia;
    pedido->zona = zona;
    if (!repositorio_.guardarPedidos(pedidos_, error)) {
        *pedido = anterior;
        return false;
    }
    std::string errorBitacora;
    bitacora_.registrar("ACTUALIZAR_PEDIDO", pedido->codigo, "Datos descriptivos actualizados", errorBitacora);
    return true;
}

bool PedidoService::eliminar(const std::string& codigo, std::string& error) {
    const auto codigoNormalizado = aMayusculas(recortar(codigo));
    const auto it = std::find_if(pedidos_.begin(), pedidos_.end(), [&](const Pedido& p) {
        return aMayusculas(p.codigo) == codigoNormalizado;
    });
    if (it == pedidos_.end()) {
        error = "Pedido no encontrado.";
        return false;
    }
    if (it->estado != EstadoPedido::PENDIENTE) {
        error = "Solo se pueden eliminar pedidos pendientes.";
        return false;
    }
    const Pedido eliminado = *it;
    eventos_.push_back({eliminado.codigo, fechaHoraActual(), "PENDIENTE", "",
                        "Pedido eliminado"});
    pedidos_.erase(it);
    if (!repositorio_.guardarPedidos(pedidos_, error) ||
        !repositorio_.guardarEventos(eventos_, error)) {
        error = "No se pudo persistir por completo la eliminación: " + error;
        return false;
    }
    std::string errorBitacora;
    bitacora_.registrar("ELIMINAR_PEDIDO", eliminado.codigo, "Pedido pendiente eliminado", errorBitacora);
    return true;
}

const std::vector<Pedido>& PedidoService::listar() const { return pedidos_; }

