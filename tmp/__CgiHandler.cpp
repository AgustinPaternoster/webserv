#include "../include/Common.hpp"


std::map<std::string, std::string> setupCgiEnvironment(void)
{
    std::map <std::string, std::string> tmp;
    tmp["REQUEST_METHOD"] = "GET";
    tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
    tmp["QUERY_STRING"] = "";
    tmp["SERVER_PORT"] = "8080";
    tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
    tmp["CONTENT_LENGTH"] = "150";
    tmp["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
}

bool validateCgiResources(t_server server)
{
    std::string uri;
    std::string file = server.locations[0].root + uri.substr(uri.find_last_of('/'));
    if(accept(server.locations[0].root.c_str(), F_OK | X_OK))
        // error directorio no existe o no tienes los permisos
    if()
    
}


void CgiHandler(void)
{
    // configurar las variables de entorno

    // hacer comprobaciones si el directorio existe o si el archivo existe
    std::map<std::string, std::string> envVar = setupCgiEnvironment();
    std::string body;

    pid_t pid;
    int fd[2];
    if(pipe(fd) < 0)
        // error
    pid = fork();
    if(envVar["REQUEST_METHOD"] == "GET")
    {
        close(fd[0]);
        close(fd[1]);
    }
    if(envVar["REQUEST_METHOD"] == "POST")
    {
        close(fd[0]);
        write(fd[1], body.c_str() , body.size());
        close(fd[0]);
    }

    if(pid < 0)
        // error
    if (pid == 0)
    {
        // proceso hijo
    }


    // configurar la response
}

#include <iostream>
#include <map>
#include <string>
#include <algorithm> // Para std::transform y std::replace

// 1. Función para normalizar el nombre del header.
// Parámetros:
//   - headerName: El nombre original del header (ej. "User-Agent").
//   - isHttpHeader: Bandera para saber si debe llevar el prefijo "HTTP_".
std::string normalizeHeaderName(const std::string& headerName, bool isHttpHeader) {
    std::string normalizedName = headerName;
    
    // 1. Convertir a mayúsculas
    // En C++98, std::transform se usa con el iterador del principio, del final y del destino
    // junto con la función a aplicar (::toupper o std::toupper).
    std::transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), (int(*)(int))std::toupper);
    
    // 2. Reemplazar '-' con '_'
    std::replace(normalizedName.begin(), normalizedName.end(), '-', '_');
    
    // 3. Agregar el prefijo "HTTP_" si es un header estándar
    if (isHttpHeader) {
        return "HTTP_" + normalizedName;
    }
    
    return normalizedName;
}

// 2. Función principal de conversión.
// Transforma un mapa de headers HTTP en un mapa de variables de entorno CGI.
std::map<std::string, std::string> convertHeadersToCGIEnv(
    const std::map<std::string, std::string>& httpHeaders) {

    std::map<std::string, std::string> cgiEnvVars;

    // Iteración estilo C++98
    for (std::map<std::string, std::string>::const_iterator it = httpHeaders.begin(); 
         it != httpHeaders.end(); 
         ++it) {
        
        const std::string& name = it->first;
        const std::string& value = it->second;

        // Regla especial para Content-Type y Content-Length:
        // Se usan sin el prefijo "HTTP_", pero se normalizan a mayúsculas y guiones bajos.
        if (name == "Content-Type" || name == "Content-Length") {
            std::string cgiName = normalizeHeaderName(name, false);
            cgiEnvVars[cgiName] = value;
        } 
        // Regla general para el resto de los headers:
        // Se prefijan con "HTTP_" y se normalizan.
        else {
            std::string cgiName = normalizeHeaderName(name, true);
            cgiEnvVars[cgiName] = value;
        }
    }

    return cgiEnvVars;
}

// --- Ejemplo de Uso (Main) ---
int main() {
    // 1. Mapa de ejemplo de headers HTTP
    std::map<std::string, std::string> clientHeaders;
    clientHeaders["Host"] = "localhost:8080";
    clientHeaders["User-Agent"] = "Mozilla/5.0";
    clientHeaders["Accept-Language"] = "es-ES,es;q=0.9";
    clientHeaders["Content-Type"] = "application/json"; // Caso especial
    clientHeaders["Content-Length"] = "4096";          // Caso especial
    clientHeaders["If-None-Match"] = "abc-123";

    
    std::cout << "--- Headers HTTP de Entrada ---\n";
    for (std::map<std::string, std::string>::const_iterator it = clientHeaders.begin(); it != clientHeaders.end(); ++it) {
        std::cout << it->first << ": " << it->second << "\n";
    }

    // 2. Ejecutar la conversión
    std::map<std::string, std::string> cgiVariables = convertHeadersToCGIEnv(clientHeaders);

    // 3. Imprimir el resultado (Variables de Entorno CGI)
    std::cout << "\n--- Variables de Entorno CGI de Salida ---\n";
    // El orden de impresión del mapa puede variar, pero las conversiones son correctas.
    for (std::map<std::string, std::string>::const_iterator it = cgiVariables.begin(); it != cgiVariables.end(); ++it) {
        std::cout << "**" << it->first << "**" << " = " << it->second << "\n";
    }

    return 0;
}