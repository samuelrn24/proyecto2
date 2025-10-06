# PRACTICE II: CANONICAL HUFFMAN CODING IN C++

---
## Members of the project
Nicolás Reyes Cano, Samuel Ramirez Noreña y Tomás Vera Franco
# Email adresses
Nreyesc@eafit.edu.co
Sramirezn1@eafit.edu.co
tveraf@eafit.edu.co

## 🇬🇧 ENGLISH SECTION

### 🧠 Overview
This project implements a compression system using **canonical Huffman codes**. The program reads a line of text of at least 30 characters within the printable ASCII range (32–126), constructs the Huffman tree from the frequencies of each symbol, extracts the code lengths, and uses these lengths to generate deterministic canonical codes. It then encodes the text with these codes, calculates the compressed size and the compression ratio relative to the original size. Finally, it displays:

* Table of frequencies per symbol (character and ASCII value).
* Table of lengths, tree codes (non-canonical), and canonical codes per symbol.
* Textual representation of the tree (preordered with parentheses).
* The first 128 bits of the compressed output and its hexadecimal equivalent.

The goal is to illustrate how a deterministic compressor is built using canonical Huffman and compare the resulting size versus fixed encoding (8 bits per character).
---

### ⚙️ Compilation & Execution

#### Compile:
```bash
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o huffman
```

#### Run:
```bash
./huffman
```

#### Example input:
```
aaaaaaaaaaaa eeeeeeeeeee   nnnnnn
```
---

## Explanation of Canonical Huffman

The classic Huffman algorithm builds a full binary tree from the frequencies of symbols.
Each leaf represents a symbol, and the leaf’s depth determines the length of its code.
Although the resulting lengths are optimal, the specific assignment of 0 and 1 to each symbol depends on the order in which the tree is built; two different implementations can produce different codes even if the lengths are the same.
To eliminate this variability, the **canonical** version is used:

1. The tree’s leaves are traversed to obtain the list of code lengths for each symbol.
2. The pairs (symbol, length) are sorted first by ascending length and, in case of ties, by the symbol’s ASCII value in ascending order.
3. An integer code initialized to 0 is assigned to the first symbol of length L; its canonical binary code is that code written in L bits (padded with leading zeros).
4. For each subsequent symbol, code is incremented by 1; if the length increases compared to the previous one, code is shifted left by the difference in lengths. Then it is converted to binary with the new length.

In this way, the obtained codes are lexicographically ordered by (length, symbol) and depend only on the lengths, not on the specific shape of the tree.
Furthermore, the decoder can reconstruct the canonical tree using only the code lengths and the corresponding symbols.

## Design decisions

* **Data modeling:** Each node in the Huffman tree contains the character (only for leaves), the accumulated frequency of its subtree, and pointers to its left and right children. It also stores the smallest character in its subtree (min_char) to correctly handle tie-breaking.
* **Tie-breaking criterion:** The priority queue (priority_queue) uses a comparator that first sorts by ascending frequency (min-heap) and, in case of a tie, by the minimum character in the subtree. Leaves are inserted in ascending ASCII order so that ties are resolved strictly by the symbol’s value. This satisfies the requirement to “break ties by symbol” and guarantees reproducibility.
* **Canonical code generation:** Symbols are sorted by (length, symbol). An initial integer value of 0 is assigned to the first symbol, and for subsequent symbols the value is incremented and shifted as needed. Conversion to a binary string is done with zero-padding on the left.
* **Edge case handling:** If the text has fewer than 30 characters or includes characters outside [32,126], an error message is displayed and execution ends. If there is only one distinct symbol, it is assigned the code 0 with length 1. Carriage returns (\r) are removed when reading input on Windows.
* **Visualization:** The tree is printed in preorder with parentheses; spaces are shown as <sp> to make them visible. Frequency, length, and code tables are displayed, along with a summary of the first 128 bits of the compressed output and its hexadecimal representation.

---

## Results

