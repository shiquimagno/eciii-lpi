# Plan del proyecto RouteTrack

## Propósito

RouteTrack resuelve la asignación manual de pedidos mediante un prototipo de consola. Registra pedidos y conductores, agrupa pedidos por zona, crea despachos y conserva el historial de cada cambio.

En esta versión, una ruta es un despacho por zona. El sistema no promete una ruta geográfica óptima ni utiliza mapas, coordenadas o algoritmos de caminos mínimos.

## Arquitectura

La solución está dividida en:

1. **Modelos:** `Pedido`, `Conductor`, `RutaEntrega` y `EventoPedido`.
2. **Persistencia:** lectura y escritura de CSV, además de una bitácora TXT.
3. **Servicios:** reglas de pedidos, conductores y rutas.
4. **Reportes:** creación de CSV y conversión a HTML/CSS.
5. **Interfaz:** menú de consola y validación de entradas.

Los servicios operan sobre colecciones `std::vector`. El repositorio es la única pieza que conoce el formato físico de los CSV.

## Flujo funcional

1. El usuario registra un pedido con cliente, dirección y zona; el sistema genera su código y estado `PENDIENTE`.
2. El usuario registra un conductor para una zona.
3. Para crear una ruta, selecciona una zona y uno de sus conductores activos.
4. El sistema incluye todos los pedidos pendientes de esa zona y los cambia a `EN_CAMINO`.
5. El usuario confirma cada entrega y el pedido cambia a `ENTREGADO`.
6. Cuando todos los pedidos están entregados, la ruta cambia a `COMPLETADA`.
7. El usuario genera reportes CSV y después los exporta a HTML/CSS.

## Persistencia

- `pedidos.csv`: información actual de cada pedido.
- `conductores.csv`: catálogo simple de conductores.
- `rutas.csv`: cabecera de cada despacho.
- `rutas_detalle.csv`: relación entre rutas y pedidos.
- `eventos.csv`: trazabilidad estructurada.
- `bitacora.txt`: registro cronológico para lectura humana.

Los códigos se generan con los formatos `PED-0001`, `CON-0001` y `RUT-0001`. Los CSV usan comas, comillas dobles para campos especiales y codificación UTF-8.

## Reportes

Se generan reportes generales, por zona, por estado, por conductor, por ruta e individuales por pedido. Cada archivo HTML se produce leyendo su CSV correspondiente. El índice HTML enlaza todos los reportes disponibles.

## Criterios de aceptación

- CRUD completo de pedidos y persistencia después de cada cambio.
- Transiciones de estado válidas y rechazo de retrocesos.
- Asignación exclusiva por zona y conductor compatible.
- Recuperación de los datos al reiniciar.
- Registro de eventos y bitácora.
- Reportes CSV correctos y páginas HTML legibles sin servidor.
- Compilación C++17 mediante CMake y ejecución satisfactoria de `RouteTrackTests`.

## Límites

No se incluyen GPS, mapas, cálculo de distancias, optimización geográfica, tráfico, IA, base de datos, interfaz gráfica, aplicación móvil ni usuarios simultáneos.

