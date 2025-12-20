#include "Common.hpp"
#include <iostream>

void print_banner() {
    // Colors
    const char* CYAN  = "\033[96m";
    const char* BLUE  = "\033[94m";
    const char* RESET = "\033[0m";
    const char* BOLD  = "\033[1m";

    std::cout << CYAN << BOLD;
    std::cout << " __          __  ______   ____     _____  ______  _____ __      __ ______  _____" << std::endl;
    std::cout << " \\ \\        / / |  ____| |  _ \\   / ____||  ____||  __ \\\\ \\    / /|  ____||  __ \\" << std::endl;
    std::cout << "  \\ \\  /\\  / /  | |__    | |_) | | (___  | |__   | |__) |\\ \\  / / | |__   | |__) |" << std::endl;
    std::cout << "   \\ \\/  \\/ /   |  __|   |  _ <   \\___ \\ |  __|  |  _  /  \\ \\/ /  |  __|  |  _  /" << std::endl;
    std::cout << "    \\  /\\  /    | |____  | |_) |  ____) || |____ | | \\ \\   \\  /   | |____ | | \\ \\" << std::endl;
    std::cout << "     \\/  \\/     |______| |____/  |_____/ |______||_|  \\_\\   \\/    |______||_|  \\_\\" << std::endl;
    
    std::cout << BLUE << " --------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << BOLD << "  HTTP Server v1.0 | Status: " << "\033[92m" << "Online" << RESET << std::endl;
    std::cout << BLUE << " --------------------------------------------------------------------------------" << RESET << std::endl;
}
