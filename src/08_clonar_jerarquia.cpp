// =============================================================================
// Diapositivas 7 y 8 — Constructor virtual: clonar() en la jerarquía
// =============================================================================
// OBJETIVO
//   Copiar un objeto a través de un puntero a la base y obtener una copia del
//   TIPO REAL (RobotSoldador, RobotPintor...), no una versión rebanada.
//
// IDEA CLAVE
//   En C++ no existen "constructores virtuales", así que se simulan con un
//   método virtual (clonar). Cada clase derivada sabe copiarse a sí misma.
//
// QUÉ OBSERVAR AL EJECUTAR
//   - La copia conserva el amperaje / el color: es del tipo real.
//   - typeid confirma el tipo dinámico de cada copia.
//   - Cambiar el nombre de la copia NO cambia al original: son independientes.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/08_clonar_jerarquia.cpp -o build/08
//   ./build/08
// =============================================================================

#include <iostream>
#include <memory>    // std::unique_ptr, std::make_unique
#include <string>
#include <typeinfo>  // typeid

// -----------------------------------------------------------------------------
// Clase base ABSTRACTA: define el contrato que toda clase de robot debe cumplir.
// -----------------------------------------------------------------------------
class Robot {
private:
    std::string nombre;

public:
    explicit Robot(std::string n) : nombre(std::move(n)) {}

    // [Nota de C++] Destructor virtual: sin él, borrar una derivada a través de
    // un Robot* sería comportamiento indefinido.
    virtual ~Robot() = default;

    // El "constructor virtual": devuelve una copia del objeto, del tipo real.
    //  - "= 0"  → virtual puro: toda derivada DEBE implementarlo.
    //  - "const" → clonar no modifica al original.
    //  - unique_ptr → deja claro que quien recibe la copia es su dueño.
    virtual std::unique_ptr<Robot> clonar() const = 0;

    virtual void describir() const = 0;

    const std::string& getNombre() const { return nombre; }
    void setNombre(std::string n) { nombre = std::move(n); }
};

// -----------------------------------------------------------------------------
// Primera derivada
// -----------------------------------------------------------------------------
class RobotSoldador : public Robot {
private:
    int amperaje;

public:
    RobotSoldador(std::string n, int a) : Robot(std::move(n)), amperaje(a) {}

    std::unique_ptr<Robot> clonar() const override {
        // *this es un RobotSoldador (aquí SÍ conocemos el tipo real).
        // make_unique<RobotSoldador>(*this) invoca el constructor de copia de
        // RobotSoldador, que copia nombre (de la base) Y amperaje (propio).
        return std::make_unique<RobotSoldador>(*this);
    }

    void describir() const override {
        std::cout << getNombre() << ": soldador a " << amperaje << " A\n";
    }
};

// -----------------------------------------------------------------------------
// Segunda derivada: cada clase nueva debe implementar su propio clonar().
// -----------------------------------------------------------------------------
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

int main() {
    // Los dos robots se manejan SIEMPRE a través de Robot* / unique_ptr<Robot>.
    std::unique_ptr<Robot> maestroSoldador = std::make_unique<RobotSoldador>("Maestro-S", 120);
    std::unique_ptr<Robot> maestroPintor   = std::make_unique<RobotPintor>("Maestro-P", "azul");

    // Quien clona NO necesita saber qué tipo de robot es.
    std::unique_ptr<Robot> copiaS = maestroSoldador->clonar();
    std::unique_ptr<Robot> copiaP = maestroPintor->clonar();

    std::cout << "--- Copias (conservan el tipo real) ---\n";
    copiaS->describir();
    copiaP->describir();

    // typeid sobre un objeto polimórfico (*puntero) devuelve el tipo DINÁMICO.
    // [Nota de C++] name() devuelve un nombre "decorado" que depende del
    // compilador (en g++ lleva el largo del nombre: 13RobotSoldador).
    // Comparar con == es lo portable.
    std::cout << "\n--- Tipo dinamico de cada copia ---\n";
    std::cout << "copiaS: " << typeid(*copiaS).name() << "\n";
    std::cout << "copiaS es RobotSoldador? "
              << (typeid(*copiaS) == typeid(RobotSoldador) ? "si" : "no") << "\n";
    std::cout << "copiaP es RobotPintor?   "
              << (typeid(*copiaP) == typeid(RobotPintor) ? "si" : "no") << "\n";

    // La copia es un objeto DISTINTO: modificarla no afecta al original.
    copiaS->setNombre("Copia-S");
    std::cout << "\n--- Independencia ---\n";
    maestroSoldador->describir();  // sigue llamándose Maestro-S
    copiaS->describir();           // se llama Copia-S

    // No hay delete: los unique_ptr liberan la memoria solos al salir de main.
    return 0;
}
