#include "modelos/Modelos.h"

#include <algorithm>
#include <cctype>

namespace {
std::string mayusculas(std::string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return texto;
}
}

std::string toString(Zona zona) {
    switch (zona) {
        case Zona::NORTE: return "NORTE";
        case Zona::SUR: return "SUR";
        case Zona::CENTRO: return "CENTRO";
        case Zona::ESTE: return "ESTE";
    }
    return "CENTRO";
}

std::string toString(EstadoPedido estado) {
    switch (estado) {
        case EstadoPedido::PENDIENTE: return "PENDIENTE";
        case EstadoPedido::EN_CAMINO: return "EN_CAMINO";
        case EstadoPedido::ENTREGADO: return "ENTREGADO";
    }
    return "PENDIENTE";
}

std::string toString(EstadoRuta estado) {
    return estado == EstadoRuta::COMPLETADA ? "COMPLETADA" : "CREADA";
}

bool parseZona(const std::string& texto, Zona& zona) {
    const auto valor = mayusculas(texto);
    if (valor == "NORTE") zona = Zona::NORTE;
    else if (valor == "SUR") zona = Zona::SUR;
    else if (valor == "CENTRO") zona = Zona::CENTRO;
    else if (valor == "ESTE") zona = Zona::ESTE;
    else return false;
    return true;
}

bool parseEstadoPedido(const std::string& texto, EstadoPedido& estado) {
    const auto valor = mayusculas(texto);
    if (valor == "PENDIENTE") estado = EstadoPedido::PENDIENTE;
    else if (valor == "EN_CAMINO") estado = EstadoPedido::EN_CAMINO;
    else if (valor == "ENTREGADO") estado = EstadoPedido::ENTREGADO;
    else return false;
    return true;
}

bool parseEstadoRuta(const std::string& texto, EstadoRuta& estado) {
    const auto valor = mayusculas(texto);
    if (valor == "CREADA") estado = EstadoRuta::CREADA;
    else if (valor == "COMPLETADA") estado = EstadoRuta::COMPLETADA;
    else return false;
    return true;
}

