// =============================================================================
// Diapositiva 6 — El primer intento (que falla): copiar con un puntero base
// =============================================================================
// OBJETIVO
//   Ver con tus propios ojos por qué NO se puede copiar un objeto polimórfico
//   a través de un puntero a su clase base.
//
// QUÉ OBSERVAR AL EJECUTAR
//   - El original describe un soldador a 120 A.
//   - La "copia" describe un robot genérico: perdió el amperaje y el
//     comportamiento de la derivada. A esto se le llama SLICING (rebanado).
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/06_slicing_copia_fallida.cpp -o build/06
//   ./build/06
// =============================================================================

#include <iostream>
#include <string>

// -----------------------------------------------------------------------------
// Clase base CONCRETA (no abstracta). Más abajo explicamos por qué importa.
// -----------------------------------------------------------------------------
class RobotBasico {
private:
    std::string nombre;

public:
    explicit RobotBasico(std::string n) : nombre(std::move(n)) {}

    // [Nota de C++] Regla del curso: si la clase tiene métodos virtuales,
    // el destructor también es virtual (= default).
    virtual ~RobotBasico() = default;

    // Método virtual: cada derivada puede describirse a su manera.
    virtual void describir() const {
        std::cout << "Robot generico: " << nombre << "\n";
    }
};

// -----------------------------------------------------------------------------
// Clase derivada: agrega un atributo (amperaje) que la base NO conoce.
// -----------------------------------------------------------------------------
class SoldadorBasico : public RobotBasico {
private:
    int amperaje;

public:
    SoldadorBasico(std::string n, int a) : RobotBasico(std::move(n)), amperaje(a) {}

    void describir() const override {
        // Aquí usamos el atributo propio de la derivada.
        std::cout << "Soldador a " << amperaje << " A\n";
    }
};

int main() {
    SoldadorBasico soldador("Maestro", 120);

    // Un puntero a la base apunta a un objeto cuyo tipo REAL es SoldadorBasico.
    // En un programa real, ese tipo podría ser desconocido (llegó por un parámetro).
    RobotBasico* base = &soldador;

    // ✗ Intento de copia: new RobotBasico(*base)
    //   *base se trata como un RobotBasico, así que el constructor de copia de
    //   RobotBasico copia SOLO la parte RobotBasico. El amperaje se pierde y
    //   el objeto nuevo es un RobotBasico de verdad, no un SoldadorBasico.
    RobotBasico* copia = new RobotBasico(*base);

    std::cout << "Original: ";
    base->describir();     // versión de SoldadorBasico (polimorfismo)
    std::cout << "Copia:    ";
    copia->describir();    // versión de RobotBasico → ¡rebanada!

    delete copia;  // new → delete. (Más adelante lo haremos con unique_ptr.)

    // -------------------------------------------------------------------------
    // ¿Y si la base fuera ABSTRACTA?
    //   Si RobotBasico tuviera un método virtual puro (= 0), la línea
    //       new RobotBasico(*base)
    //   NO COMPILARÍA: no se pueden crear instancias de una clase abstracta.
    //   Las dos situaciones llevan a la misma conclusión: para copiar a través
    //   de un puntero base necesitamos POLIMORFISMO → el método clonar()
    //   (diapositivas 7 y 8).
    // -------------------------------------------------------------------------
    return 0;
}
