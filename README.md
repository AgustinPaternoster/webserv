# Webserv - C++98 HTTP Server

![C++](https://img.shields.io/badge/Language-C++98-blue)
![Webserv](https://img.shields.io/badge/42-Webserv-success)
![License](https://img.shields.io/badge/License-MIT-green)

## 📖 Descripción

**Webserv** es un servidor HTTP totalmente funcional desarrollado en **C++98** como parte del currículo de 42. Este proyecto tiene como objetivo comprender los entresijos del protocolo HTTP/1.1 y la gestión de redes a bajo nivel.

El servidor está diseñado para ser **no bloqueante** y robusto, utilizando **I/O multiplexing** (`poll`, `epoll` o `kqueue`) para gestionar múltiples conexiones simultáneas en un solo hilo de ejecución.

## ✨ Características (Features)

Cumpliendo con los requisitos del *Subject*:

### 🚀 Core
* **Non-blocking I/O:** Todas las operaciones de red (sockets, pipes) se gestionan a través de un único bucle de eventos (`poll()` o equivalente) para asegurar que el servidor nunca se bloquee.
* **Compatibilidad:** Cumple con el estándar C++98 y compila sin errores con las flags `-Wall -Wextra -Werror -std=c++98`.
* **Gestión de Clientes:** Soporta múltiples clientes simultáneos sin colgarse.

### 🌐 HTTP
* **Métodos Soportados:** Implementación de `GET`, `POST` y `DELETE`.
* **Archivos Estáticos:** Sirve páginas web, imágenes y recursos estáticos.
* **Listado de Directorios:** Si no hay archivo índice, puede generar un listado de archivos del directorio (Autoindex).
* **Subida de Archivos:** Permite a los clientes subir archivos al servidor (Multipart/form-data).
* **Páginas de Error:** Sirve páginas de error personalizadas (404, 500, etc.) o por defecto.

### ⚙️ Configuración y CGI
* **Archivo de Configuración:** Parser inspirado en NGINX que permite configurar puertos, rutas, límites y hosts.
* **Ejecución CGI:** Soporte para ejecutar scripts dinámicos (PHP, Python, etc.) pasando las variables de entorno correctas.
* **Multi-Puerto:** Capacidad de escuchar en múltiples puertos e interfaces simultáneamente.

## 🛠️ Requisitos e Instalación

### Prerrequisitos
* Entorno Unix (Linux o macOS).
* Compilador C++ (g++ o clang++).
* `make`.

### Compilación
Clona el repositorio y compila el proyecto usando `make`:

```bash
git clone [https://github.com/tu-usuario/webserv.git](https://github.com/tu-usuario/webserv.git)
cd webserv
make
```
 
### Uso
Para iniciar el servidor, proporciona la ruta a un archivo de configuración .conf o [file]
./webserv [path/to/config_file]

## 👥 Autores
Este proyecto fue desarrollado en equipo por:
* 	Agustin Paternoster [apaterno]
* 	Yery Alexander Rodriguez Gonzalez [yrodrigu]
* 	Carlos Murillo Maza [camurill]
