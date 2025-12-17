bool RequestParser::parseHeader(const std::string& line)
{
    // 1. Manejo de líneas vacías o solo con CR
    if (line.empty() || line == "\r")
        return true;

    size_t pos = line.find(':');
    if (pos == std::string::npos)
    {
        _error = "No ':' found in header line";
        return false;
    }

    // 2. Extraer Name y Value
    std::string name = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    // 3. Limpieza (CRÍTICO: Asegurarse de quitar \r)
    // Asumo que tu trim quita espacios, pero por seguridad quitamos CR manual
    if (!name.empty() && name[name.size() - 1] == '\r') name.erase(name.size() - 1);
    if (!value.empty() && value[value.size() - 1] == '\r') value.erase(value.size() - 1);
    
    name = trim(name);
    value = trim(value);

    // 4. Validaciones RFC 7230
    if (name.empty())
    {
        _error = "Empty header name";
        return false;
    }
    
    // Espacios en el nombre NO permitidos antes de los dos puntos
    if (name.find(' ') != std::string::npos) 
    {
        _error = "Header name cannot contain spaces";
        return false;
    }

    if (!isValidToken(name))
    {
        _error = "Invalid header name: " + name;
        return false;
    }

    // --- AQUÍ EVITAS EL ERROR 'Invalid header value char' ---
    // Debes implementar esta función pequeña o poner el bucle aquí
    if (!isValidHeaderValue(value)) 
    {
        _error = "Invalid character in header value";
        return false;
    }

    // Guardamos el header
    _request.getHeaders().set_http(name, value);

    // 5. Lógica específica (Case Insensitive)
    std::string lName = toLower(name);
    
    if (lName == "content-length")
    {
        // ARREGLO strtoull: Usar end_ptr, no try-catch
        char *end_ptr = NULL;
        errno = 0; // Resetear errno
        unsigned long long len = std::strtoull(value.c_str(), &end_ptr, 10);
        
        // Si end_ptr no llegó al final (\0) o hubo overflow (ERANGE)
        if (*end_ptr != '\0' || errno == ERANGE)
        {
            _error = "Invalid Content-Length value: " + value;
            return false;
        }
        _contentLen = len;
    }
    else if (lName == "host")
    {
        if (_request.getVersion() == "HTTP/1.1" && value.empty())
        {
            _error = "Empty Host header in HTTP/1.1 request";
            return false;
        }
        // Opcional: Validar duplicados de Host (Bad Request si hay 2 Host)
    }

    return true;
}