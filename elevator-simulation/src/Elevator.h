#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Request.h"

class Elevator {
public:
    Elevator(int id, int totalFloors);
    
    void moveToFloor(int floor);
    void openDoors();
    void closeDoors();
    void addRequest(const Request& request);
    void update();
    
    int getCurrentFloor() const;
    bool isMoving() const;
    bool doorsOpen() const;
    Request::Direction getCurrentDirection() const;

private:
    int id;
    int currentFloor;
    bool moving;
    bool doorsOpenState;
    std::vector<Request> requests;
    int totalFloors;
    Request::Direction direction;
    float movementProgress; // 0.0 to 1.0 for smooth animation
    float doorProgress;     // 0.0 to 1.0 for door animation
    sf::Clock movementClock;
    sf::Clock doorClock;
    sf::Clock doorTimer;    // Timer for how long doors stay open
    int targetFloor;

    void processRequests();
    void sortRequestsByEfficiency();
};

#endif // ELEVATOR_H