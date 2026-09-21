#pragma once

#include <string>
#include <vector>

std::string fechaHoraActual();
std::string recortar(const std::string& texto);
std::string aMayusculas(std::string texto);
std::string siguienteCodigo(const std::string& prefijo,
                            const std::vector<std::string>& codigos);

