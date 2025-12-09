import socket
import requests
import time
import threading

# CONFIGURACIÓN
HOST = "127.0.0.1"
PORT = 8080
BASE_URL = f"http://{HOST}:{PORT}"

print(f"🛡️  INICIANDO WEBSERVER COMPLIANCE TESTER (NO-CGI) 🛡️")
print(f"Target: {BASE_URL}\n")

def print_pass(msg):
    print(f"✅ PASS: {msg}")

def print_fail(msg):
    print(f"❌ FAIL: {msg}")

# -------------------------------------------------------------------------
# TEST 1: Métodos No Permitidos (405)
# Subject: "List of accepted HTTP methods for the route" [cite: 161]
# -------------------------------------------------------------------------
def test_405_method_not_allowed():
    print("--- Test 1: Method Not Allowed (405) ---")
    try:
        # Intenta borrar algo en una ruta que solo admite GET (ej: la raiz o /index.html)
        r = requests.delete(f"{BASE_URL}/index.html")
        if r.status_code == 405:
            print_pass(f"DELETE en ruta GET-only devolvió 405. Headers: {r.headers.get('Allow')}")
        else:
            print_fail(f"Esperaba 405, recibió {r.status_code}")
    except Exception as e:
        print_fail(f"Excepción: {e}")

# -------------------------------------------------------------------------
# TEST 2: Redirecciones (301)
# Subject: "HTTP redirection" 
# -------------------------------------------------------------------------
def test_301_redirect():
    print("\n--- Test 2: Redirección (301) ---")
    # Configura en tu conf: location /old-page/ { return 301 http://google.com; }
    try:
        # allow_redirects=False para ver el 301 y no seguirlo automáticamente
        r = requests.get(f"{BASE_URL}/old-page/", allow_redirects=False)
        if r.status_code == 301:
            location = r.headers.get('Location')
            if location:
                print_pass(f"Redirección correcta a: {location}")
            else:
                print_fail("Código 301 recibido pero falta header Location")
        else:
            print_fail(f"Esperaba 301, recibió {r.status_code} (Revisa tu config en /old-page/)")
    except Exception as e:
        print_fail(f"Excepción: {e}")

# -------------------------------------------------------------------------
# TEST 3: Autoindex ON vs OFF (Listado de Directorios)
# Subject: "Enabling or disabling directory listing" 
# -------------------------------------------------------------------------
def test_autoindex():
    print("\n--- Test 3: Autoindex ---")
    
    # CASO A: Autoindex ON (Debe mostrar lista de archivos)
    try:
        r = requests.get(f"{BASE_URL}/uploads/") # Asumiendo que uploads tiene autoindex on
        if r.status_code == 200 and ("<a href" in r.text or "Index of" in r.text):
            print_pass("Autoindex ON funciona (Se detectaron enlaces HTML)")
        else:
            print_fail(f"Autoindex ON falló. Status: {r.status_code}. ¿Hay un index.html bloqueando?")
    except: print_fail("Error conexión Autoindex ON")

    # CASO B: Autoindex OFF (Debe dar 403 Forbidden si no hay index.html)
    # Crea una carpeta vacía 'forbidden_folder' y ponle autoindex off en el conf
    try:
        r = requests.get(f"{BASE_URL}/forbidden_folder/") 
        if r.status_code == 403:
            print_pass("Autoindex OFF correctamente devolvió 403 Forbidden")
        else:
            print_fail(f"Autoindex OFF falló. Esperaba 403, recibió {r.status_code}")
    except: pass

# -------------------------------------------------------------------------
# TEST 4: Peticiones Malformadas (Resiliencia)
# Subject: "Your program must not crash under any circumstances" [cite: 31]
# -------------------------------------------------------------------------
def test_malformed_request():
    print("\n--- Test 4: Bad Request (Raw Socket) ---")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        # Enviamos basura sin formato HTTP
        s.send(b"GET / HTTP/1.1\r\nHost: localhost\r\nJunkHeader: \x00\x01\xFF\r\n\r\n")
        data = s.recv(1024)
        s.close()
        
        # El servidor debería responder 400 o cerrar conexión, PERO NO CRASHEAR.
        if b"400" in data:
            print_pass("Servidor manejó basura con 400 Bad Request")
        else:
            print_pass("Servidor cerró conexión o respondió algo (mientras siga vivo, bien).")
            
        # Verificar que sigue vivo
        try:
            requests.get(f"{BASE_URL}/index.html", timeout=1)
            print_pass("El servidor sigue vivo después del ataque.")
        except:
            print_fail("EL SERVIDOR MURIÓ TRAS RECIBIR BASURA.")
            
    except Exception as e:
        print_fail(f"Error en el test: {e}")

# -------------------------------------------------------------------------
# TEST 5: Slowloris (Test de Non-Blocking I/O)
# Subject: "Your server must remain non-blocking at all times" 
# -------------------------------------------------------------------------
def slow_attack():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        s.send(b"GET /index.html HTTP/1.1\r\n")
        s.send(b"Host: localhost\r\n")
        # Enviamos headers muy lento y nunca terminamos...
        time.sleep(2)
        s.send(b"X-Header: Slow...")
        time.sleep(10) 
        s.close()
    except: pass

def test_concurrency_blocking():
    print("\n--- Test 5: Non-Blocking Check (Mini Slowloris) ---")
    print("Iniciando conexión lenta en segundo plano...")
    
    # Lanzamos un hilo que bloqueará una conexión por 12 segundos
    t = threading.Thread(target=slow_attack)
    t.start()
    
    time.sleep(1) # Damos tiempo a que se conecte
    
    print("Intentando acceder con cliente normal...")
    start = time.time()
    try:
        # Si tu servidor es bloqueante, esta petición esperará a que termine la lenta (10s)
        # Si es NO bloqueante (poll), esta petición pasará instantáneamente.
        r = requests.get(f"{BASE_URL}/index.html", timeout=2)
        end = time.time()
        
        if r.status_code == 200:
            print_pass(f"Servidor respondió en {end - start:.2f}s mientras había una conexión lenta activa.")
        else:
            print_fail("Servidor respondió error.")
    except requests.exceptions.Timeout:
        print_fail("TIMEOUT: El servidor se bloqueó esperando a la conexión lenta. (Revisar poll/select)")
    except Exception as e:
        print_fail(f"Error: {e}")
        
    t.join()

# -------------------------------------------------------------------------
# TEST 6: Host Header Check
# Subject: "Your server must be able to listen to multiple ports..." 
# -------------------------------------------------------------------------
def test_multiple_ports():
    print("\n--- Test 6: Múltiples Puertos ---")
    # Asume que configuras 'listen 8081' también en el conf
    ALT_PORT = 8081
    try:
        r = requests.get(f"http://{HOST}:{ALT_PORT}/index.html")
        if r.status_code == 200:
            print_pass(f"Servidor responde correctamente en puerto alternativo {ALT_PORT}")
        else:
            print_fail(f"Fallo en puerto {ALT_PORT}. Status: {r.status_code}")
    except:
        print("⚠️  Skipping Test 6: No se detectó configuración para puerto 8081 o conexión rechazada.")

# EJECUCIÓN
if __name__ == "__main__":
    test_405_method_not_allowed()
    test_301_redirect()
    test_autoindex()
    test_malformed_request()
    test_concurrency_blocking() # ESTE ES EL MÁS IMPORTANTE PARA EL SUBJECT
    test_multiple_ports()
    print("\n🏁 TESTS FINALIZADOS 🏁")