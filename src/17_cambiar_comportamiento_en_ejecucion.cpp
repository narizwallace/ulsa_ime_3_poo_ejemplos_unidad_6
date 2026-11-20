// =============================================================================
// Diapositiva 17 — Cambiar el comportamiento en ejecución
// =============================================================================
// OBJETIVO
//   Mostrar la gran ventaja de la delegación sobre la herencia: el delegado se
//   puede REEMPLAZAR mientras el programa corre.
//
// IDEAS CLAVE
//   - Con herencia, la clase de un objeto se fija al crearlo y no cambia.
//   - Con delegación, basta reasignar el puntero al delegado.
//   - Validar que el delegado no sea nullptr evita fallos difíciles de rastrear.
//
// QUÉ OBSERVAR AL EJECUTAR
//   - El MISMO objeto robot avanza con ruedas y luego con orugas.
//   - Intentar darle un delegado nulo lanza una excepción controlada.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/17_cambiar_comportamiento_en_ejecucion.cpp -o build/17
//   ./build/17
// =============================================================================

#include <iostream>
#include <memory>
#include <stdexcept>   // std::invalid_argument
#include <string>

class Locomocion {
public:
    virtual ~Locomocion() = default;
    virtual void mover(int distanciaCm) = 0;
};

class Ruedas : public Locomocion {
public:
    void mover(int d) override { std::cout << "Rodando " << d << " cm\n"; }
};

class Orugas : public Locomocion {
public:
    void mover(int d) override { std::cout << "Avanzando con orugas " << d << " cm\n"; }
};

class Robot {
private:
    std::unique_ptr<Locomocion> locomocion;

    // Método privado de apoyo: así la validación vive en UN solo lugar y la
    // usan tanto el constructor como cambiarLocomocion.
    static void validar(const std::unique_ptr<Locomocion>& l) {
        if (!l) {
            throw std::invalid_argument("La locomocion no puede ser nula");
        }
    }

public:
    explicit Robot(std::unique_ptr<Locomocion> l) : locomocion(std::move(l)) {
        // [Nota de C++] Se valida DESPUÉS de mover: el parámetro l quedó vacío,
        // pero el atributo locomocion ya tiene (o no) el objeto. Validamos el atributo.
        validar(locomocion);
    }

    void avanzar(int d) { locomocion->mover(d); }

    // Reemplazo del delegado EN EJECUCIÓN.
    // El unique_ptr anterior se destruye solo al reasignar: no hay fugas.
    void cambiarLocomocion(std::unique_ptr<Locomocion> nueva) {
        validar(nueva);                       // 1° validar antes de tocar el estado
        locomocion = std::move(nueva);        // 2° reemplazar
    }
};

int main() {
    Robot r(std::make_unique<Ruedas>());

    std::cout << "Terreno liso:\n";
    r.avanzar(100);                                  // usa Ruedas

    std::cout << "\nSe detecta terreno irregular -> cambio de locomocion:\n";
    r.cambiarLocomocion(std::make_unique<Orugas>());
    r.avanzar(100);                                  // el MISMO objeto ahora usa Orugas

    // Protección contra delegados nulos
    std::cout << "\nIntento de asignar una locomocion nula:\n";
    try {
        r.cambiarLocomocion(nullptr);
    } catch (const std::invalid_argument& e) {
        std::cout << "Error controlado: " << e.what() << "\n";
    }
    r.avanzar(50);   // el robot conserva su último delegado válido (Orugas)

    return 0;
}
