#pragma once

#include <filesystem>
#include <string>

class CsvHtmlExporter {
public:
    explicit CsvHtmlExporter(std::filesystem::path raiz);
    bool exportarTodos(std::string& error) const;

private:
    std::filesystem::path csv_;
    std::filesystem::path html_;

    bool exportarArchivo(const std::filesystem::path& origen,
                         const std::filesystem::path& destino,
                         std::string& error) const;
    static std::string escaparHtml(const std::string& texto);
};

