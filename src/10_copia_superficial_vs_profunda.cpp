// =============================================================================
// Diapositiva 10 — Copia superficial vs copia profunda
// =============================================================================
// OBJETIVO
//   Entender qué ocurre al copiar un objeto que tiene un PUNTERO CRUDO y cómo
//   resolverlo de dos formas:
//       1) copia profunda escrita a mano (regla de los tres)
//       2) usar std::vector y no escribir nada (regla del cero)  ← preferida
//
// QUÉ OBSERVAR AL EJECUTAR
//   - RegistroSuperficial: original y copia tienen la MISMA dirección de datos;
//     escribir en uno cambia al otro.
//   - RegistroProfundo y RegistroModerno: direcciones distintas, datos independientes.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/10_copia_superficial_vs_profunda.cpp -o build/10
//   ./build/10
// =============================================================================

#include <algorithm>  // std::copy
#include <iostream>
#include <vector>

// =============================================================================
// 1) MAL: puntero crudo + copia por defecto  →  copia SUPERFICIAL
// =============================================================================
class RegistroSuperficial {
private:
    int* lecturas;   // arreglo dinámico en el heap
    int n;

public:
    explicit RegistroSuperficial(int cantidad) : lecturas(new int[cantidad]{}), n(cantidad) {}

    // Esta clase NO define constructor de copia, así que el compilador genera
    // uno que copia miembro a miembro: copia el valor del puntero (la DIRECCIÓN),
    // no el arreglo al que apunta.
    //
    // Además, para poder ejecutar la demostración sin fallar, NO tiene destructor
    // que libere la memoria (se libera a mano con liberar()). Si tuviera
    //     ~RegistroSuperficial() { delete[] lecturas; }
    // el original y la copia intentarían liberar el MISMO arreglo al destruirse:
    // DOBLE delete → comportamiento indefinido (normalmente un crash).
    void liberar() {
        delete[] lecturas;
        lecturas = nullptr;
    }

    void escribir(int i, int valor) { lecturas[i] = valor; }
    int leer(int i) const { return lecturas[i]; }
    const int* direccion() const { return lecturas; }
    int tamanio() const { return n; }
};

// =============================================================================
// 2) Copia PROFUNDA a mano: regla de los tres
// =============================================================================
// Si una clase necesita destructor propio (porque posee un recurso),
// casi seguro necesita también constructor de copia y operador de asignación.
class RegistroProfundo {
private:
    int* lecturas;
    int n;

public:
    explicit RegistroProfundo(int cantidad) : lecturas(new int[cantidad]{}), n(cantidad) {}

    // (1) Destructor: libera el arreglo propio.
    ~RegistroProfundo() { delete[] lecturas; }

    // (2) Constructor de copia: reserva un arreglo NUEVO y copia los valores.
    RegistroProfundo(const RegistroProfundo& otro)
        : lecturas(new int[otro.n]), n(otro.n) {
        std::copy(otro.lecturas, otro.lecturas + otro.n, lecturas);
    }

    // (3) Asignación por copia: igual que el constructor, pero con cuidado extra.
    RegistroProfundo& operator=(const RegistroProfundo& otro) {
        if (this == &otro) return *this;      // evitar autoasignación (a = a)
        int* nuevo = new int[otro.n];         // 1° reservar (si falla, no dañamos this)
        std::copy(otro.lecturas, otro.lecturas + otro.n, nuevo);
        delete[] lecturas;                    // 2° liberar el arreglo viejo
        lecturas = nuevo;                     // 3° adoptar el nuevo
        n = otro.n;
        return *this;
    }

    void escribir(int i, int valor) { lecturas[i] = valor; }
    int leer(int i) const { return lecturas[i]; }
    const int* direccion() const { return lecturas; }
};

// =============================================================================
// 3) BIEN y simple: regla del cero (std::vector)
// =============================================================================
// std::vector ya sabe copiarse en profundidad, moverse y liberarse. Si TODOS los
// miembros de la clase se gestionan solos, no escribimos destructor, ni
// constructor de copia, ni asignación: el compilador genera los correctos.
class RegistroModerno {
private:
    std::vector<int> lecturas;

public:
    explicit RegistroModerno(std::size_t cantidad) : lecturas(cantidad, 0) {}

    void escribir(std::size_t i, int valor) { lecturas[i] = valor; }
    int leer(std::size_t i) const { return lecturas[i]; }
    const int* direccion() const { return lecturas.data(); }
};

int main() {
    std::cout << "=== 1) Copia superficial (puntero crudo) ===\n";
    RegistroSuperficial s1(3);
    s1.escribir(0, 10);
    RegistroSuperficial s2 = s1;      // copia por defecto → comparte el arreglo
    s2.escribir(0, 99);               // modificamos "solo" la copia...
    std::cout << "Direccion s1: " << s1.direccion() << " | s2: " << s2.direccion() << "\n";
    std::cout << "s1.leer(0) = " << s1.leer(0) << "  (¡cambio aunque escribimos en s2!)\n";
    s1.liberar();                     // se libera UNA sola vez
    // s2.liberar();  // ✗ ¡NO! Sería un doble delete sobre el mismo arreglo.

    std::cout << "\n=== 2) Copia profunda (regla de los tres) ===\n";
    RegistroProfundo p1(3);
    p1.escribir(0, 10);
    RegistroProfundo p2 = p1;         // usa NUESTRO constructor de copia
    p2.escribir(0, 99);
    std::cout << "Direccion p1: " << p1.direccion() << " | p2: " << p2.direccion() << "\n";
    std::cout << "p1.leer(0) = " << p1.leer(0) << "  (el original no cambio)\n";

    std::cout << "\n=== 3) Regla del cero (std::vector) ===\n";
    RegistroModerno m1(3);
    m1.escribir(0, 10);
    RegistroModerno m2 = m1;          // copia profunda generada por el compilador
    m2.escribir(0, 99);
    std::cout << "Direccion m1: " << m1.direccion() << " | m2: " << m2.direccion() << "\n";
    std::cout << "m1.leer(0) = " << m1.leer(0) << "  (el original no cambio)\n";

    // Conclusión: prefiere std::vector, std::string y unique_ptr sobre punteros crudos.
    return 0;
}
