#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct ConfigLiga {
    std::string nombre;
    int ptsGanar;
    int ptsEmpatar;
    int ptsPerder;
    std::vector<std::string> equipos;
};

struct Partido {
    std::string fecha;
    std::string local;
    std::string visitante;
    int golesLocal;
    int golesVisitante;
};

struct Equipo {
    std::string nombre;
    int jugados = 0;
    int ganados = 0;
    int empatados = 0;
    int perdidos = 0;
    int gf = 0;
    int gc = 0;
    int dg = 0;
    int puntos = 0;
};

bool leerConfig(const std::string& ruta, ConfigLiga& cfg) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo abrir " << ruta << "\n";
        return false;
    }
    bool tienNombre = false, tienGanar = false, tienEmpatar = false, tienPerder = false;
    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        std::string clave, valor;
        std::istringstream ss(linea);
        if (!std::getline(ss, clave, '=') || !std::getline(ss, valor)) continue;
        if (clave == "liga") { cfg.nombre = valor; tienNombre = true; }
        else if (clave == "ganar")    { cfg.ptsGanar   = std::stoi(valor); tienGanar   = true; } 
        else if (clave == "empatar")  { cfg.ptsEmpatar = std::stoi(valor); tienEmpatar = true; } 
        else if (clave == "perder")   { cfg.ptsPerder  = std::stoi(valor); tienPerder  = true; } 
        else if (clave == "equipo")   { cfg.equipos.push_back(valor); }
    }
    if (!tienNombre || !tienGanar || !tienEmpatar || !tienPerder || cfg.equipos.empty()) {
        std::cout << "Error: config.txt tiene formato invalido o faltan campos.\n";
        return false;
    }
    return true;
}

std::vector<Partido> leerPartidos(const std::string& ruta) {
    std::vector<Partido> lista;
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "Aviso: no se encontro " << ruta << ". Se asume sin partidos previos.\n"; 
        return lista;
    }
    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::istringstream ss(linea);
        Partido p;
        std::string gl, gv;
        if (!std::getline(ss, p.fecha,     ';')) continue;
        if (!std::getline(ss, p.local,     ';')) continue;
        if (!std::getline(ss, p.visitante, ';')) continue;
        if (!std::getline(ss, gl,          ';')) continue;
        if (!std::getline(ss, gv))               continue;
        p.golesLocal     = std::stoi(gl);
        p.golesVisitante = std::stoi(gv);
        lista.push_back(p);
    }
    return lista;
}

void mostrarJornadas(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "No hay jornadas registradas aun.\n";
        return;
    }
    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::cout << linea << "\n";
    }
}

void guardarPartido(const std::string& ruta, const Partido& p) {
    std::ofstream archivo(ruta, std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo abrir " << ruta << " para escritura.\n";
        return;
    }
    archivo << p.fecha << ";"
            << p.local << ";"
            << p.visitante << ";"
            << p.golesLocal << ";"
            << p.golesVisitante << "\n";
}

void guardarJornada(const std::string& ruta, const std::string& fecha,
                    int numJornada, const std::vector<Partido>& partidos) {
    std::ofstream archivo(ruta, std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo abrir " << ruta << " para escritura.\n";
        return;
    }
    archivo << "JORNADA=" << numJornada << " FECHA=" << fecha << "\n";
    for (int i = 0; i < (int)partidos.size(); i++) {
        if (partidos[i].fecha == fecha) {
            archivo << "  " << partidos[i].local
                    << " "  << partidos[i].golesLocal
                    << "-"  << partidos[i].golesVisitante
                    << " "  << partidos[i].visitante << "\n";
        }
    }
    archivo << "FIN_JORNADA\n";
}

