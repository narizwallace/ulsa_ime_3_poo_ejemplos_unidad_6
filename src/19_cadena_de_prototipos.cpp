// =============================================================================
// Diapositiva 19 — Cadena de prototipos: delegar la búsqueda al padre
// =============================================================================
// OBJETIVO
//   Simular en C++ cómo lenguajes como JavaScript implementan su "herencia":
//   si un objeto no tiene una propiedad, DELEGA la búsqueda en su prototipo,
//   y este en el suyo, hasta llegar al final de la cadena.
//
// IDEAS CLAVE
//   - Aquí no hay clases derivadas: hay OBJETOS que apuntan a otros objetos.
//   - Cada objeto guarda solo lo que lo hace distinto de su prototipo.
//   - Una propiedad propia "tapa" (sobrescribe) a la del prototipo.
//
// QUÉ OBSERVAR AL EJECUTAR
//   - miRobot encuentra "tipo" en su prototipo y "material" en el prototipo
//     del prototipo.
//   - "color" está en miRobot y en objetoBase: gana la más cercana.
//   - Si cambia el prototipo, los objetos que dependen de él ven el cambio.
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/19_cadena_de_prototipos.cpp -o build/19
//   ./build/19
// =============================================================================

#include <iostream>
#include <map>
#include <string>

class Objeto {
private:
    // Puntero (NO dueño) al prototipo. nullptr = final de la cadena.
    Objeto* prototipo;
    std::map<std::string, std::string> propiedades;

public:
    explicit Objeto(Objeto* p = nullptr) : prototipo(p) {}

    void set(const std::string& clave, const std::string& valor) {
        propiedades[clave] = valor;
    }

    // ¿La propiedad está en ESTE objeto (sin mirar la cadena)?
    bool tienePropia(const std::string& clave) const {
        return propiedades.count(clave) > 0;
    }

    // Búsqueda con DELEGACIÓN:
    //   1) Si el objeto tiene la propiedad, la devuelve.
    //   2) Si no, delega la búsqueda en su prototipo (llamada recursiva).
    //   3) Si no hay prototipo, la propiedad no existe.
    std::string get(const std::string& clave) const {
        auto it = propiedades.find(clave);
        if (it != propiedades.end()) {
            return it->second;                                     // caso 1
        }
        return prototipo ? prototipo->get(clave) : "(indefinido)"; // casos 2 y 3
    }
};

int main() {
    // Eslabón 1 (raíz de la cadena): lo común a todo.
    Objeto objetoBase;
    objetoBase.set("material", "acero");
    objetoBase.set("color", "gris");

    // Eslabón 2: un robot "genérico" que delega en objetoBase.
    Objeto robotGenerico(&objetoBase);
    robotGenerico.set("tipo", "industrial");

    // Eslabón 3: un robot concreto que delega en robotGenerico.
    Objeto miRobot(&robotGenerico);
    miRobot.set("nombre", "R-01");
    miRobot.set("color", "rojo");     // "tapa" el color gris de objetoBase

    std::cout << "--- Busquedas desde miRobot ---\n";
    std::cout << "nombre:   " << miRobot.get("nombre")   << "  (propia)\n";
    std::cout << "tipo:     " << miRobot.get("tipo")     << "  (delegada a robotGenerico)\n";
    std::cout << "material: " << miRobot.get("material") << "  (delegada 2 niveles, a objetoBase)\n";
    std::cout << "color:    " << miRobot.get("color")     << "  (propia; tapa al gris del prototipo)\n";
    std::cout << "voltaje:  " << miRobot.get("voltaje")   << "  (no existe en la cadena)\n";

    std::cout << "\n--- tienePropia ---\n";
    std::cout << "miRobot tiene 'tipo' propio?    " << (miRobot.tienePropia("tipo") ? "si" : "no") << "\n";
    std::cout << "miRobot tiene 'nombre' propio?  " << (miRobot.tienePropia("nombre") ? "si" : "no") << "\n";

    // Cambiar el prototipo afecta a quienes delegan en él (la búsqueda es en vivo).
    std::cout << "\n--- Cambiamos el prototipo ---\n";
    objetoBase.set("material", "aluminio");
    std::cout << "material de miRobot ahora: " << miRobot.get("material") << "\n";

    // [Nota de C++] El prototipo debe VIVIR MÁS que los objetos que lo referencian.
    // Aquí los tres son variables locales de main: se destruyen en orden inverso
    // al de su creación (miRobot, robotGenerico, objetoBase), así que es seguro.
    // Si un prototipo se destruyera antes, el puntero quedaría "colgante"
    // (dangling) y usarlo sería comportamiento indefinido.
    return 0;
}
