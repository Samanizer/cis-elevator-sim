#include "GUI.h"
#include <iostream>
#include <sstream>

GUI::GUI(sf::RenderWindow& window, Building& building)
    : window(window), building(building),
      inputMode(false), selectedElevator(-1), soundsLoaded(true),
      scaleFactor(1.0f),
      baseFloorHeight(50), baseBuildingWidth(400), baseElevatorWidth(50),
      baseBuildingLeft(200), baseBuildingBottom(650) { // Moved building bottom lower
    // Initialize the previous door states vector
    prevDoorsOpen.resize(building.getNumElevators(), false);
    
    // Initialize scaled dimensions
    updateScaledDimensions();
}

GUI::~GUI() {
    // Destructor body, cleanup if needed
}

bool GUI::initialize() {
    // Load font
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Try a fallback font
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            std::cerr << "Could not load font" << std::endl;
            return false;
        }
    }
    
    // Load sounds
    loadSounds();
    
    // Adjust UI based on building dimensions
    adjustToBuildingSize();
    
    // Setup input UI elements
    inputPrompt.setFont(font);
    inputPrompt.setCharacterSize(24);
    inputPrompt.setFillColor(sf::Color::White);
    inputPrompt.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 70);
    
    inputBox.setSize(sf::Vector2f(400, 60));
    inputBox.setFillColor(sf::Color(50, 50, 50, 200));
    inputBox.setOutlineColor(sf::Color::White);
    inputBox.setOutlineThickness(3);
    inputBox.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 30);
    
    // Instructions text
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(20 * scaleFactor);
    instructionsText.setFillColor(sf::Color::Black);
    instructionsText.setPosition(50 * scaleFactor, window.getSize().y - 90 * scaleFactor);
    instructionsText.setString(
        "Press number keys or floor button to request an elevator to that floor\n" // + std::to_string(std::min(9, building.getNumFloors())) +
        //"Press Shift+1-" + std::to_string(std::min(9, building.getNumFloors())) + " or Elevator icon to request an elevator to that floor (DOWN)\n"
        //"Press F8-F" + std::to_string(std::min(8 + building.getNumElevators() - 1, 12)) + " to add internal requests to elevators\n"
        "Click on stationary elevators to set destination floors\n"
        "Press Escape to exit"
    );
    
    // Setup scale factor display
    scaleFactorText.setFont(font);
    scaleFactorText.setCharacterSize(18);
    scaleFactorText.setFillColor(sf::Color::Black);
    scaleFactorText.setPosition(10, 10);
    scaleFactorText.setString("Scale Factor: " + std::to_string(scaleFactor) + "x (Use '+' and '-' to adjust)");
    
    // Initialize the floor buttons map
    floorButtons.clear();
    
    return true;
}

void GUI::loadSounds() {
    if (!doorOpenBuffer.loadFromFile("/home/sam/Dev/cis-elevators/elevator-simulation/assets/sounds/door_open.wav")) {
        std::cerr << "Failed to load door_open.wav" << std::endl;
        soundsLoaded = false;
    }
    
    if (!doorCloseBuffer.loadFromFile("/home/sam/Dev/cis-elevators/elevator-simulation/assets/sounds/door_close.wav")) {
        std::cerr << "Failed to load door_close.wav" << std::endl;
        soundsLoaded = false;
    }
    
    if (!elevatorDingBuffer.loadFromFile("/home/sam/Dev/cis-elevators/elevator-simulation/assets/sounds/elevator_ding.wav")) {
        std::cerr << "Failed to load elevator_ding.wav" << std::endl;
        soundsLoaded = false;
    }
    
    doorOpenSound.setBuffer(doorOpenBuffer);
    doorCloseSound.setBuffer(doorCloseBuffer);
    elevatorDingSound.setBuffer(elevatorDingBuffer);
}

