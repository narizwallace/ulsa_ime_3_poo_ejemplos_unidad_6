// =============================================================================
// Diapositiva 16 — Ejemplo en C++: el robot delega su locomoción
// =============================================================================
// OBJETIVO
//   Ver la delegación en su forma más simple: Robot NO sabe moverse; le pide a
//   un objeto colaborador (Locomocion) que lo haga.
//
// IDEAS CLAVE
//   - Robot "tiene un" objeto Locomocion (HAS-A, composición), no "es" uno.
//   - Robot conoce a su delegado SOLO por la interfaz Locomocion.
//   - Nueva forma de moverse = nueva clase Locomocion; Robot NO cambia.
//   - Se evita la explosión de subclases (RobotConRuedas, RobotConOrugas...).
//
// QUÉ OBSERVAR AL EJECUTAR
//   - Dos robots de la MISMA clase se mueven distinto según su delegado.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/16_delegacion_locomocion.cpp -o build/16
//   ./build/16
// =============================================================================

#include <iostream>
#include <memory>
#include <string>

// -----------------------------------------------------------------------------
// La INTERFAZ del delegado: lo único que Robot sabe de la locomoción.
// -----------------------------------------------------------------------------
class Locomocion {
public:
    virtual ~Locomocion() = default;               // destructor virtual (regla del curso)
    virtual void mover(int distanciaCm) = 0;       // contrato: toda locomoción sabe mover
};

// Dos implementaciones concretas del delegado.
class Ruedas : public Locomocion {
public:
    void mover(int distanciaCm) override {
        std::cout << "Rodando " << distanciaCm << " cm\n";
    }
};

class Orugas : public Locomocion {
public:
    void mover(int distanciaCm) override {
        std::cout << "Avanzando con orugas " << distanciaCm << " cm\n";
    }
};

// -----------------------------------------------------------------------------
// El DELEGANTE: Robot
// -----------------------------------------------------------------------------
class Robot {
private:
    std::string nombre;
    // unique_ptr<Locomocion>: el robot es DUEÑO de su locomoción y la usa a
    // través de la interfaz. Gracias al polimorfismo, aquí puede vivir una
    // Ruedas, unas Orugas, o cualquier clase futura que herede de Locomocion.
    std::unique_ptr<Locomocion> locomocion;

public:
    // El delegado se entrega por el constructor (la propiedad se CEDE con move).
    Robot(std::string n, std::unique_ptr<Locomocion> l)
        : nombre(std::move(n)), locomocion(std::move(l)) {}

    // DELEGACIÓN: Robot recibe la petición "avanzar" y se la reenvía a su
    // colaborador. El cliente de Robot no sabe (ni le importa) cómo se mueve.
    void avanzar(int distanciaCm) {
        std::cout << nombre << " -> ";
        locomocion->mover(distanciaCm);
    }
};

int main() {
    // La MISMA clase Robot, con comportamientos distintos, sin crear subclases.
    Robot r1("R1", std::make_unique<Ruedas>());
    Robot r2("R2", std::make_unique<Orugas>());

    r1.avanzar(100);   // R1 -> Rodando 100 cm
    r2.avanzar(100);   // R2 -> Avanzando con orugas 100 cm

    // Para agregar "Patas" basta una clase Patas : public Locomocion.
    // Robot, r1 y r2 no se modifican.
    return 0;
}
