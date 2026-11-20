// =============================================================================
// Diapositiva 9 — Notas de C++: dueño claro, retorno de clonar() y std::move
// =============================================================================
// Este archivo reúne tres ideas pequeñas que aparecen en el código de la unidad.
//
//   PARTE A  Retorno covariante con punteros crudos (y por qué unique_ptr no lo tiene)
//   PARTE B  Constructor de copia protected en la base (evitar slicing por accidente)
//   PARTE C  Qué hace realmente std::move
//
// CÓMO COMPILAR Y EJECUTAR (desde la raíz del repo)
//   g++ -Wall -Wextra -std=c++17 src/09_notas_cpp_clonar.cpp -o build/09
//   ./build/09
// =============================================================================

#include <iostream>
#include <memory>
#include <string>

// =============================================================================
// PARTE A — Retorno covariante
// =============================================================================
// Al sobrescribir un método virtual, normalmente el tipo de retorno debe ser
// idéntico. EXCEPCIÓN: si devuelve puntero o referencia a una clase, la
// derivada puede devolver un puntero a una clase DERIVADA del tipo original.
// Eso se llama retorno covariante.
class RobotCov {
public:
    virtual ~RobotCov() = default;
    virtual RobotCov* clonar() const = 0;     // devuelve RobotCov*
};

class SoldadorCov : public RobotCov {
private:
    int amperaje = 120;

public:
    // Covariante: devuelve SoldadorCov*, más específico que RobotCov*.
    // Ventaja: quien llama sobre un SoldadorCov* obtiene un SoldadorCov*
    // sin necesidad de convertir (cast).
    SoldadorCov* clonar() const override { return new SoldadorCov(*this); }

    int getAmperaje() const { return amperaje; }
};

// Con std::unique_ptr NO hay covarianza: unique_ptr<SoldadorCov> y
// unique_ptr<RobotCov> son tipos distintos sin relación de herencia entre sí.
// Por eso, en la unidad, TODAS las clases devuelven std::unique_ptr<Robot>.
// A cambio ganamos algo más importante: dueño claro y cero fugas de memoria.

// =============================================================================
// PARTE B — Constructor de copia protected en la base
// =============================================================================
// Si la base es CONCRETA, cualquiera podría escribir RobotProtegido copia(*ptr)
// y rebanar el objeto sin darse cuenta. Si el constructor de copia es protected,
// solo las clases derivadas (y los métodos de la propia clase) pueden usarlo.
class RobotProtegido {
private:
    std::string nombre;

protected:
    // Solo accesible desde la propia clase y sus derivadas.
    RobotProtegido(const RobotProtegido&) = default;

public:
    explicit RobotProtegido(std::string n) : nombre(std::move(n)) {}
    virtual ~RobotProtegido() = default;

    virtual std::unique_ptr<RobotProtegido> clonar() const {
        // Dentro de un método de la clase sí se puede usar el ctor protegido.
        // [Nota de C++] Aquí NO podemos usar std::make_unique: la copia la haría
        // el código interno de make_unique, que está FUERA de la clase y por eso
        // no tiene acceso al constructor protected. Con "new" directo, el acceso
        // ocurre dentro del método y sí funciona.
        return std::unique_ptr<RobotProtegido>(new RobotProtegido(*this));
    }
    virtual void describir() const { std::cout << "Robot " << nombre << "\n"; }
};

class SoldadorProtegido : public RobotProtegido {
private:
    int amperaje;

public:
    SoldadorProtegido(std::string n, int a) : RobotProtegido(std::move(n)), amperaje(a) {}

    // El constructor de copia implícito de SoldadorProtegido (público) llama al
    // ctor de copia protegido de la base: eso SÍ está permitido (es una derivada),
    // y aquí make_unique funciona porque el ctor de SoldadorProtegido es público.
    std::unique_ptr<RobotProtegido> clonar() const override {
        return std::make_unique<SoldadorProtegido>(*this);
    }
    void describir() const override { std::cout << "Soldador a " << amperaje << " A\n"; }
};

// =============================================================================
// PARTE C — std::move
// =============================================================================
// std::move NO mueve nada: solo CONVIERTE una expresión en "rvalue" ("este
// objeto ya no lo voy a usar"). Quien hace el trabajo es el constructor (o la
// asignación) de movimiento del tipo, que roba los recursos internos.
class Etiqueta {
private:
    std::string texto;

public:
    // Patrón habitual: recibir por VALOR y mover al atributo.
    //  - Si el llamador pasa un temporal, se mueve (sin copias).
    //  - Si pasa una variable, se copia UNA vez al parámetro y se mueve al atributo.
    explicit Etiqueta(std::string t) : texto(std::move(t)) {}
    const std::string& getTexto() const { return texto; }
};

int main() {
    // ---- Parte A ----
    std::cout << "=== Parte A: retorno covariante ===\n";
    SoldadorCov original;
    SoldadorCov* copia = original.clonar();      // ya es SoldadorCov*, sin cast
    std::cout << "Amperaje de la copia: " << copia->getAmperaje() << " A\n";
    delete copia;   // con punteros crudos hay que acordarse del delete

    // ---- Parte B ----
    std::cout << "\n=== Parte B: constructor de copia protected ===\n";
    SoldadorProtegido soldador("Maestro", 150);
    RobotProtegido* base = &soldador;

    // ✗ Esta línea NO compila (descoméntala para ver el error):
    //   RobotProtegido rebanado(*base);
    //   error: 'RobotProtegido::RobotProtegido(const RobotProtegido&)' is protected
    // La única forma de copiar a través de la base es clonar():
    auto copiaProtegida = base->clonar();
    copiaProtegida->describir();   // conserva el tipo real: "Soldador a 150 A"

    // ---- Parte C ----
    std::cout << "\n=== Parte C: std::move ===\n";

    // C1. Copia vs movimiento con std::string.
    std::string a = "una cadena lo bastante larga para vivir en el heap";
    const char* bufferOriginal = a.data();

    std::string b = a;              // COPIA: b tiene su propio buffer
    std::cout << "Despues de copiar: a sigue valiendo \"" << a.substr(0, 9) << "...\"\n";

    std::string c = std::move(a);   // MOVIMIENTO: c se queda con el buffer de a
    // Demostración (no es una garantía del estándar): c apunta al MISMO buffer
    // que tenía a, es decir, no se copiaron los caracteres.
    std::cout << "c reutilizo el buffer de a? "
              << (c.data() == bufferOriginal ? "si" : "no") << "\n";
    // [Nota de C++] Después de mover, a queda en un estado VÁLIDO pero NO
    // ESPECIFICADO: se puede destruir o reasignar, pero no hay que depender de su valor.
    std::cout << "b (copia) y c (movida) tienen " << b.size() << " y " << c.size()
              << " caracteres\n";

    // C2. unique_ptr no se puede copiar; solo se puede mover.
    std::unique_ptr<int> p = std::make_unique<int>(42);
    // auto q = p;                  // ✗ NO compila: unique_ptr no se copia (dueño único)
    auto q = std::move(p);          // ✓ la propiedad pasa de p a q
    std::cout << "p es nullptr? " << (p == nullptr ? "si" : "no")
              << " | q apunta a " << *q << "\n";

    // C3. El patrón "por valor + move" del constructor.
    Etiqueta e("soldador-estandar");
    std::cout << "Etiqueta: " << e.getTexto() << "\n";

    return 0;
}
