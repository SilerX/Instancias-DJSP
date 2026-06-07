#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

inline int32_t taillard_unif(int32_t& seed, int32_t low, int32_t high) {
    constexpr int32_t a = 16807;        // multiplicador
    constexpr int32_t b = 127773;       // m / a  (cociente)
    constexpr int32_t c = 2836;         // m % a  (resto/residuo)
    constexpr int32_t m = 2147483647;   // 2^31 - 1 (módulo primo de Mersenne)

    const int32_t k = seed / b;
    seed = a * (seed % b) - k * c;
    if (seed < 0) seed += m;

    const double u = static_cast<double>(seed) / static_cast<double>(m);
    return low + static_cast<int32_t>(u * (high - low + 1));
}

// ---------------------------------------------------------------------------
//  Configuración de semillas hardcodeadas para las 80 instancias job-shop de Taillard,
//  en orden canónico ta01 .. ta80 (fuente: OR-Library jobshop2.txt).
// ---------------------------------------------------------------------------
struct TaillardSeed {
    int     jobs;          // n (trabajos)
    int     machines;      // m (máquinas)
    int32_t time_seed;     // semilla para la generación de tiempos de procesamiento
    int32_t machine_seed;  // semilla para la generación de rutas de máquinas
};

static const std::vector<TaillardSeed> kTaillard = {
    // ---- 15 trabajos, 15 máquinas : ta01 - ta10 ----
    {15, 15,  840612802,  398197754},   // ta01
    {15, 15, 1314640371,  386720536},   // ta02
    {15, 15, 1227221349,  316176388},   // ta03
    {15, 15,  342269428, 1806358582},   // ta04
    {15, 15, 1603221416, 1501949241},   // ta05
    {15, 15, 1357584978, 1734077082},   // ta06
    {15, 15,   44531661, 1374316395},   // ta07
    {15, 15,  302545136, 2092186050},   // ta08
    {15, 15, 1153780144, 1393392374},   // ta09
    {15, 15,   73896786, 1544979948},   // ta10
    // ---- 20 trabajos, 15 máquinas : ta11 - ta20 ----
    {20, 15,  533484900,  317419073},   // ta11
    {20, 15, 1894307698, 1474268163},   // ta12
    {20, 15,  874340513,  509669280},   // ta13
    {20, 15, 1124986343, 1209573668},   // ta14
    {20, 15, 1463788335,  529048107},   // ta15
    {20, 15, 1056908795,   25321885},   // ta16
    {20, 15,  195672285, 1717580117},   // ta17
    {20, 15,  961965583, 1353003786},   // ta18
    {20, 15, 1610169733, 1734469503},   // ta19
    {20, 15,  532794656,  998486810},   // ta20
    // ---- 20 trabajos, 20 máquinas : ta21 - ta30 ----
    {20, 20, 1035939303,  773961798},   // ta21
    {20, 20,    5997802, 1872541150},   // ta22
    {20, 20, 1357503601,  722225039},   // ta23
    {20, 20,  806159563, 1166962073},   // ta24
    {20, 20, 1902815253, 1879990068},   // ta25
    {20, 20, 1503184031, 1850351876},   // ta26
    {20, 20, 1032645967,   99711329},   // ta27
    {20, 20,  229894219, 1158117804},   // ta28
    {20, 20,  823349822,  108033225},   // ta29
    {20, 20, 1297900341,  489486403},   // ta30
    // ---- 30 trabajos, 15 máquinas : ta31 - ta40 ----
    {30, 15,   98640593, 1981283465},   // ta31
    {30, 15, 1839268120,  248890888},   // ta32
    {30, 15,  573875290, 2081512253},   // ta33
    {30, 15, 1670898570,  788294565},   // ta34
    {30, 15, 1118914567, 1074349202},   // ta35
    {30, 15,  178750207,  294279708},   // ta36
    {30, 15, 1549372605,  596993084},   // ta37
    {30, 15,  798174738,  151685779},   // ta38
    {30, 15,  553410952, 1329272528},   // ta39
    {30, 15, 1661531649, 1173386294},   // ta40
    // ---- 30 trabajos, 20 máquinas : ta41 - ta50 ----
    {30, 20, 1841414609, 1357882888},   // ta41
    {30, 20, 2116959593, 1546338557},   // ta42
    {30, 20,  796392706, 1230864158},   // ta43
    {30, 20,  532496463,  254174057},   // ta44
    {30, 20, 2020525633,  978943053},   // ta45
    {30, 20,  524444252,  185526083},   // ta46
    {30, 20, 1569394691,  487269855},   // ta47
    {30, 20, 1460267840, 1631446539},   // ta48
    {30, 20,  198324822, 1937476577},   // ta49
    {30, 20,   38071822, 1541985579},   // ta50
    // ---- 50 trabajos, 15 máquinas : ta51 - ta60 ----
    {50, 15,       17271,     718939},   // ta51
    {50, 15,  660481279,  449650254},   // ta52
    {50, 15,  352229765,  949737911},   // ta53
    {50, 15, 1197518780,  166840558},   // ta54
    {50, 15, 1376020303,  483922052},   // ta55
    {50, 15, 2106639239,  955932362},   // ta56
    {50, 15, 1765352082, 1209982549},   // ta57
    {50, 15, 1105092880, 1349003108},   // ta58
    {50, 15,  907248070,  919544535},   // ta59
    {50, 15, 2011630757, 1845447001},   // ta60
    // ---- 50 trabajos, 20 máquinas : ta61 - ta70 ----
    {50, 20,    8493988,    2738939},   // ta61
    {50, 20, 1991925010,  709517751},   // ta62
    {50, 20,  342093237,  786960785},   // ta63
    {50, 20, 1634043183,  973178279},   // ta64
    {50, 20,  341706507,  286513148},   // ta65
    {50, 20,  320167954, 1411193018},   // ta66
    {50, 20, 1089696753,  298068750},   // ta67
    {50, 20,  433032965, 1589656152},   // ta68
    {50, 20,  615974477,  331205412},   // ta69
    {50, 20,  236150141,  592292984},   // ta70
    // ---- 100 trabajos, 20 máquinas : ta71 - ta80 ----
    {100, 20,  302034063, 1203569070},  // ta71
    {100, 20, 1437643198, 1692025209},  // ta72
    {100, 20, 1792475497, 1039908559},  // ta73
    {100, 20, 1647273132, 1012841433},  // ta74
    {100, 20,  696480901, 1689682358},  // ta75
    {100, 20, 1785569423, 1092647459},  // ta76
    {100, 20,  117806902,  739059626},  // ta77
    {100, 20, 1639154709, 1319962509},  // ta78
    {100, 20, 2007423389,  749368241},  // ta79
    {100, 20,  682761130,  262763021},  // ta80
};

