#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include "Building.h"
#include "Elevator.h"
#include "Request.h"

class GUI {
public:
    GUI(sf::RenderWindow& window, Building& building);
    ~GUI();
    
    // Initialization methods
    bool initialize();
    
    // Event handling
    bool handleEvent(sf::Event& event);
    
    // Update and render
    void update(float deltaTime);
    void render();
    
    // Resize methods
    void setScaleFactor(float factor);
    float getScaleFactor() const { return scaleFactor; }
    
    // Adjust the UI based on building dimensions
    void adjustToBuildingSize();
    
private:
    // Reference to window and building
    sf::RenderWindow& window;
    Building& building;
    
    // UI scaling factor
    float scaleFactor;
    
    // Font and visual elements
    sf::Font font;
    sf::Text instructionsText;
    sf::Text scaleFactorText;
    
    // Rendering constants (base values before scaling)
    int baseFloorHeight;
    int baseBuildingWidth;
    int baseElevatorWidth;
    int baseBuildingLeft;
    int baseBuildingBottom;
    
    // Scaled rendering values
    int floorHeight;
    int buildingWidth;
    int elevatorWidth;
    int buildingLeft;
    int buildingBottom;
    
    // Audio elements
    sf::SoundBuffer doorOpenBuffer;
    sf::SoundBuffer doorCloseBuffer;
    sf::SoundBuffer elevatorDingBuffer;
    sf::Sound doorOpenSound;
    sf::Sound doorCloseSound;
    sf::Sound elevatorDingSound;
    bool soundsLoaded;
    std::vector<bool> prevDoorsOpen;
    
    // Input mode handling
    bool inputMode;
    int selectedElevator;
    std::string inputText;
    sf::Text inputPrompt;
    sf::RectangleShape inputBox;
    sf::Clock inputBlinkClock;
    
    // Floor buttons for calling elevators
    std::map<int, sf::FloatRect> floorButtons;
    
    // Helper methods
    void handleElevatorClick(int mouseX, int mouseY);
    void handleFloorButtonClick(int mouseX, int mouseY);
    void onWindowResize(unsigned int width, unsigned int height);
    void loadSounds();
    void playSounds();
    void drawBuilding();
    void drawFloors();
    void drawElevators();
    void drawInputUI();
    void updateScaledDimensions();
    bool checkElevatorAtFloor(int floor);
    void openDoorsForElevatorsAtFloor(int floor);
};

#endif // GUI_H
