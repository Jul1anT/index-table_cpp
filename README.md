# index-table_cpp

Árbol B+ (orden 4) implementado desde cero en C++20 como índice para una tabla de empleados (id_edad → nombre, salario, cargo). Soporta inserción masiva y búsqueda exacta en O(log n).

## ¿Qué es un Árbol B+?

Un Árbol B+ es una estructura de datos de árbol autobalanceado que mantiene los datos ordenados y permite búsquedas, acceso secuencial, inserciones y eliminaciones en tiempo logarítmico. Es una variante del Árbol B optimizada para sistemas que leen y escriben bloques grandes de datos.

### Características principales:

1. **Orden del árbol**: Define el número máximo de hijos que puede tener un nodo. En nuestra implementación usamos orden 4.
2. **Todos los datos en las hojas**: A diferencia del Árbol B, todos los valores se almacenan en los nodos hoja.
3. **Hojas enlazadas**: Los nodos hoja están conectados en una lista enlazada, permitiendo recorridos secuenciales eficientes.
4. **Nodos internos como índice**: Los nodos internos solo almacenan claves para guiar la búsqueda.

## Diferencias entre Árbol B y Árbol B+

| Característica | Árbol B | Árbol B+ |
|---------------|---------|----------|
| **Almacenamiento de datos** | Datos en todos los nodos | Datos solo en hojas |
| **Duplicación de claves** | No hay duplicación | Las claves se duplican en nodos internos |
| **Búsqueda** | Puede terminar en cualquier nivel | Siempre termina en las hojas |
| **Acceso secuencial** | Requiere recorrido in-order del árbol | Eficiente mediante lista enlazada de hojas |
| **Uso de espacio** | Más eficiente en espacio | Usa más espacio por duplicación |
| **Rendimiento en rangos** | Menos eficiente | Muy eficiente |

## Complejidades de Tiempo

| Operación | Complejidad |
|-----------|-------------|
| **Búsqueda** | O(log n) |
| **Inserción** | O(log n) |
| **Eliminación** | O(log n) |
| **Búsqueda por rango** | O(log n + k), donde k es el número de elementos en el rango |
| **Recorrido secuencial** | O(n) |

### Justificación:

- **Búsqueda O(log n)**: La altura del árbol es O(log n) porque cada nodo tiene al menos ⌈m/2⌉ hijos (donde m es el orden).
- **Inserción O(log n)**: Encontrar la hoja correcta es O(log n), y las divisiones de nodos se propagan hacia arriba en O(log n).
- **Espacio O(n)**: El árbol almacena n elementos con un overhead constante por nodo.

## Estructura del Proyecto

```
index-table_cpp/
├── BPlusTree.h     # Implementación del Árbol B+ (orden 4)
├── main.cpp        # Programa principal
├── input.txt       # Ejemplo de entrada
├── output.txt      # Ejemplo de salida esperada
└── README.md       # Este archivo
```

## Estructura del Registro

```cpp
struct Employee {
    int id_edad;        // Clave de indexación
    std::string nombre; // Nombre del empleado
    double salario;     // Salario
    std::string cargo;  // Cargo/posición
};
```

## Formato de Entrada

```
n                           # Número de empleados
id_edad nombre salario cargo  # Repetir n veces
q                           # Número de consultas
1 id                        # Consulta: buscar por id_edad
```

## Compilación y Ejecución

```bash
# Compilar con C++20
g++ -std=c++20 -o main main.cpp

# Ejecutar con archivo de entrada
./main < input.txt

# O ejecutar y comparar con salida esperada
./main < input.txt > resultado.txt
diff resultado.txt output.txt
```

## Ejemplo

**Entrada (input.txt):**
```
5
101 Juan 2500.50 Ingeniero
102 Maria 3200.75 Gerente
103 Carlos 1800.00 Analista
104 Ana 4500.25 Director
105 Pedro 2100.00 Tecnico
6
1 101
1 103
1 106
1 104
1 102
1 999
```

**Salida (output.txt):**
```
101 Juan 2500.50 Ingeniero
103 Carlos 1800.00 Analista
No encontrado
104 Ana 4500.25 Director
102 Maria 3200.75 Gerente
No encontrado
```

## Autor

Implementación en C++20 para demostrar el uso de Árboles B+ como estructuras de índice en bases de datos.
