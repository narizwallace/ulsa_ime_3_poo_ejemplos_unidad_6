// =============================================================================
// Diapositiva 21 — Delegación en la práctica
// =============================================================================
// Tres usos cotidianos de la delegación en un solo archivo:
//
//   PARTE A  Callbacks con std::function (el sensor delega "qué hacer")
//   PARTE B  Adaptador: una Pila implementada sobre std::vector
//   PARTE C  Semáforo → Conductor → Automóvil (Unidad II, adelanto de Observer)
//
// Strategy (otro uso de la delegación) está en 16_delegacion_locomocion.cpp.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/21_delegacion_en_la_practica.cpp -o build/21
//   ./build/21
// =============================================================================

#include <functional>   // std::function
#include <iostream>
#include <string>
#include <vector>

// =============================================================================
// PARTE A — Callbacks con std::function
// =============================================================================
// El sensor NO sabe qué hacer cuando detecta algo. Delega esa decisión en una
// función que le entrega quien lo usa. Mismo sensor, comportamientos distintos.
class Sensor {
private:
    // std::function<void(int)>: "cualquier cosa invocable con un int que no
    // devuelve nada" (función libre, lambda, objeto-función...).
    std::function<void(int)> alDetectar;

public:
    void conectar(std::function<void(int)> accion) { alDetectar = std::move(accion); }

    void leer(int valor) {
        // Un std::function vacío es "falso": evitamos llamarlo si nadie lo conectó.
        if (alDetectar) {
            alDetectar(valor);      // DELEGACIÓN: el sensor cede la decisión
        }
    }
};

// Función libre: otra forma de ser delegado.
void registrarEnBitacora(int valor) {
    std::cout << "  [bitacora] lectura = " << valor << "\n";
}

// =============================================================================
// PARTE B — Adaptador: Pila delega TODO en std::vector
// =============================================================================
// Pila ofrece solo push/pop/top (una interfaz más pequeña y segura), pero no
// reimplementa el almacenamiento: lo delega en el vector.
class Pila {
private:
    std::vector<int> elementos;

public:
    void apilar(int v) { elementos.push_back(v); }            // delega
    void desapilar() { elementos.pop_back(); }                // delega
    int cima() const { return elementos.back(); }             // delega
    bool vacia() const { return elementos.empty(); }          // delega
};

// =============================================================================
// PARTE C — Semáforo → Conductor → Automóvil
// =============================================================================
// El semáforo NO manda al automóvil: avisa a los conductores y cada conductor
// decide qué hacer con SU automóvil. La dependencia se invierte y el semáforo
// no conoce a los automóviles.
class Automovil {
private:
    std::string placa;

public:
    explicit Automovil(std::string p) : placa(std::move(p)) {}
    void frenar() const { std::cout << "  Automovil " << placa << ": frenando\n"; }
    void avanzar() const { std::cout << "  Automovil " << placa << ": avanzando\n"; }
};

class Conductor {
private:
    std::string nombre;
    Automovil* automovil;   // el conductor manda a SU automóvil

public:
    Conductor(std::string n, Automovil* a) : nombre(std::move(n)), automovil(a) {}

    // El semáforo llama a este método; el conductor decide y delega en el auto.
    void reaccionar(bool luzVerde) const {
        std::cout << "  " << nombre << " ve la luz " << (luzVerde ? "verde" : "roja") << "\n";
        if (luzVerde) automovil->avanzar();
        else automovil->frenar();
    }
};

class Semaforo {
private:
    bool verde = false;
    std::vector<Conductor*> conductores;   // conoce a los Conductores, NO a los Automóviles

public:
    void suscribir(Conductor* c) { conductores.push_back(c); }

    void cambiarLuz() {
        verde = !verde;
        std::cout << "Semaforo cambia a " << (verde ? "VERDE" : "ROJO") << "\n";
        for (const Conductor* c : conductores) {
            c->reaccionar(verde);          // delegación: notifica a todos
        }
    }
};

int main() {
    std::cout << "=== Parte A: callbacks ===\n";
    Sensor sensor;

    // 1) Una lambda: función anónima escrita en el sitio.
    sensor.conectar([](int v) { std::cout << "  Alarma si v > 50. v = " << v << "\n"; });
    sensor.leer(70);

    // 2) Una función libre.
    sensor.conectar(registrarEnBitacora);
    sensor.leer(70);

    // 3) Una lambda que CAPTURA una variable local por referencia ([&]).
    int detecciones = 0;
    sensor.conectar([&detecciones](int) { ++detecciones; });
    sensor.leer(1);
    sensor.leer(2);
    sensor.leer(3);
    std::cout << "  El callback conto " << detecciones << " detecciones\n";
    // [Nota de C++] Capturar por referencia exige que la variable viva más que
    // la lambda; aquí 'detecciones' vive hasta el final de main, así que es seguro.

    std::cout << "\n=== Parte B: adaptador (Pila sobre vector) ===\n";
    Pila pila;
    pila.apilar(10);
    pila.apilar(20);
    std::cout << "  cima = " << pila.cima() << "\n";    // 20
    pila.desapilar();
    std::cout << "  cima = " << pila.cima() << "\n";    // 10

    std::cout << "\n=== Parte C: Semaforo -> Conductor -> Automovil ===\n";
    Automovil a1("ABC-123");
    Automovil a2("XYZ-789");
    Conductor c1("Ana", &a1);
    Conductor c2("Luis", &a2);

    Semaforo semaforo;
    semaforo.suscribir(&c1);
    semaforo.suscribir(&c2);
    semaforo.cambiarLuz();    // VERDE
    semaforo.cambiarLuz();    // ROJO

    return 0;
}
