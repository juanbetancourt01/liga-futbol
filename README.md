# Liga de Futbol

## Descripcion
Aplicacion de consola en C++ que gestiona una liga de futbol. Registra partidos,
calcula la tabla de posiciones y lleva historial de jornadas. El estado persiste
entre ejecuciones mediante archivos de texto.

## Compilar
g++ -o liga src/main.cpp

## Ejecutar
./liga

## Formato de config.txt
Archivo con pares clave=valor, uno por linea.
Las lineas que empiezan con # son comentarios.

Ejemplo:
liga=Premier League
ganar=3
empatar=1
perder=0
equipo=ManchesterCity
equipo=Arsenal

## Decisiones de diseno
- Se usan punteros en actualizarEstadisticas() para modificar directamente el struct Equipo
- Los partidos se persisten en partidos.txt con delimitador ; para facilitar el parseo
- Las jornadas se guardan en fechas.txt con bloques JORNADA=N / FIN_JORNADA
- config.txt define todo: nombre de liga, puntos y equipos. Sin ese archivo el programa no inicia