void GUI::adjustToBuildingSize() {
    // Adjust window size if needed based on number of floors and elevators
    int minWindowHeight = building.getNumFloors() * baseFloorHeight + 100; // Extra space for UI elements
    int minWindowWidth = baseBuildingLeft + baseBuildingWidth + 100; // Extra space
    
    // Update window size if needed
    sf::Vector2u currentSize = window.getSize();
    if (currentSize.y < minWindowHeight || currentSize.x < minWindowWidth) {
        window.setSize(sf::Vector2u(
            std::max(currentSize.x, (unsigned int)minWindowWidth),
            std::max(currentSize.y, (unsigned int)minWindowHeight)
        ));
    }
    
    // Reset base dimensions if needed
    if (building.getNumFloors() > 15) {
        // Reduce floor height for buildings with many floors
        baseFloorHeight = 800 / building.getNumFloors();
    }
    
    // Adjust building width based on elevator count
    if (building.getNumElevators() > 5) {
        // Increase building width for more elevators
        baseBuildingWidth = std::max(400, 80 * building.getNumElevators());
    }
    
    // Update scaled dimensions
    updateScaledDimensions();
}

void GUI::setScaleFactor(float factor) {
    // Limit scale factor to reasonable range (0.5 to 3.0)
    scaleFactor = std::max(0.5f, std::min(factor, 3.0f));
    
    // Update the scaled dimensions
    updateScaledDimensions();
    
    // Update the scale factor text
    scaleFactorText.setString("Scale Factor: " + std::to_string(scaleFactor) + "x (Use '+' and '-' to adjust)");
    
    // Resize the window to fit the new scale
    int newWidth = static_cast<int>(800 * scaleFactor);
    int newHeight = static_cast<int>(720 * scaleFactor);
    window.setSize(sf::Vector2u(newWidth, newHeight));
    
    // Update UI positions after scale change
    onWindowResize(newWidth, newHeight);
}

void GUI::updateScaledDimensions() {
    // Scale all rendering constants by the scale factor
    floorHeight = static_cast<int>(baseFloorHeight * scaleFactor);
    buildingWidth = static_cast<int>(baseBuildingWidth * scaleFactor);
    elevatorWidth = static_cast<int>(baseElevatorWidth * scaleFactor);
    buildingLeft = static_cast<int>(baseBuildingLeft * scaleFactor);
    buildingBottom = static_cast<int>(baseBuildingBottom * scaleFactor);
}

