# Ejemplos de la Unidad VI — Prototipos y Delegación

Este repositorio acompaña la presentación de la Unidad VI. Cada archivo de `src/` es un
programa **independiente** (con su propio `main`) que corresponde a una o más
diapositivas. El número al inicio del nombre es el número de la diapositiva.

## Cómo usar los ejemplos

1. Abre en el editor el archivo de la diapositiva que estamos viendo.
2. Lee primero el **encabezado**: dice el objetivo, las ideas clave y qué observar al ejecutar.
3. Compila y ejecuta (desde la raíz del repo):

```bash
mkdir -p build
g++ -Wall -Wextra -std=c++17 src/08_clonar_jerarquia.cpp -o build/08
./build/08
```

Para compilar todos de una vez:

```bash
mkdir -p build
for f in src/*.cpp; do g++ -Wall -Wextra -std=c++17 "$f" -o "build/$(basename "$f" .cpp)"; done
```

La carpeta `build/` está en el `.gitignore`: los ejecutables no se suben al repositorio.

## Qué significan los comentarios

- **Encabezado del archivo:** objetivo, ideas clave, qué observar y cómo compilar.
- **`[Nota de C++]`:** una razón técnica del lenguaje (por qué se escribe así).
- **`✗`:** código que **no** debe hacerse, o que no compila; casi siempre está comentado
  para que puedas descomentarlo y ver el error.
- **Comentarios línea a línea:** explican las líneas relevantes del tema, no la sintaxis obvia.

## Índice

| Diapositiva | Archivo | Qué muestra |
|---|---|---|
| 6 | `src/06_slicing_copia_fallida.cpp` | Por qué copiar con un puntero a la base rebana el objeto (slicing) |
| 7 y 8 | `src/08_clonar_jerarquia.cpp` | `clonar()` como constructor virtual en una jerarquía de robots |
| 9 | `src/09_notas_cpp_clonar.cpp` | Retorno covariante, constructor de copia `protected` y qué hace `std::move` |
| 10 | `src/10_copia_superficial_vs_profunda.cpp` | Copia superficial, regla de los tres y regla del cero |
| 11 y 12 | `src/11_12_catalogo_y_fabrica.cpp` | Catálogo de prototipos y fábrica de robots por plantillas |
| 16 | `src/16_delegacion_locomocion.cpp` | El robot delega su locomoción en un objeto colaborador |
| 17 | `src/17_cambiar_comportamiento_en_ejecucion.cpp` | Reemplazar el delegado en ejecución y validar `nullptr` |
| 18 | `src/18_delegacion_pura.cpp` | Reenvío vs delegación pura (el delegado recibe a `self`) |
| 19 | `src/19_cadena_de_prototipos.cpp` | Cadena de prototipos: delegar la búsqueda al padre |
| 21 | `src/21_delegacion_en_la_practica.cpp` | Callbacks con `std::function`, adaptador `Pila` y Semáforo → Conductor → Automóvil |
| 23 | `src/23_integrador_prototipo_y_delegacion.cpp` | Robot que se clona y delega la copia en su locomoción |

Las diapositivas sin archivo (1–5, 13–15, 20, 22 y 24) son conceptuales y no llevan código ejecutable.

## Orden de lectura sugerido

Prototipos: `06` → `08` → `09` → `10` → `11_12`.
Delegación: `16` → `17` → `18` → `19` → `21`.
Integración: `23`.

## Reto para casa

1. En `11_12`, agrega un `RobotEnsamblador` y regístralo en el catálogo **sin modificar** la clase `CatalogoDeRobots`.
2. En `16`, agrega una locomoción `Patas` y comprueba que `Robot` no cambia.
3. En `23`, ¿qué pasa si `Orugas` olvida implementar `clonar()`? Compila y lee el error.