### Case 1 – Many spaces
Example: `This    is    a    text    with    many      spaces      included.` 
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| <sp> | 32 | 32 |
| . | 46 | 1 |
| T | 84 | 1 |
| a | 97 | 3 |
| c | 99 | 2 |
| d | 100 | 2 |
| e | 101 | 3 |
| h | 104 | 2 |
| i | 105 | 4 |
| l | 108 | 1 |
| m | 109 | 1 |
| n | 110 | 2 |
| p | 112 | 1 |
| s | 115 | 4 |
| t | 116 | 3 |
| u | 117 | 1 |
| w | 119 | 1 |
| x | 120 | 1 |
| y | 121 | 1 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| <sp> | 32 | 1 | 0 | 0 |
| i | 4 | 4 | 1011 | 1000 |
| s | 4 | 4 | 1101 | 1001 |
| t | 3 | 4 | 1000 | 1010 |
| a | 3 | 5 | 11110 | 10110 |
| c | 2 | 5 | 10010 | 10111 |
| d | 2 | 5 | 10011 | 11000 |
| e | 3 | 5 | 11111 | 11001 |
| h | 2 | 5 | 10100 | 11010 |
| n | 2 | 5 | 11000 | 11011 |
| l | 1 | 6 | 101010 | 111000 |
| m | 1 | 6 | 101011 | 111001 |
| p | 1 | 6 | 110010 | 111010 |
| u | 1 | 6 | 110011 | 111011 |
| w | 1 | 6 | 111000 | 111100 |
| x | 1 | 6 | 111001 | 111101 |
| y | 1 | 6 | 111010 | 111110 |
| . | 1 | 7 | 1110110 | 1111110 |
| T | 1 | 7 | 1110111 | 1111111 |

**Compression Summary:**
```
Input length (characters): 66
Original size (bits): 528
Compressed size (bits): 202
Compression ratio: 0.3826
Reduction: 61.7424%

```

**Compressed output (first 128 bits):**
```
11111111101010001001000010001001000010110000010101100111110110100000111100100010101101000001110011011011011111110000000100111101...
Total compressed bits: 202

```

**Compressed output (hex):**
```
FFA890890B0567DA0F22B41CDB7F013D5AF9902377E3BC671F8
```

**Huffman tree (preorder with parentheses):**
(<sp>(((t(cd))((h(lm))i))(((n(pu))s)(((wx)(y(.T)))(ae)))))


### Case 2 – Few different symbols
Example: `AAAABBBCCCDDDAAAABBBCCCDDDAAAABBBCCCDDDAAAABBBCCCDDD`
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| A | 65 | 16 |
| B | 66 | 12 |
| C | 67 | 12 |
| D | 68 | 12 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| A | 16 | 2 | 11 | 00 |
| B | 12 | 2 | 00 | 01 |
| C | 12 | 2 | 01 | 10 |
| D | 12 | 2 | 10 | 11 |

**Compression Summary:**
```
Input length (characters): 52
Original size (bits): 416
Compressed size (bits): 104
Compression ratio: 0.2500
Reduction: 75.0000%

```

**Compressed output (first 128 bits):**
```
00000000010101101010111111000000000101011010101111110000000001010110101011111100000000010101101010111111
Total compressed bits: 104

```

**Compressed output (hex):**
```
0056AFC015ABF0056AFC015ABF
```

**Huffman tree (preorder with parentheses):**
((BC)(DA))