bool GUI::handleEvent(sf::Event& event) {
    // Handle window close event
    if (event.type == sf::Event::Closed) {
        window.close();
        return true;
    }
    
    // Handle window resize event
    if (event.type == sf::Event::Resized) {
        onWindowResize(event.size.width, event.size.height);
        return true;
    }
    
    // Handle mouse button presses
    if (event.type == sf::Event::MouseButtonPressed) {
        if (!inputMode && event.mouseButton.button == sf::Mouse::Left) {
            // Check for elevator clicks
            handleElevatorClick(event.mouseButton.x, event.mouseButton.y);
            
            // Check for floor button clicks
            handleFloorButtonClick(event.mouseButton.x, event.mouseButton.y);
            
            return true;
        }
    }
    
    // Handle keyboard events for scale factor adjustment
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add) {  // '+' key
            setScaleFactor(scaleFactor + 0.1f);
            return true;
        }
        else if (event.key.code == sf::Keyboard::Dash || event.key.code == sf::Keyboard::Subtract) {  // '-' key
            setScaleFactor(scaleFactor - 0.1f);
            return true;
        }
    }
    
    // Handle keyboard events based on current mode
    if (inputMode) {
        // In input mode: handle text input and special keys
        if (event.type == sf::Event::TextEntered) {
            // Only accept numeric input
            if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                inputText += static_cast<char>(event.text.unicode);
            }
            return true;
        } 
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Enter) {
                // Process the input when Enter is pressed
                if (!inputText.empty()) {
                    try {
                        int targetFloor = std::stoi(inputText);
                        
                        if (targetFloor >= 1 && targetFloor <= building.getNumFloors()) {
                            // Get elevator reference and add request
                            auto& elevators = building.getElevators();
                            const_cast<Elevator&>(elevators[selectedElevator]).addRequest(
                                Request(targetFloor, Request::NONE));
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing input: " << e.what() << std::endl;
                    }
                }
                
                // Exit input mode
                inputMode = false;
                inputText = "";
            }
            else if (event.key.code == sf::Keyboard::Escape) {
                // Cancel input on Escape
                inputMode = false;
                inputText = "";
            }
            else if (event.key.code == sf::Keyboard::BackSpace) {
                // Handle backspace to delete characters
                if (!inputText.empty()) {
                    inputText.pop_back();
                }
            }
            return true;
        }
    } 
    else {
        // Normal mode: handle game controls
        if (event.type == sf::Event::KeyPressed) {
            // Exit on Escape
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
                return true;
            }
            
            // Handle numeric keys for external requests
            if (event.key.code >= sf::Keyboard::Num1 && 
                event.key.code <= sf::Keyboard::Num9) {
                
                int floor = event.key.code - sf::Keyboard::Num0;
                if (floor <= building.getNumFloors()) {
                    // First check if there's an elevator already at this floor
                    if (checkElevatorAtFloor(floor)) {
                        // Open doors for all elevators at this floor
                        openDoorsForElevatorsAtFloor(floor);
                    } else {
                        // No elevator at floor, add normal request
                        Request::Direction direction = 
                            (event.key.shift) ? Request::DOWN : Request::UP;
                        
                        building.addRequest(Request(floor, direction));
                    }
                    return true;
                }
            }
            
            // Handle function keys for internal requests - support up to 10 elevators with F8-F17
            if (event.key.code >= sf::Keyboard::F8 && 
                event.key.code <= sf::Keyboard::F8 + building.getNumElevators() - 1) {
                
                int elevatorIndex = event.key.code - sf::Keyboard::F8;
                if (elevatorIndex < building.getNumElevators()) {
                    // Prompt for floor input
                    int targetFloor;
                    std::cout << "Enter floor for elevator " << (elevatorIndex + 1) << " (1-" 
                              << building.getNumFloors() << "): ";
                    std::cin >> targetFloor;
                    
                    if (targetFloor >= 1 && targetFloor <= building.getNumFloors()) {
                        auto& elevators = building.getElevators();
                        const_cast<Elevator&>(elevators[elevatorIndex]).addRequest(
                            Request(targetFloor, Request::NONE));
                    }
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool GUI::checkElevatorAtFloor(int floor) {
    const auto& elevators = building.getElevators();
    for (size_t i = 0; i < elevators.size(); i++) {
        if (elevators[i].getCurrentFloor() == floor && !elevators[i].isMoving() && !elevators[i].doorsOpen()) {
            return true;
        }
    }
    return false;
}

void GUI::openDoorsForElevatorsAtFloor(int floor) {
    std::cout << "Looking for an available elevator at floor " << floor << std::endl;
    
    const auto& elevators = building.getElevators();
    bool foundElevator = false;
    
    // Find the first available elevator at the floor
    for (size_t i = 0; i < elevators.size(); i++) {
        // If elevator is at the requested floor, stationary, and doors are closed
        if (elevators[i].getCurrentFloor() == floor && 
            !elevators[i].isMoving() && 
            !elevators[i].doorsOpen()) {
            
            std::cout << "Opening doors for elevator " << (i + 1) << std::endl;
            
            // Get a non-const reference and open the doors
            auto& mutableElevator = const_cast<Elevator&>(elevators[i]);
            mutableElevator.openDoors();
            
            // Play the door open and ding sounds immediately
            if (soundsLoaded) {
                doorOpenSound.play();
                elevatorDingSound.play();
            }
            
            // Only open one elevator's doors, then break the loop
            foundElevator = true;
            break;  // Important: Break after handling one elevator
        }
    }
    
    if (!foundElevator) {
        std::cout << "No available elevators found at floor " << floor << std::endl;
    }
}

void GUI::handleElevatorClick(int mouseX, int mouseY) {
    // Convert mouse position to view coordinates
    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(mouseX, mouseY));
    
    // Check if clicked on an elevator
    const auto& elevators = building.getElevators();
    for (size_t i = 0; i < elevators.size(); i++) {
        // Calculate elevator position and bounds
        float elevatorSpacing = buildingWidth / (elevators.size() + 1);
        float elevatorX = buildingLeft + elevatorSpacing * (i + 1) - elevatorWidth / 2;
        float y = buildingBottom - (elevators[i].getCurrentFloor() - 1) * floorHeight - floorHeight;
        
        // Create a rectangle representing the elevator's clickable area
        sf::FloatRect elevatorBounds(elevatorX, y, elevatorWidth, floorHeight - 10);
        
        // Check if converted mouse position is within the elevator bounds
        if (elevatorBounds.contains(worldPos)) {
            // Only interact with stationary elevators
            if (elevators[i].isMoving() || elevators[i].doorsOpen()) {
                continue;
            }
            
            // Enter input mode for this elevator
            inputMode = true;
            selectedElevator = i;
            inputText = "";
            inputBlinkClock.restart();
            break;
        }
    }
}

void GUI::handleFloorButtonClick(int mouseX, int mouseY) {
    // Convert mouse position to view coordinates
    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(mouseX, mouseY));
    
    // Check if any floor button was clicked
    for (const auto& pair : floorButtons) {
        int floor = pair.first + 1; // Convert index to floor number
        sf::FloatRect buttonBounds = pair.second;
        
        if (buttonBounds.contains(worldPos)) {
            std::cout << "Floor " << floor << " button clicked" << std::endl;
            
            // Check if an elevator is already at this floor
            if (checkElevatorAtFloor(floor)) {
                // Open doors of one elevator at this floor
                openDoorsForElevatorsAtFloor(floor);
            } else {
                // No elevator at floor, add normal UP request (could make this smarter later)
                building.addRequest(Request(floor, Request::UP));
            }
            break;
        }
    }
}

void GUI::onWindowResize(unsigned int width, unsigned int height) {
    // Update the view to match the new window size
    sf::FloatRect visibleArea(0, 0, width, height);
    window.setView(sf::View(visibleArea));
    
    // Update positions of UI elements
    scaleFactorText.setPosition(10, 10);
    instructionsText.setPosition(10, window.getSize().y - 70);
    inputPrompt.setPosition(window.getSize().x / 2 - 200 * scaleFactor, window.getSize().y / 2 - 70 * scaleFactor);
    inputBox.setPosition(window.getSize().x / 2 - 200 * scaleFactor, window.getSize().y / 2 - 30 * scaleFactor);
    
    // Resize the input box
    inputBox.setSize(sf::Vector2f(400 * scaleFactor, 60 * scaleFactor));
}

void GUI::update(float deltaTime) {
    // Update input mode blink timer
    float blinkTime = inputBlinkClock.getElapsedTime().asSeconds();
    if (blinkTime > 1.0f) {
        inputBlinkClock.restart();
    }
    
    // Play sounds for elevator actions
    playSounds();
}

void GUI::playSounds() {
    const auto& elevators = building.getElevators();
    
    // Make sure the previous doors state vector is properly sized
    if (prevDoorsOpen.size() != elevators.size()) {
        prevDoorsOpen.resize(elevators.size(), false);
    }
    
    for (size_t i = 0; i < elevators.size(); i++) {
        // Play door open sound when doors change from closed to open
        if (elevators[i].doorsOpen() && !prevDoorsOpen[i] && soundsLoaded) {
            doorOpenSound.play();
        }
        
        // Play door close sound when doors change from open to closed
        if (!elevators[i].doorsOpen() && prevDoorsOpen[i] && soundsLoaded) {
            doorCloseSound.play();
        }
        
        // Play ding sound when elevator arrives at floor (doors opening)
        if (elevators[i].doorsOpen() && !prevDoorsOpen[i] && soundsLoaded) {
            elevatorDingSound.play();
        }
        
        // Update previous state
        prevDoorsOpen[i] = elevators[i].doorsOpen();
    }
}

void GUI::render() {
    window.clear(sf::Color::White);
    
    // Draw scale factor text at the top
    window.draw(scaleFactorText);
    
    // Draw instructions
    window.draw(instructionsText);
    
    // Draw building and elevators
    drawBuilding();
    drawFloors();
    drawElevators();
    
    // Highlight floor buttons if they're being hovered
    if (!inputMode) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        
        for (const auto& pair : floorButtons) {
            sf::FloatRect buttonBounds = pair.second;
            
            if (buttonBounds.contains(worldPos)) {
                // Draw a highlight for the hovered button
                sf::CircleShape highlight(12);
                highlight.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent yellow
                highlight.setPosition(buttonBounds.left - 2, buttonBounds.top - 2);
                window.draw(highlight);
                break;
            }
        }
    }
    
    // Draw input mode UI if active
    if (inputMode) {
        drawInputUI();
    }
    
    window.display();
}

