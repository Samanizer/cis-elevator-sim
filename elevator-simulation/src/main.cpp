#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include "Building.h"
#include "GUI.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -e, --elevators <num>   Set number of elevators (1-5, default: 4)" << std::endl;
    std::cout << "  -f, --floors <num>      Set number of floors (2-12, default: 10)" << std::endl;
    std::cout << "  -h, --help              Display this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Default configuration - updated defaults
        int numFloors = 10;
        int numElevators = 4;
        
        // Parse command-line arguments
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--elevators") == 0) {
                if (i + 1 < argc) {
                    try {
                        numElevators = std::stoi(argv[++i]);
                        // Validate range - updated maximum
                        if (numElevators < 1 || numElevators > 5) {
                            std::cerr << "Warning: Number of elevators must be between 1 and 5. Using default (4)." << std::endl;
                            numElevators = 4;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing elevator count. Using default (4)." << std::endl;
                        numElevators = 4;
                    }
                }
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--floors") == 0) {
                if (i + 1 < argc) {
                    try {
                        numFloors = std::stoi(argv[++i]);
                        // Validate range - updated maximum
                        if (numFloors < 2 || numFloors > 12) {
                            std::cerr << "Warning: Number of floors must be between 2 and 12. Using default (10)." << std::endl;
                            numFloors = 10;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing floor count. Using default (10)." << std::endl;
                        numFloors = 10;
                    }
                }
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printUsage(argv[0]);
                return 0;
            } else {
                std::cerr << "Unknown option: " << argv[i] << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }
        
        std::cout << "Initializing elevator simulation with " << numElevators 
                  << " elevators and " << numFloors << " floors." << std::endl;
        
        // Window setup
        sf::RenderWindow window(sf::VideoMode(800, 720), "Elevator Simulation");
        window.setFramerateLimit(60);
        
        // Create building with configurable floors and elevators
        Building building(numFloors, numElevators);
        
        // Create and initialize GUI
        GUI gui(window, building);
        if (!gui.initialize()) {
            std::cerr << "Failed to initialize GUI" << std::endl;
            return 1;
        }
        
        // Main simulation loop
        sf::Clock clock;
        while (window.isOpen()) {
            // Calculate delta time
            sf::Time deltaTime = clock.restart();
            
            // Handle events
            sf::Event event;
            while (window.pollEvent(event)) {
                gui.handleEvent(event);
            }
            
            // Update simulation
            building.update();
            
            // Update GUI
            gui.update(deltaTime.asSeconds());
            
            // Render
            gui.render();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}