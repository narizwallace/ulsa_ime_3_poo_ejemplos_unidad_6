// =============================================================================
// Diapositiva 23 — Integrador: prototipo + delegación
// =============================================================================
// OBJETIVO
//   Juntar las dos ideas de la unidad: un robot que se CLONA (prototipo) y que
//   DELEGA su movimiento en una locomoción (delegación).
//
// EL PROBLEMA QUE RESUELVE
//   Si el robot se clona pero su locomoción NO, original y clon compartirían el
//   mismo delegado (copia superficial). Para una copia profunda, el robot
//   delega también la COPIA en su locomoción: Locomocion::clonar().
//
// QUÉ OBSERVAR AL EJECUTAR
//   - Original y clon apuntan a delegados con DIRECCIONES DISTINTAS.
//   - Cambiar la locomoción del clon NO afecta al original.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/23_integrador_prototipo_y_delegacion.cpp -o build/23
//   ./build/23
// =============================================================================

#include <iostream>
#include <memory>
#include <string>

// -----------------------------------------------------------------------------
// El delegado también es un prototipo: sabe clonarse (constructor virtual).
// -----------------------------------------------------------------------------
class Locomocion {
public:
    virtual ~Locomocion() = default;
    virtual void mover(int distanciaCm) = 0;
    virtual std::unique_ptr<Locomocion> clonar() const = 0;
};

class Ruedas : public Locomocion {
public:
    void mover(int d) override { std::cout << "Rodando " << d << " cm\n"; }
    std::unique_ptr<Locomocion> clonar() const override {
        return std::make_unique<Ruedas>(*this);
    }
};

class Orugas : public Locomocion {
public:
    void mover(int d) override { std::cout << "Avanzando con orugas " << d << " cm\n"; }
    std::unique_ptr<Locomocion> clonar() const override {
        return std::make_unique<Orugas>(*this);
    }
};

// -----------------------------------------------------------------------------
// El robot: se clona Y delega
// -----------------------------------------------------------------------------
class RobotMovil {
private:
    std::string nombre;
    std::unique_ptr<Locomocion> locomocion;

public:
    RobotMovil(std::string n, std::unique_ptr<Locomocion> l)
        : nombre(std::move(n)), locomocion(std::move(l)) {}

    // CONSTRUCTOR DE COPIA PROFUNDA.
    // El robot NO sabe qué locomoción tiene (Ruedas, Orugas...). Le PIDE a su
    // locomoción que se clone: delegación aplicada a la copia.
    // [Nota de C++] Con un unique_ptr como miembro, el compilador NO puede
    // generar una copia (unique_ptr no se copia). Por eso la escribimos nosotros.
    RobotMovil(const RobotMovil& otro)
        : nombre(otro.nombre), locomocion(otro.locomocion->clonar()) {}

    // Asignación por copia desactivada explícitamente: en este ejemplo solo
    // queremos crear copias por clonar(), no sobrescribir robots existentes.
    RobotMovil& operator=(const RobotMovil&) = delete;

    // Constructor virtual a nivel de robot (aquí no hay derivadas, así que no
    // es virtual; es solo la forma habitual de pedir una copia).
    std::unique_ptr<RobotMovil> clonar() const {
        return std::make_unique<RobotMovil>(*this);   // usa el ctor de copia profunda
    }

    void setNombre(std::string n) { nombre = std::move(n); }

    void avanzar(int d) {
        std::cout << nombre << " -> ";
        locomocion->mover(d);                         // delegación
    }

    void cambiarLocomocion(std::unique_ptr<Locomocion> nueva) {
        locomocion = std::move(nueva);
    }

    // Solo para la demostración: ¿en qué dirección vive el delegado?
    const Locomocion* delegado() const { return locomocion.get(); }
};

int main() {
    RobotMovil original("Original", std::make_unique<Ruedas>());

    // Prototipo: clonamos el robot completo (incluido su delegado).
    std::unique_ptr<RobotMovil> clon = original.clonar();
    clon->setNombre("Clon");

    std::cout << "--- Ambos se mueven igual (el clon heredo la locomocion) ---\n";
    original.avanzar(100);
    clon->avanzar(100);

    std::cout << "\n--- Copia PROFUNDA: delegados distintos ---\n";
    std::cout << "Delegado del original: " << original.delegado() << "\n";
    std::cout << "Delegado del clon:     " << clon->delegado() << "\n";
    std::cout << "Son el mismo objeto? "
              << (original.delegado() == clon->delegado() ? "si" : "no") << "\n";

    std::cout << "\n--- Cambiar el delegado del clon NO afecta al original ---\n";
    clon->cambiarLocomocion(std::make_unique<Orugas>());
    original.avanzar(50);    // sigue con ruedas
    clon->avanzar(50);       // ahora con orugas

    return 0;
}
