// =============================================================================
// Diapositivas 11 y 12 — Catálogo de prototipos y fábrica de robots por plantillas
// =============================================================================
// OBJETIVO
//   Crear robots clonando "plantillas" guardadas en un catálogo, sin que el
//   catálogo conozca las clases concretas.
//
// IDEAS CLAVE
//   - El catálogo guarda prototipos (objetos ya configurados) con una clave.
//   - crear(clave) devuelve un CLON; el prototipo nunca se entrega ni se modifica.
//   - Para tener un tipo nuevo basta registrar otro prototipo: el catálogo no
//     cambia (principio Abierto/Cerrado, Unidad IV).
//
// QUÉ OBSERVAR AL EJECUTAR
//   - Cinco robots salen de dos plantillas.
//   - Renombrar un clon NO cambia al prototipo.
//   - Pedir una clave inexistente devuelve nullptr y se maneja sin fallar.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/11_12_catalogo_y_fabrica.cpp -o build/11_12
//   ./build/11_12
// =============================================================================

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Jerarquía de robots (la misma de la diapositiva 8)
// -----------------------------------------------------------------------------
class Robot {
private:
    std::string nombre;

public:
    explicit Robot(std::string n) : nombre(std::move(n)) {}
    virtual ~Robot() = default;

    virtual std::unique_ptr<Robot> clonar() const = 0;
    virtual void describir() const = 0;

    const std::string& getNombre() const { return nombre; }
    void setNombre(std::string n) { nombre = std::move(n); }
};

class RobotSoldador : public Robot {
private:
    int amperaje;

public:
    RobotSoldador(std::string n, int a) : Robot(std::move(n)), amperaje(a) {}

    std::unique_ptr<Robot> clonar() const override {
        return std::make_unique<RobotSoldador>(*this);
    }
    void describir() const override {
        std::cout << getNombre() << ": soldador a " << amperaje << " A\n";
    }
};

class RobotPintor : public Robot {
private:
    std::string color;

public:
    RobotPintor(std::string n, std::string c) : Robot(std::move(n)), color(std::move(c)) {}

    std::unique_ptr<Robot> clonar() const override {
        return std::make_unique<RobotPintor>(*this);
    }
    void describir() const override {
        std::cout << getNombre() << ": pintor de color " << color << "\n";
    }
};

// -----------------------------------------------------------------------------
// El catálogo (diapositiva 11)
// -----------------------------------------------------------------------------
class CatalogoDeRobots {
private:
    // Clave (texto) → prototipo. El catálogo es DUEÑO de sus prototipos
    // (unique_ptr), así que se liberan solos cuando el catálogo se destruye.
    std::map<std::string, std::unique_ptr<Robot>> prototipos;

public:
    // Recibe el prototipo POR VALOR de unique_ptr: quien llama cede la propiedad
    // (por eso se usa std::move al llamar). Si la clave ya existía, se reemplaza.
    void registrar(const std::string& clave, std::unique_ptr<Robot> prototipo) {
        prototipos[clave] = std::move(prototipo);
    }

    // Devuelve un CLON, nunca el prototipo. Si la clave no existe, devuelve nullptr.
    // Observa que el catálogo solo conoce a Robot: no sabe nada de RobotSoldador
    // ni de RobotPintor.
    std::unique_ptr<Robot> crear(const std::string& clave) const {
        auto it = prototipos.find(clave);   // it apunta a un par <clave, prototipo>
        if (it == prototipos.end()) {
            return nullptr;                 // clave desconocida
        }
        return it->second->clonar();        // it->second es el unique_ptr<Robot>
    }
};

// -----------------------------------------------------------------------------
// Fábrica de robots en acción (diapositiva 12)
// -----------------------------------------------------------------------------
int main() {
    CatalogoDeRobots catalogo;

    // Se registran las plantillas. make_unique crea el prototipo y el unique_ptr
    // temporal se mueve dentro del catálogo.
    catalogo.registrar("soldador-estandar",
                       std::make_unique<RobotSoldador>("Estandar", 120));
    catalogo.registrar("soldador-pesado",
                       std::make_unique<RobotSoldador>("Pesado", 250));
    catalogo.registrar("pintor-azul",
                       std::make_unique<RobotPintor>("Pintor", "azul"));

    // La línea de producción guarda robots de tipos distintos en un solo vector.
    std::vector<std::unique_ptr<Robot>> linea;

    for (int i = 1; i <= 3; ++i) {
        auto robot = catalogo.crear("soldador-estandar");
        robot->setNombre("Estandar-" + std::to_string(i));  // solo cambia el CLON
        linea.push_back(std::move(robot));  // unique_ptr no se copia: se mueve
    }
    linea.push_back(catalogo.crear("soldador-pesado"));
    linea.push_back(catalogo.crear("pintor-azul"));

    std::cout << "--- Linea de produccion ---\n";
    for (const auto& robot : linea) {
        robot->describir();   // polimorfismo: cada uno se describe a su manera
    }

    // El prototipo NO cambió: pedimos otro clon y conserva el nombre original.
    std::cout << "\n--- El prototipo sigue intacto ---\n";
    catalogo.crear("soldador-estandar")->describir();   // "Estandar", no "Estandar-3"

    // Clave inexistente: siempre hay que verificar el nullptr antes de usar.
    std::cout << "\n--- Clave inexistente ---\n";
    auto inexistente = catalogo.crear("soldador-fantasma");
    if (!inexistente) {
        std::cout << "No existe la plantilla 'soldador-fantasma'\n";
    }

    // Agregar un tipo nuevo = registrar otro prototipo. El catálogo no se toca.
    return 0;
}