void actualizarEstadisticas(Equipo* e, int aFavor, int enContra,
                             int ptsGanar, int ptsEmpatar, int ptsPerder) {
    e->jugados++;
    e->gf += aFavor;
    e->gc += enContra;
    e->dg = e->gf - e->gc;
    if (aFavor > enContra) {
        e->ganados++;
        e->puntos += ptsGanar;
    } else if (aFavor == enContra) {
        e->empatados++;
        e->puntos += ptsEmpatar;
    } else {
        e->perdidos++;
        e->puntos += ptsPerder;
    }
}

std::vector<Equipo> construirTabla(const std::vector<Partido>& partidos,
                                   const ConfigLiga& cfg) {
    std::vector<Equipo> tabla;
    for (int i = 0; i < (int)cfg.equipos.size(); i++) {
        Equipo e;
        e.nombre = cfg.equipos[i];
        tabla.push_back(e);
    }
    for (int i = 0; i < (int)partidos.size(); i++) {
        for (int j = 0; j < (int)tabla.size(); j++) {
            if (tabla[j].nombre == partidos[i].local)
                actualizarEstadisticas(&tabla[j], partidos[i].golesLocal,
                                       partidos[i].golesVisitante,
                                       cfg.ptsGanar, cfg.ptsEmpatar, cfg.ptsPerder);
            if (tabla[j].nombre == partidos[i].visitante)
                actualizarEstadisticas(&tabla[j], partidos[i].golesVisitante,
                                       partidos[i].golesLocal,
                                       cfg.ptsGanar, cfg.ptsEmpatar, cfg.ptsPerder);
        }
    }
    return tabla;
}

bool compararEquipos(const Equipo& a, const Equipo& b) {
    if (a.puntos != b.puntos) return a.puntos > b.puntos;
    if (a.dg != b.dg)         return a.dg > b.dg;
    return a.gf > b.gf;
}

void mostrarTabla(std::vector<Equipo>& tabla) {
    std::sort(tabla.begin(), tabla.end(), compararEquipos);
    std::cout << "\n";
    std::cout << "#  Equipo                 PJ PG PE PP GF GC DG PTS\n";
    std::cout << std::string(65, '-') << "\n";
    for (int i = 0; i < (int)tabla.size(); i++) {
        std::string nombre = tabla[i].nombre;
        while ((int)nombre.size() < 22) nombre += " ";
        std::cout << i + 1 << "  "
                  << nombre
                  << tabla[i].jugados   << " "
                  << tabla[i].ganados   << " "
                  << tabla[i].empatados << " "
                  << tabla[i].perdidos  << " "
                  << tabla[i].gf        << " "
                  << tabla[i].gc        << " "
                  << tabla[i].dg        << " "
                  << tabla[i].puntos    << "\n";
    }
}

void exportarTabla(std::vector<Equipo>& tabla) {
    std::sort(tabla.begin(), tabla.end(), compararEquipos);
    std::ofstream archivo("tabla.txt");
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo guardar tabla.txt\n";
        return;
    }
    archivo << "#  Equipo                 PJ PG PE PP GF GC DG PTS\n";
    for (int i = 0; i < (int)tabla.size(); i++) {
        std::string nombre = tabla[i].nombre;
        while ((int)nombre.size() < 22) nombre += " ";
        archivo << i + 1 << "  "
                << nombre
                << tabla[i].jugados   << " "
                << tabla[i].ganados   << " "
                << tabla[i].empatados << " "
                << tabla[i].perdidos  << " "
                << tabla[i].gf        << " "
                << tabla[i].gc        << " "
                << tabla[i].dg        << " "
                << tabla[i].puntos    << "\n";
    }
    std::cout << "Tabla guardada en tabla.txt\n";
}

bool equipoValido(const std::string& nombre, const ConfigLiga& cfg) {
    for (int i = 0; i < (int)cfg.equipos.size(); i++) {
        if (cfg.equipos[i] == nombre) return true;
    }
    return false;
}

bool partidoRepetido(const std::string& fecha, const std::string& local,
                     const std::string& visitante,
                     const std::vector<Partido>& partidos) {
    for (int i = 0; i < (int)partidos.size(); i++) {
        if (partidos[i].fecha     == fecha &&
            partidos[i].local     == local &&
            partidos[i].visitante == visitante)
            return true;
    }
    return false;
}

