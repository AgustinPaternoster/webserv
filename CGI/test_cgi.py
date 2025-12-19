#!/usr/bin/env python3

import os
import sys
import time


# Definición de la secuencia de nueva línea estándar de HTTP/CGI
CRLF = "\r\n"

# --- 1. Obtener la información de la solicitud ---

method = os.environ.get("REQUEST_METHOD", "N/A")
query_string = os.environ.get("QUERY_STRING", "N/A")
content_length_str = os.environ.get("CONTENT_LENGTH", "0")
content_type = os.environ.get("CONTENT_TYPE", "N/A")

# Intentar leer el cuerpo de la solicitud (si es POST)
post_data = ""
try:
    content_length = int(content_length_str)
    if content_length > 0:
        # sys.stdin.read(length) lee exactamente esa cantidad de bytes del pipe
        post_data = sys.stdin.read(content_length)
except Exception:
    post_data = "Error al leer el cuerpo de la solicitud."


# --- 2. Construir la Respuesta HTML ---

# Usamos f-string de triple comilla para manejar el HTML multilinea
html_body = f"""
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Webserv CGI Test</title>
    <style>
        body {{ font-family: sans-serif; background-color: #f4f4f9; padding: 20px; }}
        .container {{ background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }}
        h1 {{ color: #333; border-bottom: 2px solid #eee; padding-bottom: 10px; }}
        pre {{ background-color: #eee; padding: 10px; border-radius: 4px; overflow-x: auto; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>✅ Webserv CGI Operativo</h1>
        <p>¡Hola desde el script de Python! Tu servidor HTTP funciona correctamente con CGI.</p>

        <h2>Detalles de la Solicitud</h2>
        <p><strong>Método HTTP:</strong> <code>{method}</code></p>
        <p><strong>Query String:</strong> <code>{query_string}</code></p>
        <p><strong>Content-Type:</strong> <code>{content_type}</code></p>
        <p><strong>Content-Length:</strong> <code>{content_length_str}</code> bytes</p>

        <h2>Datos POST Recibidos (Input Pipe)</h2>
        <pre>{post_data or 'No se recibieron datos POST.'}</pre>

        <h2>Todas las Variables de Entorno (Debug)</h2>
        <pre>{chr(10).join(f"{key}: {value}" for key, value in os.environ.items())}</pre>
    </div>
</body>
</html>
"""

# --- 3. Imprimir las Cabeceras CGI y el Cuerpo (al stdout/Pipe de Salida) ---

# Usamos sys.stdout.write() para tener control explícito sobre los saltos de línea (\r\n)

# 1. Cabeceras
sys.stdout.write(f"Content-type: text/html{CRLF}")
sys.stdout.write(f"Status: 200 OK{CRLF}")

# 2. Separador OBLIGATORIO: La línea vacía \r\n
# Esta es la secuencia que tu código C++ está buscando (\r\n\r\n)
sys.stdout.write(CRLF) 

# 3. Cuerpo HTML
sys.stdout.write(html_body) 

# Aseguramos que la salida se envíe inmediatamente al pipe del servidor
sys.stdout.flush()