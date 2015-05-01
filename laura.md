
# Herramientas

## IDEs y compiladores

Te recomiendo no usar Visual Studio para aprender (Muchos bugs en el compilador, MS hace las cosas a su bola, etc). Alternativas:

### Compiladores:

 - GCC: Para windows lo tienes en cualquier distro de MinGW o cygwin. Personalmente prefiero cygwin. De MinGW bájate [este](http://nuwen.net/mingw.html).
 - Clang: El claro ganador en cuanto a compiladores de C++ hoy en día. La interfaz CLI es la misma que la de gcc, no tienes que aprender nada nuevo ;). 
          Para windows no se si lo hay, cygwin tiene un paquete. [Bájate esta terminal para windows anda...](http://babun.github.io/).
 - Si de verdad quieres morir... Visual Studio xD. Cuanto mas nuevo mejor.

### IDEs:

 - Netbeans: A mi siempre me ha ido de puta madre. Es java, así que te comerá toda la RAM que tengas...
 - Eclipse CDT: Horror, pero prueba a ver si te gusta. Para gustos los colores.
 - Clion: Un nuevo IDE para C++ multiplataforma que se los está comiendo a todos con patatitas. Te lo recomiendo 100%.

## Build systems

Como comprenderás la gente no suele llamar a GCC a pelo xD. Ni siquiera escribir Makefiles. Te recomiendo aprender [CMake](http://www.cmake.org/) desde el principio, es el estandar del mercado.
La idea de CMake es que es un build system multiplataforma que compila al "Makefile" equivalente de tu sistema. Por ejemplo, si le dices que quieres un makefile te hace un makefile, si le dices que estas usando Visual Studio te genera una solución de Visual Studio, etc.
Aviso que está documentado con el culo y a veces es un horror. StackOverflow a mansalva xD

# ¿Qué aprender y en qué orden?

Pues no se que decirte chica xD. Para mi los conceptos mas importantes son (En orden decreciente de importancia):

 1. Value semantics
 2. RAII
 3. Lo que tu escribes no es lo que hará, es una representación de alto nivel.
    No se si me explico. La idea es que entiendas que no puedes pensar en tu código
    como "Ah, pues aquí hará esto". 99% de probabilidad de error ;) Piensa en tu código 
    como en una descripción de alto nivel. Con el tiempo te irás dando cuenta.
 4. Funcionamiento del runtime/SO: Proceso, memoria virtual, heap, stack, etc.
 5. El lenguaje en sí (Clases, plantillas, sobrecarga de funciones, etc). Qué parte del lenguaje
    es mas importante? Es un puto elefante, no se que decirte. Intenta aprenderlo todo y elije según la
    situación.

Lo que quiero decir con esos cinco puntos es que lo primero que tienes que hacer es tener los dos primeros conceptos muy claros,
y que mientras aprendas cosas del lenguaje no se te olviden el resto. En que orden ver el lenguaje no se, si estas acostumbrada
a OOP aprende clases primero.

Cualquier cosa pregunta.

# Sobre los ejemplos de este repo:

Un pequeño resumen de los ejemplos que vienen en este repo. Le he pegado un repaso y no son sencillos ninguno la verdad xD.

 - `instantation_profiler.cpp/hpp`: Es un ejemplo para entender las value semantics del lenguaje (Eso de que el objeto vive en la pila y no lo puedes
   mover de ahí, en contraposición a java y sus punteros). La gracia del ejemplo es que lo compiles activando y desactivando una optimización concreta
   (Preguntame si lo quieres mirar).

 - `aabb_2d/3d`: Ya te puedes imaginar lo que es. No estoy muy orgulloso del resultado... Pero bueno funciona.
 - `math_2d`: vector 2d. Lo mismo que arriba. Para estas dos cosas mejor si miras [este (el de este año)](https://github.com/Manu343726/octopus/blob/master/blocks/manu343726/math/vector.hpp). No es trivial, aviso.
 - `string.hpp`: Una clase string facilona, para aprender como se hacen los constructores, destructor, operadores de asignación, etc.
                 Para un ejemplo de implementación de string mas cercano a lo que se hace en la realidad mira [esto](https://gist.github.com/Manu343726/02287de75bb24f2cef00).
 - `binary_literals.hpp`: Desde C++11 puedes inventarte tus propios literales. Es decir igual que 10.0f significa que es un 10.0 `float`, y 
                          10u un 10 `unsigned int`, ahora te puedes inventar tus propios sufijos ([referencia](http://en.cppreference.com/w/cpp/language/user_literal)). Ese ejemplo implementa el sufijo `_b` para meter números en binario. Es tan enrevesado el ejemplo porque primero te calcula
                          el tipo entero mas pequeño que puede guardar el número de bits que has escrito.
 - `sortvsqsort.cpp`: Una comparativa del "quicksort" de C con el de C++. Moraleja: El de C++ va 2.5 veces mas rápido (No castings, no punteros a funciones, CPU feliz).
 - `to_string.pp`: Como aprenderás, C++ codifica información de una función/tipo en su nombre, a esto se le llama "name mangling". Esta es una implementación
   supuestamente portable de "demanglear" los nombres para volver a ver ese bonito `f()` en lugar de `__ZSXX_2i_w_f()`.

 - `polinomial.hpp`: Este es de los mas avanzados. Es un ejemplo de la técnica conocida como [expression templates](http://en.wikipedia.org/wiki/Expression_templates). El ejemplo consiste en inventarse una "gramática" que permite inicializar una variable que representa un polinomio mas o menos así:

   ```cpp
   poli p = 2*X^2 + 1; //Código C++ válido y que compila ;)
   ```

 - `scoped_resource.hpp`: Una utilidad para entender RAII. Encapsula un recurso que no utiliza RAII.
 - `event.hpp`: Plantilla que representa un evento de una signatura cualquiera. La idea es que una variable de esas representa
   un evento, pudiendo lanzarlo y añadir callbacks que respondan a él. Enrevesado por lo genérico que es. En C++ se suele llamar
   signal y slot, no evento.

 - `Particles/`: Es un motor de partículas personalizable. La idea es enseñar Policy Based Designs, que es la misma idea del diseño por componentes. 
   Pero esos componentes se pasan como parámetros de plantilla, con lo que el código y datos es totalemtente optimizable por el compilador y te saca 
   código muy rápido. El ejemplo es largo, no lo voy a explicar aquí.