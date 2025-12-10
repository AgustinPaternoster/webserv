#include <iostream>
#include <map>
#include <string>

// ---------------------------------------------------------------------
// Paso 1: Implementar la búsqueda del prefijo más largo (la lógica correcta)
// ---------------------------------------------------------------------

// Nota: Esta función es privada o auxiliar en tu clase Config
const t_location* Config::_findMostSpecificLocation(
    const std::string& requested_location, 
    const t_server& server_config
) {
    const t_location* best_match_location = NULL;
    size_t longest_match_len = 0;
    
    // Iteramos sobre todas las locations de este servidor
    for (std::vector<t_location>::const_iterator it = server_config.locations.begin(); 
         it != server_config.locations.end(); 
         ++it) {
        
        const t_location& current_loc = *it;
        const std::string& config_path = current_loc.path;
        size_t config_len = config_path.length();
        
        // Criterio de Coincidencia de Prefijo
        if (requested_location.length() >= config_len && 
            requested_location.substr(0, config_len) == config_path) {
            
            // Criterio de Coincidencia Más Larga
            if (config_len > longest_match_len) {
                longest_match_len = config_len;
                best_match_location = &current_loc; // Guardamos el puntero a la mejor coincidencia
            }
        }
    }
    
    // Devolvemos el puntero a la estructura t_location más específica
    return best_match_location; 
}