void GUI::drawBuilding() {
    // Draw building outline
    sf::RectangleShape buildingOutline(sf::Vector2f(buildingWidth, building.getNumFloors() * floorHeight));
    buildingOutline.setOutlineColor(sf::Color::Black);
    buildingOutline.setOutlineThickness(2);
    buildingOutline.setFillColor(sf::Color(240, 240, 240, 255)); // Light gray
    buildingOutline.setPosition(buildingLeft, buildingBottom - building.getNumFloors() * floorHeight);
    window.draw(buildingOutline);
}

void GUI::drawFloors() {
    for (int i = 0; i < building.getNumFloors(); i++) {
        // Floor line
        sf::RectangleShape floorLine(sf::Vector2f(buildingWidth, 2));
        floorLine.setFillColor(sf::Color(150, 150, 150));
        floorLine.setPosition(buildingLeft, buildingBottom - i * floorHeight);
        window.draw(floorLine);
        
        // Floor number
        sf::Text floorNumber;
        floorNumber.setFont(font);
        floorNumber.setString(std::to_string(i + 1));
        floorNumber.setCharacterSize(18);
        floorNumber.setFillColor(sf::Color::Black);
        floorNumber.setPosition(buildingLeft - 30, buildingBottom - i * floorHeight - 20);
        window.draw(floorNumber);
        
        // Floor call button (single button that calls elevator to this floor)
        sf::CircleShape callButton(10);
        callButton.setFillColor(sf::Color(180, 180, 180));
        callButton.setOutlineColor(sf::Color::Black);
        callButton.setOutlineThickness(1);
        callButton.setPosition(buildingLeft - 50, buildingBottom - i * floorHeight - 30 * scaleFactor);
        
        // Store the button position and floor for click detection
        floorButtons[i] = callButton.getGlobalBounds();
        
        window.draw(callButton);
        
        // Add a small icon inside button
        sf::ConvexShape arrow;
        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f(0, 0));
        arrow.setPoint(1, sf::Vector2f(10, 0));
        arrow.setPoint(2, sf::Vector2f(5, 8));
        arrow.setFillColor(sf::Color::Black);
        arrow.setPosition(buildingLeft - 45, buildingBottom - i * floorHeight - 27 * scaleFactor);
        window.draw(arrow);
    }
}

