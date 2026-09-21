# Guía de demostración y sustentación

## Preparación

Compile en modo `Release`, ejecute `RouteTrackTests` y confirme que termina sin errores. Para una demostración limpia puede respaldar y vaciar manualmente los archivos de `data/`, `logs/` y `reportes/`.

## Recorrido sugerido

1. Presente el problema: los pedidos se asignaban manualmente y no existía trazabilidad.
2. Explique que una ruta representa un despacho por zona, no navegación geográfica.
3. Registre dos conductores en zonas distintas.
4. Registre al menos cuatro pedidos, incluyendo una dirección con coma.
5. Muestre el listado, la búsqueda y la actualización de un pedido pendiente.
6. Registre un pedido temporal y elimínelo para demostrar el CRUD completo.
7. Cree una ruta para una zona y compruebe que solo recibe pedidos pendientes de ella.
8. Muestre que los pedidos pasan automáticamente a `EN_CAMINO`.
9. Marque todos los pedidos como entregados y muestre que la ruta se completa.
10. Genere los reportes CSV y expórtelos a HTML.
11. Abra `reportes/html/index.html` y revise el reporte general y la trazabilidad individual.
12. Cierre y vuelva a ejecutar el programa para demostrar la persistencia.

## Conceptos que cada integrante debe dominar

- Por qué `std::vector` equivale a una colección dinámica tipo ArrayList.
- Diferencia entre modelo, servicio, persistencia e interfaz.
- Validación de estados y de zonas.
- Funcionamiento de `fstream` y escapado CSV.
- Relación entre los CSV y los HTML generados.
- Límites del prototipo y mejoras futuras posibles.