### Case 3 – Approximately uniform distribution
Example: `abcdefghijklmnopqrstuvwxyz0123456789!@#$%^`
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| ! | 33 | 1 |
| # | 35 | 1 |
| $ | 36 | 1 |
| % | 37 | 1 |
| 0 | 48 | 1 |
| 1 | 49 | 1 |
| 2 | 50 | 1 |
| 3 | 51 | 1 |
| 4 | 52 | 1 |
| 5 | 53 | 1 |
| 6 | 54 | 1 |
| 7 | 55 | 1 |
| 8 | 56 | 1 |
| 9 | 57 | 1 |
| @ | 64 | 1 |
| ^ | 94 | 1 |
| a | 97 | 1 |
| b | 98 | 1 |
| c | 99 | 1 |
| d | 100 | 1 |
| e | 101 | 1 |
| f | 102 | 1 |
| g | 103 | 1 |
| h | 104 | 1 |
| i | 105 | 1 |
| j | 106 | 1 |
| k | 107 | 1 |
| l | 108 | 1 |
| m | 109 | 1 |
| n | 110 | 1 |
| o | 111 | 1 |
| p | 112 | 1 |
| q | 113 | 1 |
| r | 114 | 1 |
| s | 115 | 1 |
| t | 116 | 1 |
| u | 117 | 1 |
| v | 118 | 1 |
| w | 119 | 1 |
| x | 120 | 1 |
| y | 121 | 1 |
| z | 122 | 1 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| e | 1 | 5 | 00000 | 00000 |
| f | 1 | 5 | 00001 | 00001 |
| g | 1 | 5 | 00010 | 00010 |
| h | 1 | 5 | 00011 | 00011 |
| i | 1 | 5 | 00100 | 00100 |
| j | 1 | 5 | 00101 | 00101 |
| k | 1 | 5 | 00110 | 00110 |
| l | 1 | 5 | 00111 | 00111 |
| m | 1 | 5 | 01000 | 01000 |
| n | 1 | 5 | 01001 | 01001 |
| o | 1 | 5 | 01010 | 01010 |
| p | 1 | 5 | 01011 | 01011 |
| q | 1 | 5 | 01100 | 01100 |
| r | 1 | 5 | 01101 | 01101 |
| s | 1 | 5 | 01110 | 01110 |
| t | 1 | 5 | 01111 | 01111 |
| u | 1 | 5 | 10000 | 10000 |
| v | 1 | 5 | 10001 | 10001 |
| w | 1 | 5 | 10010 | 10010 |
| x | 1 | 5 | 10011 | 10011 |
| y | 1 | 5 | 10100 | 10100 |
| z | 1 | 5 | 10101 | 10101 |
| ! | 1 | 6 | 101100 | 101100 |
| # | 1 | 6 | 101101 | 101101 |
| $ | 1 | 6 | 101110 | 101110 |
| % | 1 | 6 | 101111 | 101111 |
| 0 | 1 | 6 | 110000 | 110000 |
| 1 | 1 | 6 | 110001 | 110001 |
| 2 | 1 | 6 | 110010 | 110010 |
| 3 | 1 | 6 | 110011 | 110011 |
| 4 | 1 | 6 | 110100 | 110100 |
| 5 | 1 | 6 | 110101 | 110101 |
| 6 | 1 | 6 | 110110 | 110110 |
| 7 | 1 | 6 | 110111 | 110111 |
| 8 | 1 | 6 | 111000 | 111000 |
| 9 | 1 | 6 | 111001 | 111001 |
| @ | 1 | 6 | 111010 | 111010 |
| ^ | 1 | 6 | 111011 | 111011 |
| a | 1 | 6 | 111100 | 111100 |
| b | 1 | 6 | 111101 | 111101 |
| c | 1 | 6 | 111110 | 111110 |
| d | 1 | 6 | 111111 | 111111 |

**Compression Summary:**
```
Input length (characters): 42
Original size (bits): 336
Compressed size (bits): 230
Compression ratio: 0.6845
Reduction: 31.5476%

```

**Compressed output (first 128 bits):**
```
11110011110111111011111100000000010001000011001000010100110001110100001001010100101101100011010111001111100001000110010100111010...
Total compressed bits: 230

```

**Compressed output (hex):**
```
F3DFBF00443214C74254B635CF84653A570C72CF4D76DF8E6CEADBAFEC
```