void GUI::drawElevators() {
    const auto& elevators = building.getElevators();
    for (size_t i = 0; i < elevators.size(); i++) {
        float elevatorSpacing = buildingWidth / (elevators.size() + 1);
        float elevatorX = buildingLeft + elevatorSpacing * (i + 1) - elevatorWidth / 2;
        
        // Elevator shaft
        sf::RectangleShape elevatorShaft(sf::Vector2f(elevatorWidth + 10, building.getNumFloors() * floorHeight));
        elevatorShaft.setFillColor(sf::Color(220, 220, 220));
        elevatorShaft.setPosition(elevatorX - 5, buildingBottom - building.getNumFloors() * floorHeight);
        window.draw(elevatorShaft);
        
        // Elevator car
        sf::RectangleShape elevatorCar(sf::Vector2f(elevatorWidth, floorHeight - 10));
        
        // Color based on state
        if (elevators[i].doorsOpen()) {
            elevatorCar.setFillColor(sf::Color::Green);
        } else if (elevators[i].isMoving()) {
            elevatorCar.setFillColor(sf::Color::Yellow);
        } else {
            elevatorCar.setFillColor(sf::Color::Blue);
        }
        
        // Position based on current floor
        float y = buildingBottom - (elevators[i].getCurrentFloor() - 1) * floorHeight - floorHeight;
        elevatorCar.setPosition(elevatorX, y);
        window.draw(elevatorCar);
        
        // Elevator ID
        sf::Text elevatorIdText;
        elevatorIdText.setFont(font);
        elevatorIdText.setString(std::to_string(i + 1));
        elevatorIdText.setCharacterSize(18);
        elevatorIdText.setFillColor(sf::Color::White);
        elevatorIdText.setPosition(elevatorX + elevatorWidth / 2 - 5, y + floorHeight / 2 - 10);
        window.draw(elevatorIdText);
        
        // Current floor display above elevator
        sf::Text floorDisplay;
        floorDisplay.setFont(font);
        std::stringstream ss;
        ss << "At: " << elevators[i].getCurrentFloor();
        floorDisplay.setString(ss.str());
        floorDisplay.setCharacterSize(16 * scaleFactor);
        floorDisplay.setFillColor(sf::Color::Black);
        floorDisplay.setPosition(elevatorX - 5, buildingBottom - building.getNumFloors() * floorHeight - (20 * scaleFactor));
        window.draw(floorDisplay);
    }
}

