#include "persistencia/CsvRepository.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {
std::vector<std::vector<std::string>> leerFilas(const std::filesystem::path& archivo) {
    std::ifstream entrada(archivo, std::ios::binary);
    std::vector<std::vector<std::string>> filas;
    std::string linea;
    bool primera = true;
    while (std::getline(entrada, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (primera) {
            primera = false;
            if (linea.size() >= 3 && static_cast<unsigned char>(linea[0]) == 0xEF &&
                static_cast<unsigned char>(linea[1]) == 0xBB &&
                static_cast<unsigned char>(linea[2]) == 0xBF) {
                linea.erase(0, 3);
            }
            continue;
        }
        if (!linea.empty()) filas.push_back(CsvRepository::separarLinea(linea));
    }
    return filas;
}

bool abrirSalida(const std::filesystem::path& archivo, std::ofstream& salida,
                 std::string& error) {
    salida.open(archivo, std::ios::binary | std::ios::trunc);
    if (!salida) {
        error = "No se pudo escribir el archivo: " + archivo.string();
        return false;
    }
    return true;
}
}

CsvRepository::CsvRepository(std::filesystem::path raiz) : raiz_(std::move(raiz)) {}

void CsvRepository::inicializarDirectorios() const {
    std::filesystem::create_directories(datos());
    std::filesystem::create_directories(raiz_ / "logs");
    std::filesystem::create_directories(raiz_ / "reportes" / "csv");
    std::filesystem::create_directories(raiz_ / "reportes" / "html");
}

const std::filesystem::path& CsvRepository::raiz() const { return raiz_; }
std::filesystem::path CsvRepository::datos() const { return raiz_ / "data"; }

std::string CsvRepository::escapar(const std::string& valor) {
    if (valor.find_first_of(",\"\r\n") == std::string::npos) return valor;
    std::string resultado = "\"";
    for (char c : valor) {
        if (c == '\"') resultado += "\"\"";
        else resultado += c;
    }
    resultado += '"';
    return resultado;
}

std::vector<std::string> CsvRepository::separarLinea(const std::string& linea) {
    std::vector<std::string> campos;
    std::string actual;
    bool entreComillas = false;
    for (std::size_t i = 0; i < linea.size(); ++i) {
        const char c = linea[i];
        if (c == '"') {
            if (entreComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                actual += '"';
                ++i;
            } else {
                entreComillas = !entreComillas;
            }
        } else if (c == ',' && !entreComillas) {
            campos.push_back(actual);
            actual.clear();
        } else {
            actual += c;
        }
    }
    campos.push_back(actual);
    return campos;
}

std::vector<Pedido> CsvRepository::cargarPedidos() const {
    std::vector<Pedido> resultado;
    for (const auto& fila : leerFilas(datos() / "pedidos.csv")) {
        if (fila.size() < 10) continue;
        Pedido pedido;
        EstadoPedido estado;
        Zona zona;
        if (!parseZona(fila[3], zona) || !parseEstadoPedido(fila[4], estado)) continue;
        pedido.codigo = fila[0];
        pedido.cliente = fila[1];
        pedido.direccion = fila[2];
        pedido.zona = zona;
        pedido.estado = estado;
        pedido.codigoConductor = fila[5];
        pedido.codigoRuta = fila[6];
        pedido.fechaRegistro = fila[7];
        pedido.fechaDespacho = fila[8];
        pedido.fechaEntrega = fila[9];
        resultado.push_back(std::move(pedido));
    }
    return resultado;
}

std::vector<Conductor> CsvRepository::cargarConductores() const {
    std::vector<Conductor> resultado;
    for (const auto& fila : leerFilas(datos() / "conductores.csv")) {
        if (fila.size() < 4) continue;
        Zona zona;
        if (!parseZona(fila[2], zona)) continue;
        resultado.push_back({fila[0], fila[1], zona, fila[3] == "1" || fila[3] == "true"});
    }
    return resultado;
}

