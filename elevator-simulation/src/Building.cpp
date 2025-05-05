#include "Building.h"
#include "Elevator.h"
#include "Floor.h"
#include <vector>
#include <cmath>

Building::Building(int numFloors, int numElevators) 
    : numFloors(numFloors) {
    
    // Create the elevators
    for (int i = 0; i < numElevators; i++) {
        elevators.push_back(Elevator(i, numFloors));
    }
}

void Building::addFloor() {
    floors.push_back(Floor(floors.size() + 1));
}

void Building::addElevator() {
    elevators.push_back(Elevator(elevators.size(), numFloors));
}

void Building::addRequest(const Request& request) {
    // Find the best elevator to handle this request
    Elevator* bestElevator = findBestElevator(request);
    if (bestElevator) {
        bestElevator->addRequest(request);
    }
}

void Building::update() {
    // Update all elevators
    for (auto& elevator : elevators) {
        elevator.update();
    }
}

int Building::getNumFloors() const {
    return numFloors;
}

int Building::getNumElevators() const {
    return elevators.size();
}

const std::vector<Elevator>& Building::getElevators() const {
    return elevators;
}

std::vector<Floor>& Building::getFloors() {
    return floors;
}

Elevator* Building::findBestElevator(const Request& request) {
    if (elevators.empty()) {
        return nullptr;
    }
    
    // If there's only one elevator, it's the best choice
    if (elevators.size() == 1) {
        return &elevators[0];
    }
    
    // Find the best elevator based on proximity and direction
    Elevator* bestElevator = nullptr;
    int minDistance = numFloors + 1; // More than the maximum possible distance
    
    for (auto& elevator : elevators) {
        // Calculate distance to the requested floor
        int distance = std::abs(elevator.getCurrentFloor() - request.getFloor());
        
        // If elevator is idle or heading in the same direction as the request, it's preferred
        if (!elevator.isMoving() || 
            (elevator.getCurrentDirection() == request.getDirection() && 
             ((request.getDirection() == Request::UP && elevator.getCurrentFloor() < request.getFloor()) ||
              (request.getDirection() == Request::DOWN && elevator.getCurrentFloor() > request.getFloor())))) {
            
            if (distance < minDistance) {
                minDistance = distance;
                bestElevator = &elevator;
            }
        }
    }
    
    // If no elevator was found based on direction, just pick the closest one
    if (!bestElevator) {
        for (auto& elevator : elevators) {
            int distance = std::abs(elevator.getCurrentFloor() - request.getFloor());
            if (distance < minDistance) {
                minDistance = distance;
                bestElevator = &elevator;
            }
        }
    }
    
    return bestElevator;
}