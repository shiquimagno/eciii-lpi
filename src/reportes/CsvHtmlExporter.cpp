#include "reportes/CsvHtmlExporter.h"

#include "persistencia/CsvRepository.h"
#include "servicios/Utilidades.h"

#include <algorithm>
#include <fstream>
#include <vector>

CsvHtmlExporter::CsvHtmlExporter(std::filesystem::path raiz)
    : csv_(raiz / "reportes" / "csv"), html_(std::move(raiz) / "reportes" / "html") {}

std::string CsvHtmlExporter::escaparHtml(const std::string& texto) {
    std::string resultado;
    for (char c : texto) {
        switch (c) {
            case '&': resultado += "&amp;"; break;
            case '<': resultado += "&lt;"; break;
            case '>': resultado += "&gt;"; break;
            case '"': resultado += "&quot;"; break;
            case '\'': resultado += "&#39;"; break;
            default: resultado += c;
        }
    }
    return resultado;
}

bool CsvHtmlExporter::exportarArchivo(const std::filesystem::path& origen,
                                      const std::filesystem::path& destino,
                                      std::string& error) const {
    std::ifstream entrada(origen, std::ios::binary);
    if (!entrada) {
        error = "No se pudo leer el reporte CSV: " + origen.string();
        return false;
    }
    std::ofstream salida(destino, std::ios::binary | std::ios::trunc);
    if (!salida) {
        error = "No se pudo escribir el reporte HTML: " + destino.string();
        return false;
    }
    std::string titulo = origen.stem().string();
    std::replace(titulo.begin(), titulo.end(), '_', ' ');
    salida << "<!doctype html>\n<html lang=\"es\">\n<head>\n"
           << "<meta charset=\"utf-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
           << "<title>RouteTrack - " << escaparHtml(titulo) << "</title>\n"
           << "<link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n<main>\n"
           << "<a class=\"volver\" href=\"index.html\">← Volver al índice</a>\n"
           << "<h1>" << escaparHtml(titulo) << "</h1>\n"
           << "<p class=\"meta\">Reporte estático generado por RouteTrack.</p>\n<div class=\"tabla\"><table>\n";
    std::string linea;
    bool encabezado = true;
    while (std::getline(entrada, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        const auto campos = CsvRepository::separarLinea(linea);
        salida << (encabezado ? "<thead><tr>" : "<tbody><tr>");
        for (const auto& campo : campos) {
            salida << (encabezado ? "<th>" : "<td>") << escaparHtml(campo)
                   << (encabezado ? "</th>" : "</td>");
        }
        salida << (encabezado ? "</tr></thead>\n" : "</tr></tbody>\n");
        encabezado = false;
    }
    salida << "</table></div>\n</main>\n</body>\n</html>\n";
    return static_cast<bool>(salida);
}

bool CsvHtmlExporter::exportarTodos(std::string& error) const {
    if (!std::filesystem::exists(csv_)) {
        error = "No existe el directorio de reportes CSV. Genere los reportes primero.";
        return false;
    }
    std::filesystem::create_directories(html_);
    std::ofstream estilos(html_ / "styles.css", std::ios::binary | std::ios::trunc);
    if (!estilos) {
        error = "No se pudo crear styles.css.";
        return false;
    }
    estilos << "*{box-sizing:border-box}body{margin:0;background:#f4f7fb;color:#172033;font-family:Segoe UI,Arial,sans-serif}"
            << "main{width:min(1200px,94%);margin:40px auto;background:#fff;padding:32px;border-radius:14px;box-shadow:0 8px 28px #20304a18}"
            << "h1{margin:12px 0 4px;text-transform:capitalize;color:#174a7e}.meta{color:#64748b}.volver{color:#1769aa;text-decoration:none}"
            << ".tabla{overflow:auto;margin-top:24px}table{width:100%;border-collapse:collapse;font-size:14px}"
            << "th{background:#174a7e;color:#fff;text-align:left}th,td{padding:10px 12px;border:1px solid #dbe3ec;white-space:nowrap}"
            << "tbody:nth-child(even){background:#f5f9fd}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:16px;margin-top:24px}"
            << ".card{display:block;padding:20px;border:1px solid #dbe3ec;border-radius:10px;color:#174a7e;text-decoration:none;background:#f8fbff}"
            << ".card:hover{border-color:#1769aa;background:#eef7ff}";
    estilos.close();

    std::vector<std::filesystem::path> archivos;
    for (const auto& entrada : std::filesystem::directory_iterator(csv_)) {
        if (entrada.is_regular_file() && entrada.path().extension() == ".csv") archivos.push_back(entrada.path());
    }
    std::sort(archivos.begin(), archivos.end());
    if (archivos.empty()) {
        error = "No hay reportes CSV para exportar.";
        return false;
    }
    for (const auto& archivo : archivos) {
        if (!exportarArchivo(archivo, html_ / (archivo.stem().string() + ".html"), error)) return false;
    }
    std::ofstream indice(html_ / "index.html", std::ios::binary | std::ios::trunc);
    if (!indice) {
        error = "No se pudo crear el índice HTML.";
        return false;
    }
    indice << "<!doctype html><html lang=\"es\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
           << "<title>RouteTrack - Reportes</title><link rel=\"stylesheet\" href=\"styles.css\"></head><body><main>"
           << "<h1>Reportes RouteTrack</h1><p class=\"meta\">Archivos generados desde los reportes CSV.</p><div class=\"grid\">";
    for (const auto& archivo : archivos) {
        std::string titulo = archivo.stem().string();
        std::replace(titulo.begin(), titulo.end(), '_', ' ');
        indice << "<a class=\"card\" href=\"" << archivo.stem().string() << ".html\">"
               << escaparHtml(titulo) << "</a>";
    }
    indice << "</div></main></body></html>";
    return static_cast<bool>(indice);
}
