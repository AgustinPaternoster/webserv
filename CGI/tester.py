#!/usr/bin/env python3
# No importes sys ni os si no los usas para lógica compleja
print("Content-Type: text/plain\r\n\r\n", end='')
print("Hola Mundo desde el CGI", end='')
# Elimina sys.stdout.flush() -> Deja que el exit del proceso lo haga