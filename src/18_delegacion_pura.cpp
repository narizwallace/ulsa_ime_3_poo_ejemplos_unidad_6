// =============================================================================
// Diapositiva 18 — Reenvío vs delegación pura
// =============================================================================
// OBJETIVO
//   Distinguir dos formas de delegar:
//     - REENVÍO (lo común en C++): el delegado atiende la petición sin saber
//       quién lo llamó.
//     - DELEGACIÓN PURA: el delegado recibe también al objeto original ("self")
//       y puede actuar SOBRE él. En lenguajes con prototipos (JavaScript) esto
//       viene incluido; en C++ se simula pasando una referencia.
//
// QUÉ OBSERVAR AL EJECUTAR
//   - Parte 1 (reenvío): la locomoción imprime un mensaje genérico.
//   - Parte 2 (pura): la locomoción conoce al robot que la invocó, usa su nombre
//     y gasta SU batería.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/18_delegacion_pura.cpp -o build/18
//   ./build/18
// =============================================================================

#include <iostream>
#include <memory>
#include <string>

// =============================================================================
// PARTE 1 — Reenvío simple
// =============================================================================
class LocomocionSimple {
public:
    virtual ~LocomocionSimple() = default;
    // El delegado recibe SOLO los datos de la petición. No sabe quién lo llamó.
    virtual void mover(int distanciaCm) = 0;
};

class RuedasSimples : public LocomocionSimple {
public:
    void mover(int distanciaCm) override {
        std::cout << "  (alguien) rodo " << distanciaCm << " cm\n";
    }
};

class RobotSimple {
private:
    std::unique_ptr<LocomocionSimple> locomocion;

public:
    explicit RobotSimple(std::unique_ptr<LocomocionSimple> l) : locomocion(std::move(l)) {}

    // Reenvío: se pasa solo la distancia.
    void avanzar(int d) { locomocion->mover(d); }
};

// =============================================================================
// PARTE 2 — Delegación "pura": el delegado recibe al objeto original
// =============================================================================
class Robot;   // declaración adelantada: la interfaz necesita mencionar a Robot

class Locomocion {
public:
    virtual ~Locomocion() = default;
    // "Robot& quien" representa a self: el objeto en cuyo nombre se actúa.
    virtual void mover(Robot& quien, int distanciaCm) = 0;
};

class Robot {
private:
    std::string nombre;
    int bateria;                              // porcentaje
    std::unique_ptr<Locomocion> locomocion;

public:
    Robot(std::string n, std::unique_ptr<Locomocion> l)
        : nombre(std::move(n)), bateria(100), locomocion(std::move(l)) {}

    const std::string& getNombre() const { return nombre; }
    int getBateria() const { return bateria; }
    void consumirBateria(int porcentaje) { bateria -= porcentaje; }

    // *this es el objeto actual: se lo pasamos al delegado como "self".
    void avanzar(int d) { locomocion->mover(*this, d); }
};

// Estas clases se definen DESPUÉS de Robot porque usan sus métodos
// (getNombre, consumirBateria), y Robot debe estar completo para eso.
class Ruedas : public Locomocion {
public:
    void mover(Robot& quien, int distanciaCm) override {
        std::cout << "  " << quien.getNombre() << " rodo " << distanciaCm << " cm\n";
        quien.consumirBateria(distanciaCm / 50);       // gasta POCA batería
    }
};

class Orugas : public Locomocion {
public:
    void mover(Robot& quien, int distanciaCm) override {
        std::cout << "  " << quien.getNombre() << " avanzo con orugas " << distanciaCm << " cm\n";
        quien.consumirBateria(distanciaCm / 20);       // gasta MÁS batería
    }
};

int main() {
    std::cout << "=== Parte 1: reenvio (el delegado no sabe quien lo llamo) ===\n";
    RobotSimple simple(std::make_unique<RuedasSimples>());
    simple.avanzar(100);

    std::cout << "\n=== Parte 2: delegacion pura (el delegado actua sobre self) ===\n";
    Robot r1("R1", std::make_unique<Ruedas>());
    Robot r2("R2", std::make_unique<Orugas>());
    r1.avanzar(100);
    r2.avanzar(100);

    // El delegado modificó el estado del robot que lo invocó.
    std::cout << "Bateria de R1: " << r1.getBateria() << "%\n";   // 98%
    std::cout << "Bateria de R2: " << r2.getBateria() << "%\n";   // 95%

    // [Nota de C++] En el reenvío el delegado no puede hacer nada de esto
    // (no conoce al robot). En la delegación pura el delegado y el delegante
    // quedan más ACOPLADOS: el delegado depende de la interfaz pública de Robot.
    return 0;
}