**Huffman tree (preorder with parentheses):**
(((((ef)(gh))((ij)(kl)))(((mn)(op))((qr)(st))))((((uv)(wx))((yz)((!#)($%))))((((01)(23))((45)(67)))(((89)(@^))((ab)(cd))))))

### 📹 Demonstration Video

Add the video link here:  
➡️ [YouTube Video Link](https://youtu.be/XXXXXXXXXXX)

---

### 🤖 Acknowledgments

This project was developed with the support of **GitHub Copilot** and **CLion AI Assistant**, which were used to assist in **code optimization**, **comment clarity**, and **bug detection**.  
All final logic, structure, and implementation decisions were made by the development team.

---
## Miembros del proyecto
Nicolás Reyes Cano, Samuel Ramirez Noreña y Tomás Vera Franco
# Direcciones de correo electronico
Nreyesc@eafit.edu.co
Sramirezn1@eafit.edu.co
tveraf@eafit.edu.co

## 🇪🇸 SECCIÓN EN ESPAÑOL

### 🧠 Descripción General
Este proyecto implementa un sistema de compresión mediante **códigos de Huffman canónicos**.  El programa lee una línea de texto de al menos 30 caracteres dentro del rango ASCII imprimible (32–126), construye el árbol de Huffman a partir de las frecuencias de cada símbolo, extrae las longitudes de código y utiliza estas longitudes para generar códigos canónicos deterministas.  A continuación codifica el texto con dichos códigos, calcula el tamaño comprimido y la proporción de compresión respecto al tamaño original.  Finalmente muestra:

* Tabla de frecuencias por símbolo (carácter y valor ASCII).
* Tabla de longitudes, códigos de árbol (no canónicos) y códigos canónicos por símbolo.
* Representación textual del árbol (preorden con paréntesis).
* Los primeros 128 bits de la salida comprimida y su equivalente hexadecimal.

El objetivo es ilustrar cómo se construye un comprimidor determinista usando Huffman canónico y comparar el tamaño resultante frente a la codificación fija (8 bits por carácter).
---

### ⚙️ Compilación y Ejecución

#### Compilación:
```bash
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o huffman
```

#### Ejecución:
```bash
./huffman
```

#### Ejemplo de entrada:
```
aaaaaaaaaaaa eeeeeeeeeee   nnnnnn
```

---

## Explicación de Huffman canónico

El algoritmo de Huffman clásico construye un árbol binario completo a partir de las frecuencias de los símbolos.  Cada hoja representa un símbolo y la profundidad de la hoja determina la longitud de su código.  Aunque las longitudes resultantes son óptimas, la asignación concreta de `0` y `1` a cada símbolo depende del orden en que se construya el árbol; dos implementaciones diferentes pueden producir diferentes códigos aun cuando las longitudes sean las mismas.  Para eliminar esta variabilidad se emplea la versión **canónica**:

1. Se recorren las hojas del árbol para obtener la lista de **longitudes** de código por símbolo.
2. Se ordenan los pares `(símbolo, longitud)` primero por la longitud ascendente y, en caso de empate, por el valor ASCII del símbolo ascendente.
3. Se asigna un entero `code` inicial igual a 0 al primer símbolo de longitud `L`: su código binario canónico es ese `code` escrito en `L` bits (rellenando con ceros a la izquierda).
4. Para cada símbolo posterior se incrementa `code` en 1; si la longitud aumenta respecto de la anterior, se desplaza `code` a la izquierda en la diferencia de longitudes.  Luego se convierte a binario con la nueva longitud.

De esta manera, los códigos obtenidos son **lexicográficamente ordenados** según `(longitud, símbolo)` y solo dependen de las longitudes, no de la forma concreta del árbol.  Además, el decodificador puede reconstruir el árbol canónico únicamente conociendo las longitudes y los símbolos.


## Decisiones de diseño

* **Modelado de datos:** Cada nodo del árbol de Huffman contiene el carácter (solo en hojas), la frecuencia acumulada de su subárbol y punteros a los hijos izquierdo y derecho.  También almacena el carácter más pequeño de su subárbol (`min_char`) para desempatar correctamente.
* **Criterio de desempate:** La cola de prioridad (`priority_queue`) utiliza un comparador que ordena primero por frecuencia ascendente (min-heap) y, en caso de empate, por el **mínimo carácter** del subárbol.  Las hojas se insertan en orden ASCII ascendente, de modo que los empates se resuelven estrictamente por el valor del símbolo.  Esto respeta el requerimiento de “desempatar por símbolo” y garantiza reproducibilidad.
* **Generación de códigos canónicos:** Los símbolos se ordenan por `(longitud, símbolo)`.  Se asigna un entero inicial igual a 0 al primer símbolo y luego se incrementa y desplaza según corresponda para los siguientes.  La conversión a cadena binaria se realiza con relleno de ceros a la izquierda.
* **Manejo de casos borde:** Si el texto tiene menos de 30 caracteres o contiene caracteres fuera de `[32,126]`, se muestra un error y se termina.  Si hay un único símbolo distinto, se asigna código `0` de longitud 1.  Se eliminan retornos de carro (`\r`) al leer en Windows.
* **Visualización:** El árbol se imprime en preorden con paréntesis; el espacio se muestra como `<sp>` para que sea visible.  Se muestran las tablas de frecuencias, longitudes y códigos, así como un resumen de los primeros 128 bits de la salida comprimida y su representación hexadecimal.

### Resultados

### Case 1 – Muchos espacios
Example: `This    is    a    text    with    many      spaces      included.` 
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| <sp> | 32 | 32 |
| . | 46 | 1 |
| T | 84 | 1 |
| a | 97 | 3 |
| c | 99 | 2 |
| d | 100 | 2 |
| e | 101 | 3 |
| h | 104 | 2 |
| i | 105 | 4 |
| l | 108 | 1 |
| m | 109 | 1 |
| n | 110 | 2 |
| p | 112 | 1 |
| s | 115 | 4 |
| t | 116 | 3 |
| u | 117 | 1 |
| w | 119 | 1 |
| x | 120 | 1 |
| y | 121 | 1 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| <sp> | 32 | 1 | 0 | 0 |
| i | 4 | 4 | 1011 | 1000 |
| s | 4 | 4 | 1101 | 1001 |
| t | 3 | 4 | 1000 | 1010 |
| a | 3 | 5 | 11110 | 10110 |
| c | 2 | 5 | 10010 | 10111 |
| d | 2 | 5 | 10011 | 11000 |
| e | 3 | 5 | 11111 | 11001 |
| h | 2 | 5 | 10100 | 11010 |
| n | 2 | 5 | 11000 | 11011 |
| l | 1 | 6 | 101010 | 111000 |
| m | 1 | 6 | 101011 | 111001 |
| p | 1 | 6 | 110010 | 111010 |
| u | 1 | 6 | 110011 | 111011 |
| w | 1 | 6 | 111000 | 111100 |
| x | 1 | 6 | 111001 | 111101 |
| y | 1 | 6 | 111010 | 111110 |
| . | 1 | 7 | 1110110 | 1111110 |
| T | 1 | 7 | 1110111 | 1111111 |

**Compression Summary:**
```
Input length (characters): 66
Original size (bits): 528
Compressed size (bits): 202
Compression ratio: 0.3826
Reduction: 61.7424%

```

**Compressed output (first 128 bits):**
```
11111111101010001001000010001001000010110000010101100111110110100000111100100010101101000001110011011011011111110000000100111101...
Total compressed bits: 202

```

**Compressed output (hex):**
```
FFA890890B0567DA0F22B41CDB7F013D5AF9902377E3BC671F8
```

**Huffman tree (preorder with parentheses):**
(<sp>(((t(cd))((h(lm))i))(((n(pu))s)(((wx)(y(.T)))(ae)))))


### Caso 2 – Pocos símbolos distintos
Texto de ejemplo: `AAAABBBCCCDDDAAAABBBCCCDDDAAAABBBCCCDDDAAAABBBCCCDDD`
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| A | 65 | 16 |
| B | 66 | 12 |
| C | 67 | 12 |
| D | 68 | 12 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| A | 16 | 2 | 11 | 00 |
| B | 12 | 2 | 00 | 01 |
| C | 12 | 2 | 01 | 10 |
| D | 12 | 2 | 10 | 11 |

**Compression Summary:**
```
Input length (characters): 52
Original size (bits): 416
Compressed size (bits): 104
Compression ratio: 0.2500
Reduction: 75.0000%

```

**Compressed output (first 128 bits):**
```
00000000010101101010111111000000000101011010101111110000000001010110101011111100000000010101101010111111
Total compressed bits: 104

```

**Compressed output (hex):**
```
0056AFC015ABF0056AFC015ABF
```

**Huffman tree (preorder with parentheses):**
((BC)(DA))

### Caso 3 – Distribución aproximadamente uniforme
Texto de ejemplo: `abcdefghijklmnopqrstuvwxyz0123456789!@#$%^`
**Frequencies:**

Frequency table (sorted by symbol):
| Symbol | ASCII | Freq |
| --- | --- | --- |
| Symbol | ASCII | Freq |
| ! | 33 | 1 |
| # | 35 | 1 |
| $ | 36 | 1 |
| % | 37 | 1 |
| 0 | 48 | 1 |
| 1 | 49 | 1 |
| 2 | 50 | 1 |
| 3 | 51 | 1 |
| 4 | 52 | 1 |
| 5 | 53 | 1 |
| 6 | 54 | 1 |
| 7 | 55 | 1 |
| 8 | 56 | 1 |
| 9 | 57 | 1 |
| @ | 64 | 1 |
| ^ | 94 | 1 |
| a | 97 | 1 |
| b | 98 | 1 |
| c | 99 | 1 |
| d | 100 | 1 |
| e | 101 | 1 |
| f | 102 | 1 |
| g | 103 | 1 |
| h | 104 | 1 |
| i | 105 | 1 |
| j | 106 | 1 |
| k | 107 | 1 |
| l | 108 | 1 |
| m | 109 | 1 |
| n | 110 | 1 |
| o | 111 | 1 |
| p | 112 | 1 |
| q | 113 | 1 |
| r | 114 | 1 |
| s | 115 | 1 |
| t | 116 | 1 |
| u | 117 | 1 |
| v | 118 | 1 |
| w | 119 | 1 |
| x | 120 | 1 |
| y | 121 | 1 |
| z | 122 | 1 |

**Codes (Tree & Canonical):**

Symbol details (sorted by code length then symbol):
| Symbol | Freq | Length | TreeCode | Canonical |
| --- | --- | --- | --- | --- |
| Symbol | Freq | Length | TreeCode | Canonical |
| e | 1 | 5 | 00000 | 00000 |
| f | 1 | 5 | 00001 | 00001 |
| g | 1 | 5 | 00010 | 00010 |
| h | 1 | 5 | 00011 | 00011 |
| i | 1 | 5 | 00100 | 00100 |
| j | 1 | 5 | 00101 | 00101 |
| k | 1 | 5 | 00110 | 00110 |
| l | 1 | 5 | 00111 | 00111 |
| m | 1 | 5 | 01000 | 01000 |
| n | 1 | 5 | 01001 | 01001 |
| o | 1 | 5 | 01010 | 01010 |
| p | 1 | 5 | 01011 | 01011 |
| q | 1 | 5 | 01100 | 01100 |
| r | 1 | 5 | 01101 | 01101 |
| s | 1 | 5 | 01110 | 01110 |
| t | 1 | 5 | 01111 | 01111 |
| u | 1 | 5 | 10000 | 10000 |
| v | 1 | 5 | 10001 | 10001 |
| w | 1 | 5 | 10010 | 10010 |
| x | 1 | 5 | 10011 | 10011 |
| y | 1 | 5 | 10100 | 10100 |
| z | 1 | 5 | 10101 | 10101 |
| ! | 1 | 6 | 101100 | 101100 |
| # | 1 | 6 | 101101 | 101101 |
| $ | 1 | 6 | 101110 | 101110 |
| % | 1 | 6 | 101111 | 101111 |
| 0 | 1 | 6 | 110000 | 110000 |
| 1 | 1 | 6 | 110001 | 110001 |
| 2 | 1 | 6 | 110010 | 110010 |
| 3 | 1 | 6 | 110011 | 110011 |
| 4 | 1 | 6 | 110100 | 110100 |
| 5 | 1 | 6 | 110101 | 110101 |
| 6 | 1 | 6 | 110110 | 110110 |
| 7 | 1 | 6 | 110111 | 110111 |
| 8 | 1 | 6 | 111000 | 111000 |
| 9 | 1 | 6 | 111001 | 111001 |
| @ | 1 | 6 | 111010 | 111010 |
| ^ | 1 | 6 | 111011 | 111011 |
| a | 1 | 6 | 111100 | 111100 |
| b | 1 | 6 | 111101 | 111101 |
| c | 1 | 6 | 111110 | 111110 |
| d | 1 | 6 | 111111 | 111111 |

**Compression Summary:**
```
Input length (characters): 42
Original size (bits): 336
Compressed size (bits): 230
Compression ratio: 0.6845
Reduction: 31.5476%

```

**Compressed output (first 128 bits):**
```
11110011110111111011111100000000010001000011001000010100110001110100001001010100101101100011010111001111100001000110010100111010...
Total compressed bits: 230

```

**Compressed output (hex):**
```
F3DFBF00443214C74254B635CF84653A570C72CF4D76DF8E6CEADBAFEC
```

**Huffman tree (preorder with parentheses):**
(((((ef)(gh))((ij)(kl)))(((mn)(op))((qr)(st))))((((uv)(wx))((yz)((!#)($%))))((((01)(23))((45)(67)))(((89)(@^))((ab)(cd))))))


### 📹 Video de Sustentación

Agrega aquí el enlace:  
➡️ [Video de YouTube](https://youtu.be/XXXXXXXXXXX)

---

### 🤖 Agradecimientos

Este proyecto fue desarrollado con apoyo de **GitHub Copilot** y **CLion AI Assistant**, utilizados para **corrección**, **optimización** y **claridad de comentarios**.  
Todas las decisiones finales de diseño y lógica fueron realizadas por el equipo de desarrollo.
