# RouteTrack

Prototipo académico en C++17 para registrar pedidos, agruparlos por zona, asignarlos a un conductor y conservar su trazabilidad mediante archivos CSV, TXT y reportes HTML/CSS. Hola

## Reglas del proyecto

- La aplicación principal funciona en consola y aplica programación orientada a objetos.
- `std::vector` es la colección dinámica principal para pedidos, conductores, rutas y eventos.
- Una **ruta** representa un despacho por zona. No es navegación real y no calcula coordenadas, distancias ni caminos mínimos.
- Las zonas válidas son `NORTE`, `SUR`, `CENTRO` y `ESTE`.
- Los estados de un pedido siguen exclusivamente la secuencia `PENDIENTE -> EN_CAMINO -> ENTREGADO`.
- Los datos de cliente, dirección y zona solo se modifican mientras el pedido está pendiente.
- Solo se puede eliminar físicamente un pedido pendiente; la eliminación siempre se registra en la bitácora.
- Una ruta toma todos los pedidos pendientes de su zona y solo puede asignarse a un conductor activo de esa misma zona.
- La ruta se completa cuando todos sus pedidos están entregados.
- Cada operación válida se persiste inmediatamente. CSV es la fuente estructurada y `logs/bitacora.txt` es el registro legible.
- Los reportes HTML se generan a partir de reportes CSV y usan una única hoja `styles.css`.
- No se incorporan GPS, mapas, tráfico, IA, servidores, SQLite, Qt ni aplicaciones móviles.

## Organización

- `Base_proyecto/`: plan, rúbrica y guía de demostración.
- `include/` y `src/`: cabeceras e implementación por responsabilidad.
- `data/`: persistencia CSV de la aplicación.
- `logs/`: bitácora TXT.
- `reportes/csv/`: reportes estructurados.
- `reportes/html/`: páginas estáticas y estilos.
- `tests/`: pruebas automatizadas sin dependencias externas.

## Compilar con Visual Studio 2026

Desde PowerShell, en la raíz del repositorio:

```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
  -S . -B build -G "Visual Studio 18 2026" -A x64

& "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
  --build build --config Release
```

Ejecutables generados:

```text
build/Release/RouteTrack.exe
build/Release/RouteTrackTests.exe
```

## Ejecutar

```powershell
.\build\Release\RouteTrack.exe
.\build\Release\RouteTrackTests.exe
```

El programa crea automáticamente las carpetas y los encabezados de los archivos que falten. Todos los archivos se guardan en UTF-8.

## Convenciones de desarrollo

- Clases y enumeraciones: `PascalCase`.
- Funciones y variables: `camelCase`.
- Constantes: `MAYUSCULAS_CON_GUIONES_BAJOS`.
- Una responsabilidad principal por módulo.
- Validar las entradas antes de modificar las colecciones.
- No subir `build/`, binarios ni archivos temporales.
- Hacer commits pequeños con mensajes que describan el comportamiento incorporado.
