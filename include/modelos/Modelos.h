#pragma once

#include <string>
#include <vector>

enum class Zona { NORTE, SUR, CENTRO, ESTE };
enum class EstadoPedido { PENDIENTE, EN_CAMINO, ENTREGADO };
enum class EstadoRuta { CREADA, COMPLETADA };

std::string toString(Zona zona);
std::string toString(EstadoPedido estado);
std::string toString(EstadoRuta estado);
bool parseZona(const std::string& texto, Zona& zona);
bool parseEstadoPedido(const std::string& texto, EstadoPedido& estado);
bool parseEstadoRuta(const std::string& texto, EstadoRuta& estado);

struct Pedido {
    std::string codigo;
    std::string cliente;
    std::string direccion;
    Zona zona{Zona::CENTRO};
    EstadoPedido estado{EstadoPedido::PENDIENTE};
    std::string codigoConductor;
    std::string codigoRuta;
    std::string fechaRegistro;
    std::string fechaDespacho;
    std::string fechaEntrega;
};

struct Conductor {
    std::string codigo;
    std::string nombre;
    Zona zonaAsignada{Zona::CENTRO};
    bool activo{true};
};

struct RutaEntrega {
    std::string codigo;
    Zona zona{Zona::CENTRO};
    std::string codigoConductor;
    std::string fechaCreacion;
    EstadoRuta estado{EstadoRuta::CREADA};
    std::vector<std::string> codigosPedidos;
};

struct EventoPedido {
    std::string codigoPedido;
    std::string fecha;
    std::string estadoAnterior;
    std::string estadoNuevo;
    std::string descripcion;
};

