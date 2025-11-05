# HTTP ENGINE - GUÍA DE IMPLEMENTACIÓN COMPLETA

## 📚 DOCUMENTOS GENERADOS

### ✅ Teoría Fundamentales
1. **Guia-Practica-Primeras-Clases.md** → Primeras 4 clases básicas
2. **Teoria-Completa-HTTP-Engine.md** → Teoría detallada del protocolo
3. **IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt** → RequestParser, ResponseBuilder, HttpUtils

### ✅ Código Completo
4. **Test-Files-Complete.md** → test_parser.cpp, test_builder.cpp, test_utils.cpp

## 🚀 INSTRUCCIONES PASO A PASO PARA IMPLEMENTAR

### PASO 1: Crear Estructura de Directorios

```bash
# Crear estructura
mkdir -p webserv/{include,src,tests,obj,bin}
cd webserv

# Verificar estructura
tree
# webserv/
# ├── include/
# ├── src/
# ├── tests/
# ├── obj/
# └── bin/
```

### PASO 2: Copiar Headers (Include)

Los siguientes archivos .hpp van en `include/`:

1. **HttpHeaders.hpp** (header-only)
   - Está en: Guia-Practica-Primeras-Clases.md
   - Función: Gestión case-insensitive de headers

2. **HttpRequest.hpp** (header-only)
   - Está en: Guia-Practica-Primeras-Clases.md
   - Función: Representación de HTTP request

3. **HttpResponse.hpp** (header-only)
   - Está en: Guia-Practica-Primeras-Clases.md
   - Función: Representación de HTTP response

4. **HttpStatusCode.hpp** (header-only)
   - Está en: Guia-Practica-Primeras-Clases.md
   - Función: Códigos de estado HTTP

5. **RequestParser.hpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Función: Parser incremental

6. **ResponseBuilder.hpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Función: Constructor fluido

7. **HttpUtils.hpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Función: Utilidades HTTP

### PASO 3: Copiar Implementaciones (Src)

Los siguientes archivos .cpp van en `src/`:

1. **HttpStatusCode.cpp**
   - Está en: Guia-Practica-Primeras-Clases.md (dentro de la sección)
   - Nota: Implementación del mapeo de status codes

2. **RequestParser.cpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Nota: Es el más complejo (~500 líneas)

3. **ResponseBuilder.cpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Nota: Implementación del builder (~350 líneas)

4. **HttpUtils.cpp**
   - Está en: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt
   - Nota: Funciones auxiliares (~300 líneas)

### PASO 4: Copiar Tests (tests/)

Los siguientes archivos .cpp van en `tests/`:

1. **test_parser.cpp**
   - Está en: Test-Files-Complete.md
   - Tests: 8 tests para RequestParser

2. **test_builder.cpp**
   - Está en: Test-Files-Complete.md
   - Tests: 8 tests para ResponseBuilder

3. **test_utils.cpp**
   - Está en: Test-Files-Complete.md
   - Tests: 7 tests para HttpUtils

### PASO 5: Crear Makefile

Copia el Makefile de IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt a la raíz de webserv/

```makefile
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude
# ... resto del Makefile
```

### PASO 6: Compilar y Ejecutar Tests

```bash
# Compilar todo
make

# Ejecutar todos los tests
make test

# O ejecutar tests individuales
./bin/test_parser
./bin/test_builder
./bin/test_utils
```

## 📋 CHECKLIST DE IMPLEMENTACIÓN

### Phase 1: Clases Básicas (Día 1)
- [ ] HttpHeaders.hpp → include/
- [ ] HttpRequest.hpp → include/
- [ ] HttpResponse.hpp → include/
- [ ] HttpStatusCode.hpp → include/
- [ ] HttpStatusCode.cpp → src/
- [ ] Compilar y verificar que no hay errores