std::vector<RutaEntrega> CsvRepository::cargarRutas() const {
    std::vector<RutaEntrega> resultado;
    for (const auto& fila : leerFilas(datos() / "rutas.csv")) {
        if (fila.size() < 5) continue;
        Zona zona;
        EstadoRuta estado;
        if (!parseZona(fila[1], zona) || !parseEstadoRuta(fila[4], estado)) continue;
        resultado.push_back({fila[0], zona, fila[2], fila[3], estado, {}});
    }

    std::unordered_map<std::string, std::size_t> indices;
    for (std::size_t i = 0; i < resultado.size(); ++i) indices[resultado[i].codigo] = i;
    for (const auto& fila : leerFilas(datos() / "rutas_detalle.csv")) {
        if (fila.size() < 2) continue;
        const auto it = indices.find(fila[0]);
        if (it != indices.end()) resultado[it->second].codigosPedidos.push_back(fila[1]);
    }
    return resultado;
}

std::vector<EventoPedido> CsvRepository::cargarEventos() const {
    std::vector<EventoPedido> resultado;
    for (const auto& fila : leerFilas(datos() / "eventos.csv")) {
        if (fila.size() >= 5) resultado.push_back({fila[0], fila[1], fila[2], fila[3], fila[4]});
    }
    return resultado;
}

bool CsvRepository::guardarPedidos(const std::vector<Pedido>& pedidos, std::string& error) const {
    std::ofstream salida;
    if (!abrirSalida(datos() / "pedidos.csv", salida, error)) return false;
    salida << "codigo,cliente,direccion,zona,estado,codigo_conductor,codigo_ruta,fecha_registro,fecha_despacho,fecha_entrega\n";
    for (const auto& p : pedidos) {
        salida << escapar(p.codigo) << ',' << escapar(p.cliente) << ',' << escapar(p.direccion) << ','
               << toString(p.zona) << ',' << toString(p.estado) << ',' << escapar(p.codigoConductor) << ','
               << escapar(p.codigoRuta) << ',' << escapar(p.fechaRegistro) << ','
               << escapar(p.fechaDespacho) << ',' << escapar(p.fechaEntrega) << '\n';
    }
    return static_cast<bool>(salida);
}

bool CsvRepository::guardarConductores(const std::vector<Conductor>& conductores, std::string& error) const {
    std::ofstream salida;
    if (!abrirSalida(datos() / "conductores.csv", salida, error)) return false;
    salida << "codigo,nombre,zona,activo\n";
    for (const auto& c : conductores) {
        salida << escapar(c.codigo) << ',' << escapar(c.nombre) << ',' << toString(c.zonaAsignada)
               << ',' << (c.activo ? "1" : "0") << '\n';
    }
    return static_cast<bool>(salida);
}

bool CsvRepository::guardarRutas(const std::vector<RutaEntrega>& rutas, std::string& error) const {
    std::ofstream cabecera;
    if (!abrirSalida(datos() / "rutas.csv", cabecera, error)) return false;
    cabecera << "codigo,zona,codigo_conductor,fecha_creacion,estado\n";
    for (const auto& r : rutas) {
        cabecera << escapar(r.codigo) << ',' << toString(r.zona) << ',' << escapar(r.codigoConductor)
                 << ',' << escapar(r.fechaCreacion) << ',' << toString(r.estado) << '\n';
    }
    cabecera.close();
    std::ofstream detalle;
    if (!abrirSalida(datos() / "rutas_detalle.csv", detalle, error)) return false;
    detalle << "codigo_ruta,codigo_pedido\n";
    for (const auto& r : rutas) {
        for (const auto& codigoPedido : r.codigosPedidos) {
            detalle << escapar(r.codigo) << ',' << escapar(codigoPedido) << '\n';
        }
    }
    return static_cast<bool>(detalle);
}

bool CsvRepository::guardarEventos(const std::vector<EventoPedido>& eventos, std::string& error) const {
    std::ofstream salida;
    if (!abrirSalida(datos() / "eventos.csv", salida, error)) return false;
    salida << "codigo_pedido,fecha,estado_anterior,estado_nuevo,descripcion\n";
    for (const auto& e : eventos) {
        salida << escapar(e.codigoPedido) << ',' << escapar(e.fecha) << ','
               << escapar(e.estadoAnterior) << ',' << escapar(e.estadoNuevo) << ','
               << escapar(e.descripcion) << '\n';
    }
    return static_cast<bool>(salida);
}