Partido pedirPartido(const ConfigLiga& cfg, const std::vector<Partido>& partidos) {
    Partido p;
    p.golesLocal = -1;
    std::cout << "\nEquipos disponibles:\n";
    for (int i = 0; i < (int)cfg.equipos.size(); i++) {
        std::cout << "  " << i + 1 << ". " << cfg.equipos[i] << "\n";
    }
    std::cout << "Fecha (YYYY-MM-DD): ";
    std::cin >> p.fecha;
    std::cout << "Equipo local: ";
    std::cin >> p.local;
    if (!equipoValido(p.local, cfg)) {
        std::cout << "Equipo no reconocido.\n";
        return p;
    }
    std::cout << "Equipo visitante: ";
    std::cin >> p.visitante;
    if (!equipoValido(p.visitante, cfg)) {
        std::cout << "Equipo no reconocido.\n";
        return p;
    }
    if (p.local == p.visitante) {
        std::cout << "Los equipos deben ser distintos.\n";
        return p;
    }
    if (partidoRepetido(p.fecha, p.local, p.visitante, partidos)) {
        std::cout << "Ese partido ya fue registrado en esa fecha.\n";
        return p;
    }
    std::cout << "Goles " << p.local     << ": "; std::cin >> p.golesLocal;
    std::cout << "Goles " << p.visitante << ": "; std::cin >> p.golesVisitante;
    return p;
}

void mostrarTodosLosPartidos(const std::vector<Partido>& partidos) {
    if (partidos.empty()) {
        std::cout << "No hay partidos registrados.\n";
        return;
    }
    std::cout << "\nPartidos jugados:\n";
    for (int i = 0; i < (int)partidos.size(); i++) {
        std::cout << partidos[i].fecha << "  "
                  << partidos[i].local << " "
                  << partidos[i].golesLocal << "-"
                  << partidos[i].golesVisitante << " "
                  << partidos[i].visitante << "\n";
    }
}

int mostrarMenu(const std::string& nombreLiga) {
    int opcion;
    std::cout << "\n========== " << nombreLiga << " ==========\n";
    std::cout << "1. Ver tabla de posiciones\n";
    std::cout << "2. Registrar resultado\n";
    std::cout << "3. Ver historial de jornadas\n";
    std::cout << "4. Ver todos los partidos\n";
    std::cout << "5. Salir\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;
    return opcion;
}

int main() {
    ConfigLiga cfg;
    if (!leerConfig("config.txt", cfg)) {
        return 1;
    }
    std::vector<Partido> partidos = leerPartidos("partidos.txt");
    int opcion;
    int numJornada = 1;
    do {
        opcion = mostrarMenu(cfg.nombre);
        if (opcion == 1) {
            std::vector<Equipo> tabla = construirTabla(partidos, cfg);
            mostrarTabla(tabla);
            char guardar;
            std::cout << "Guardar tabla en archivo? (s/n): ";
            std::cin >> guardar;
            if (guardar == 's' || guardar == 'S') {
                exportarTabla(tabla);
            }
        } else if (opcion == 2) {
            Partido nuevo = pedirPartido(cfg, partidos);
            if (nuevo.golesLocal >= 0) {
                partidos.push_back(nuevo);
                guardarPartido("partidos.txt", nuevo);
                guardarJornada("fechas.txt", nuevo.fecha, numJornada, partidos);
                numJornada++;
                std::cout << "Partido registrado.\n";
            }
        } else if (opcion == 3) {
            mostrarJornadas("fechas.txt");
        } else if (opcion == 4) {
            mostrarTodosLosPartidos(partidos);
        } else if (opcion != 5) {
            std::cout << "Opcion no valida.\n";
        }
    } while (opcion != 5);
    std::cout << "Hasta luego.\n";
    return 0;
}
