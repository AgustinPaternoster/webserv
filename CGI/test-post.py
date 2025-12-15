#!/usr/bin/env python3

import sys
import os
import json

# Definición de la secuencia de nueva línea estándar de HTTP/CGI
CRLF = "\r\n"

# --- 1. Inicialización y Lectura de la Solicitud ---
response_data = {}
post_data = None
content_length_str = os.environ.get('CONTENT_LENGTH', '0')
content_length = 0

try:
    content_length = int(content_length_str)
    
    if content_length > 0:
        # Leer el número exacto de bytes del cuerpo POST de STDIN
        post_data = sys.stdin.read(content_length)
        
        # Intentar analizar (parsear) el JSON
        data_recibida = json.loads(post_data)

        # --- PREPARAR RESPUESTA EXITOSA ---
        response_data = {
            "status": "success",
            "message": "Cuerpo POST recibido, analizado y devuelto con éxito.",
            "content_length_read": content_length,
            "datos_enviados_por_el_cliente": data_recibida  # <-- PRUEBA
        }
    else:
        # --- PREPARAR RESPUESTA SIN DATOS ---
        response_data = {
            "status": "warning",
            "message": "Solicitud POST recibida, pero el Content-Length era 0."
        }

except json.JSONDecodeError as e:
    # --- MANEJO DE ERROR DE FORMATO JSON ---
    response_data = {
        "status": "error_json",
        "message": f"Error al analizar el JSON: Formato no válido. {e}",
        "raw_data_received": post_data
    }
    
except Exception as e:
    # --- MANEJO DE OTROS ERRORES ---
    response_data = {
        "status": "fatal_error",
        "message": f"Ocurrió un error inesperado: {e}",
        "content_length": content_length_str
    }

# --- 2. Preparar el Cuerpo JSON para la Respuesta ---
# El cuerpo siempre se genera como una cadena JSON
json_body = json.dumps(response_data, indent=4)

# --- 3. Imprimir las Cabeceras CGI y el Cuerpo (Usando CRLF) ---

# Usamos sys.stdout.write() para tener control explícito sobre \r\n

# 3.1. Cabeceras Obligatorias (terminadas en CRLF)
sys.stdout.write(f"Content-type: application/json{CRLF}")
sys.stdout.write(f"Status: 200 OK{CRLF}")
sys.stdout.write(f"Content-Length: {len(json_body)}{CRLF}") # Recomendado: añadir Content-Length de la respuesta

# 3.2. Separador OBLIGATORIO: La línea vacía (\r\n\r\n)
# Esto indica al servidor Webserv que las cabeceras han terminado.
sys.stdout.write(CRLF) 

# 3.3. Cuerpo de la Respuesta (el JSON con los datos de verificación)
sys.stdout.write(json_body) 

# Aseguramos que la salida se envíe inmediatamente al pipe del servidor
sys.stdout.flush()

# El script termina.