void GUI::drawInputUI() {
    // Draw semi-transparent overlay for entire screen
    sf::RectangleShape fullOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    fullOverlay.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black
    window.draw(fullOverlay);
    
    // Create a more visible modal dialog - scale with the UI scale factor
    sf::RectangleShape modalBox(sf::Vector2f(500 * scaleFactor, 200 * scaleFactor));
    modalBox.setFillColor(sf::Color(50, 50, 100, 255)); // Solid color for visibility
    modalBox.setOutlineColor(sf::Color::White);
    modalBox.setOutlineThickness(3 * scaleFactor);
    modalBox.setPosition(window.getSize().x / 2 - 250 * scaleFactor, window.getSize().y / 2 - 100 * scaleFactor);
    window.draw(modalBox);
    
    // Update font sizes for scaled UI
    inputPrompt.setCharacterSize(static_cast<unsigned int>(24 * scaleFactor));
    
    // Update and draw input prompt text
    inputPrompt.setString("Enter floor for Elevator " + 
                         std::to_string(selectedElevator + 1) + 
                         " (1-" + std::to_string(building.getNumFloors()) + "):");
    window.draw(inputPrompt);
    
    // Draw input text with blinking cursor
    sf::Text inputDisplay;
    inputDisplay.setFont(font);
    
    // Add blinking cursor
    float blinkTime = inputBlinkClock.getElapsedTime().asSeconds();
    bool showCursor = (blinkTime < 0.5f);
    std::string displayText = inputText;
    if (showCursor) {
        displayText += "_";
    }
    
    inputDisplay.setString(displayText);
    inputDisplay.setCharacterSize(static_cast<unsigned int>(32 * scaleFactor)); // Larger text for better visibility
    inputDisplay.setFillColor(sf::Color::White);
    inputDisplay.setPosition(window.getSize().x / 2 - 180 * scaleFactor, window.getSize().y / 2 - 10 * scaleFactor);
    window.draw(inputDisplay);
    
    // Draw help text
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setString("Press ENTER to confirm or ESC to cancel");
    helpText.setCharacterSize(static_cast<unsigned int>(18 * scaleFactor));
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(window.getSize().x / 2 - 180 * scaleFactor, window.getSize().y / 2 + 40 * scaleFactor);
    window.draw(helpText);
}