### Phase 2: Clases Avanzadas (Día 2-3)
- [ ] RequestParser.hpp → include/
- [ ] RequestParser.cpp → src/
- [ ] ResponseBuilder.hpp → include/
- [ ] ResponseBuilder.cpp → src/
- [ ] HttpUtils.hpp → include/
- [ ] HttpUtils.cpp → src/

### Phase 3: Testing (Día 4)
- [ ] test_parser.cpp → tests/
- [ ] test_builder.cpp → tests/
- [ ] test_utils.cpp → tests/
- [ ] Makefile → webserv/
- [ ] Ejecutar: make test
- [ ] Verificar: ✓ ALL TESTS PASSED

## 🔍 VERIFICACIÓN FINAL

### Estructura Esperada

```
webserv/
├── include/
│   ├── HttpHeaders.hpp          ✓
│   ├── HttpRequest.hpp          ✓
│   ├── HttpResponse.hpp         ✓
│   ├── HttpStatusCode.hpp       ✓
│   ├── RequestParser.hpp        ✓
│   ├── ResponseBuilder.hpp      ✓
│   └── HttpUtils.hpp            ✓
├── src/
│   ├── HttpStatusCode.cpp       ✓
│   ├── RequestParser.cpp        ✓
│   ├── ResponseBuilder.cpp      ✓
│   └── HttpUtils.cpp            ✓
├── tests/
│   ├── test_parser.cpp          ✓
│   ├── test_builder.cpp         ✓
│   └── test_utils.cpp           ✓
├── obj/                         (generado)
├── bin/                         (generado)
└── Makefile                     ✓
```

### Tests Esperados

```
✓ TEST 1: Simple GET Request
✓ TEST 2: POST with JSON Body
✓ TEST 3: Chunked Data Streaming
✓ TEST 4: Invalid Request Detection
✓ TEST 5: Multiple Headers
✓ TEST 6: PUT Request
✓ TEST 7: DELETE Request
✓ TEST 8: Case-Insensitive Headers

✓ TEST 1: Basic 200 OK Response
✓ TEST 2: JSON Response
✓ TEST 3: 404 Not Found
✓ TEST 4: 500 Internal Server Error
✓ TEST 5: Custom Headers
✓ TEST 6: Convenience Methods
✓ TEST 7: Fluent Interface
✓ TEST 8: Automatic Headers

✓ TEST 1: Trim
✓ TEST 2: toLower and toUpper
✓ TEST 3: Valid HTTP Methods
✓ TEST 4: MIME Type Detection
✓ TEST 5: URL Encoding
✓ TEST 6: URL Decoding
✓ TEST 7: HTTP Date Format
```

## 💡 TIPS DE IMPLEMENTACIÓN

### Ordenar Correctamente

✓ CORRECTO:
1. HttpHeaders (depende solo de STL)
2. HttpRequest (usa HttpHeaders)
3. HttpResponse (usa HttpHeaders)
4. HttpStatusCode (depende solo de STL)
5. RequestParser (usa HttpRequest, HttpHeaders)
6. ResponseBuilder (usa HttpResponse, HttpStatusCode)
7. HttpUtils (independiente)

### Compilación por Fases

```bash
# Fase 1: Solo headers (sin .cpp)
c++ -Wall -Wextra -std=c++98 -c include/HttpStatusCode.hpp

# Fase 2: Con primeros .cpp
c++ -Wall -Wextra -std=c++98 -Iinclude -c src/HttpStatusCode.cpp

# Fase 3: Agregar RequestParser
c++ -Wall -Wextra -std=c++98 -Iinclude -c src/RequestParser.cpp

# Fase 4: Agregar ResponseBuilder
c++ -Wall -Wextra -std=c++98 -Iinclude -c src/ResponseBuilder.cpp

# Fase 5: Agregar HttpUtils
c++ -Wall -Wextra -std=c++98 -Iinclude -c src/HttpUtils.cpp
```

### Debugging

Si algo falla, verificar:

```bash
# 1. Errores de compilación
g++ -Wall -Wextra -std=c++98 -Iinclude src/*.cpp tests/test_parser.cpp -o test 2>&1

# 2. Memory leaks
valgrind --leak-check=full ./test

# 3. Segmentation faults
gdb ./test

# 4. Warnings adicionales
g++ -Wall -Wextra -Wconversion -std=c++98 -Iinclude src/*.cpp
```

## 📚 DOCUMENTOS DE REFERENCIA

### Para Entender HTTP
- **Teoria-Completa-HTTP-Engine.md** → Todo sobre el protocolo HTTP

### Para Entender las Clases Básicas
- **Guia-Practica-Primeras-Clases.md** → HttpHeaders, HttpRequest, HttpResponse, HttpStatusCode

### Para Entender las Clases Avanzadas
- **IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt** → RequestParser, ResponseBuilder, HttpUtils

### Para Entender los Tests
- **Test-Files-Complete.md** → test_parser.cpp, test_builder.cpp, test_utils.cpp

## 🎯 OBJETIVOS POR DÍA

### Día 1: Primeras 4 Clases Básicas
- Estudiar: Guia-Practica-Primeras-Clases.md (secciones 1-4)
- Implementar: HttpHeaders, HttpRequest, HttpResponse, HttpStatusCode
- Verificar: Compilar sin errores
- Tiempo estimado: 4-6 horas

### Día 2: State Machine y Parser
- Estudiar: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt (Parte 1)
- Implementar: RequestParser.hpp + RequestParser.cpp
- Verificar: Compilar, tests/test_parser.cpp
- Tiempo estimado: 4-6 horas

### Día 3: Builder Pattern y Response
- Estudiar: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt (Parte 2)
- Implementar: ResponseBuilder.hpp + ResponseBuilder.cpp
- Verificar: Compilar, tests/test_builder.cpp
- Tiempo estimado: 3-4 horas

### Día 4: Utilidades y Integración
- Estudiar: IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt (Parte 3)
- Implementar: HttpUtils.hpp + HttpUtils.cpp
- Integración: Crear Makefile
- Tests: Ejecutar make test
- Tiempo estimado: 3-4 horas

### Día 5: Integración Completa y Validación
- Ejecutar: Integración completa (ejemplo en IMPLEMENTACION_COMPLETA_HTTP_ENGINE.txt)
- Testing: Todos los tests deben pasar
- Documentación: Crear README.md
- Tiempo estimado: 2-3 horas

## 🚀 PRÓXIMOS PASOS DESPUÉS DE HTTP ENGINE

Una vez completado el HTTP Engine, puedes:

1. **Socket Programming**: Conectar con servidor TCP/IP
2. **Server Loop**: Implementar servidor que acepta conexiones
3. **Request Handling**: Procesar requests en el servidor
4. **Router**: Mapear URLs a handlers
5. **Static Files**: Servir archivos estáticos
6. **CGI/FastCGI**: Ejecutar scripts
7. **Logging**: Sistema de logs
8. **Performance**: Optimizaciones

## 📞 TROUBLESHOOTING

### Error: "Undefined reference to HttpStatusCode::..."
Solución: Asegúrate de compilar HttpStatusCode.cpp

### Error: "std::stoi not found"
Solución: Asegúrate de usar C++11 o cambiar a std::atoi para C++98

### Error: "No such file or directory"
Solución: Verifica que la estructura de directorios es correcta

### Warning: "deprecated conversion"
Solución: Normal en C++98, ignorar o usar -Wno-deprecated-conversions

═══════════════════════════════════════════════════════════════════════════════

¡YA TIENES TODO LO QUE NECESITAS PARA IMPLEMENTAR HTTP ENGINE COMPLETO!

Sigue esta guía paso a paso y tendrás un servidor HTTP funcional en una semana.

═══════════════════════════════════════════════════════════════════════════════