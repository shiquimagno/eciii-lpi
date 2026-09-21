#include "servicios/Utilidades.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string fechaHoraActual() {
    const auto ahora = std::chrono::system_clock::now();
    const std::time_t tiempo = std::chrono::system_clock::to_time_t(ahora);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &tiempo);
#else
    localtime_r(&tiempo, &local);
#endif
    std::ostringstream salida;
    salida << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return salida.str();
}

std::string recortar(const std::string& texto) {
    const auto inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return {};
    const auto fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

std::string aMayusculas(std::string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return texto;
}

std::string siguienteCodigo(const std::string& prefijo,
                            const std::vector<std::string>& codigos) {
    int maximo = 0;
    const std::string marcador = prefijo + "-";
    for (const auto& codigo : codigos) {
        if (codigo.rfind(marcador, 0) != 0) continue;
        try {
            maximo = std::max(maximo, std::stoi(codigo.substr(marcador.size())));
        } catch (...) {
            // Los códigos externos inválidos no impiden generar el siguiente código válido.
        }
    }
    std::ostringstream salida;
    salida << prefijo << '-' << std::setw(4) << std::setfill('0') << (maximo + 1);
    return salida.str();
}
