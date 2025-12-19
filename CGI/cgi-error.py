#!/usr/bin/python3
import sys
import time
import os

# Obtenemos el parámetro 'error' de la Query String (ej: ?error=loop)
query_string = os.environ.get("QUERY_STRING", "")

print("Content-Type: text/html\r\n\r\n")

if "error=loop" in query_string:
    # 1. BUCLE INFINITO (Prueba de Timeout y CPU)
    print("<h1>Iniciando bucle infinito...</h1>")
    while True:
        pass

elif "error=crash" in query_string:
    # 2. ERROR DE EJECUCIÓN (Prueba de 500 Internal Server Error)
    # Forzamos una división por cero
    x = 1 / 0

elif "error=bad_header" in query_string:
    # 3. CABECERAS INVÁLIDAS (Prueba de formato)
    # Nota: sys.exit() termina el script sin enviar la línea en blanco \r\n\r\n
    sys.stderr.write("Error enviado a stderr\n")
    sys.exit(1)

else:
    # COMPORTAMIENTO NORMAL
    print("<h1>CGI funcionando correctamente</h1>")
    print(f"<p>Query String recibida: {query_string}</p>")