// ---------------------------------------------------------------------------
//  Reconstruye las matrices de tiempos de procesamiento y de rutas de máquinas
//  para una instancia, siguiendo el orden de generación exacto de Taillard.
//
//  duration[j][i] : tiempo de procesamiento de la i-ésima operación del trabajo j.
//  routing[j][i]  : máquina (indexada desde 0) que realiza la i-ésima operación del
//                   trabajo j (una permutación de 0..m-1 para cada trabajo).
// ---------------------------------------------------------------------------
void generate_instance(const TaillardSeed& s,
                       std::vector<std::vector<int>>& duration,
                       std::vector<std::vector<int>>& routing) {
    const int n = s.jobs;
    const int m = s.machines;

    int32_t time_seed    = s.time_seed;
    int32_t machine_seed = s.machine_seed;

    duration.assign(n, std::vector<int>(m, 0));
    routing.assign(n, std::vector<int>(m, 0));

    // Tiempos de procesamiento, generados priorizando filas (orden por trabajos): 
    // para cada trabajo, se obtienen m duraciones en U[1, 99] desde el flujo de la semilla de tiempo.
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < m; ++i)
            duration[j][i] = taillard_unif(time_seed, 1, 99);

    // Rutas de máquinas: comenzar desde la permutación identidad por trabajo...
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < m; ++i)
            routing[j][i] = i;

    // ... luego aplicar el barajado (shuffle) Fisher-Yates de Taillard por trabajo,
    // obteniendo el índice de intercambio desde U[i, m-1] del flujo de la semilla de máquina.
    for (int j = 0; j < n; ++j)
        for (int i = 0; i < m; ++i) {
            const int32_t k = taillard_unif(machine_seed, i, m - 1);
            std::swap(routing[j][i], routing[j][k]);
        }
}

// ---------------------------------------------------------------------------
//  Escribe una matriz de enteros de (n x m) como columnas alineadas a la derecha
//  y delimitadas por espacios: limpio de leer y trivial de parsear con extracción de flujos.
// ---------------------------------------------------------------------------
void write_matrix(std::ofstream& out, const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        for (std::size_t i = 0; i < row.size(); ++i) {
            if (i) out << ' ';
            out << std::setw(2) << row[i];
        }
        out << '\n';
    }
}

// ---------------------------------------------------------------------------
//  Compone el nombre canónico de la instancia: ta01 .. ta80 (basado en 1, con relleno de ceros).
// ---------------------------------------------------------------------------
std::string instance_name(std::size_t one_based_index) {
    std::ostringstream os;
    os << "ta" << std::setw(2) << std::setfill('0') << one_based_index;
    return os.str();
}

int main(int argc, char** argv) {
    const std::array<int, 3> factories = {2, 3, 4};   // f en {2, 3, 4}

    const fs::path outdir = (argc > 1) ? fs::path(argv[1])
                                       : fs::path("djsp_instances");

    std::error_code ec;
    fs::create_directories(outdir, ec);
    if (ec) {
        std::cerr << "Error: no se pudo crear el directorio de salida '"
                  << outdir.string() << "': " << ec.message() << '\n';
        return 1;
    }

    int files_written = 0;

    for (std::size_t idx = 0; idx < kTaillard.size(); ++idx) {
        const TaillardSeed& s = kTaillard[idx];

        std::vector<std::vector<int>> duration;
        std::vector<std::vector<int>> routing;
        generate_instance(s, duration, routing);

        const std::string name = instance_name(idx + 1);

        for (int f : factories) {
            const fs::path file =
                outdir / (name + "_f" + std::to_string(f) + ".txt");

            std::ofstream out(file);
            if (!out) {
                std::cerr << "Error: no se puede abrir '" << file.string()
                          << "' para escritura\n";
                return 1;
            }

            out << s.jobs << ' ' << s.machines << ' ' << f << '\n';
            write_matrix(out, duration);   // n filas: tiempos de procesamiento
            write_matrix(out, routing);    // n filas: rutas de máquinas

            ++files_written;
        }

        std::cout << "Generada " << name << "  (" << s.jobs << " x "
                  << s.machines << ")  -> f=2,3,4\n";
    }

    std::cout << "\nTerminado. " << files_written << " archivos guardados en '"
              << fs::absolute(outdir).string() << "'.\n";
    return 0;
}