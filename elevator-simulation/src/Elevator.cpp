#include "Elevator.h"
#include "Request.h"
#include <algorithm>
#include <iostream>
#include <vector>

Elevator::Elevator(int id, int totalFloors) 
    : id(id), currentFloor(1), moving(false), doorsOpenState(false), totalFloors(totalFloors),
      direction(Request::NONE), movementProgress(0.0f), doorProgress(0.0f), targetFloor(1) {}

void Elevator::moveToFloor(int floor) {
    if (floor < 1 || floor > totalFloors || floor == currentFloor) {
        return;
    }
    
    targetFloor = floor;
    moving = true;
    direction = (targetFloor > currentFloor) ? Request::UP : Request::DOWN;
    movementProgress = 0.0f;
    movementClock.restart();
}

void Elevator::openDoors() {
    if (!doorsOpenState) {
        doorsOpenState = true;
        doorProgress = 0.0f;
        doorClock.restart();
        doorTimer.restart(); // Make sure door timer is restarted when doors open
    }
}

void Elevator::closeDoors() {
    if (doorsOpenState) {
        doorsOpenState = false;
        doorProgress = 0.0f;
        doorClock.restart();
    }
}

void Elevator::addRequest(const Request& request) {
    // Check if request is already in the queue
    for (const auto& req : requests) {
        if (req.getFloor() == request.getFloor()) {
            return; // Already have a request for this floor
        }
    }
    
    requests.push_back(request);
    sortRequestsByEfficiency();
}

void Elevator::update() {
    // Handle door state timing
    if (doorsOpenState) {
        // If doors have been open for 1.5 seconds, close them
        if (doorTimer.getElapsedTime().asSeconds() >= 1.5f) {
            closeDoors();
        }
    }
    
    // Handle door animation
    if (doorProgress < 1.0f) {
        float doorTime = doorClock.getElapsedTime().asSeconds();
        doorProgress = std::min(doorTime / 1.0f, 1.0f); // 1 second for door operation
    }
    
    // If we're moving, update position
    if (moving) {
        float moveTime = movementClock.getElapsedTime().asSeconds();
        movementProgress = std::min(moveTime / 2.0f, 1.0f); // 2 seconds per floor
        
        if (movementProgress >= 1.0f) {
            // Reached the target floor
            currentFloor = targetFloor;
            moving = false;
            openDoors(); // Open doors when arriving at floor
            
            // Remove requests for this floor
            auto it = std::remove_if(requests.begin(), requests.end(),
                [this](const Request& req) { return req.getFloor() == currentFloor; });
            requests.erase(it, requests.end());
            
            // Start door timer
            doorTimer.restart();
        }
    }
    else if (!requests.empty()) {
        // If we're not moving and doors are closed, process next request
        processRequests();
    }
}

int Elevator::getCurrentFloor() const {
    if (!moving) {
        return currentFloor;
    }
    
    // Calculate intermediate floor position for animation
    float floorDifference = targetFloor - currentFloor;
    return currentFloor + floorDifference * movementProgress;
}

bool Elevator::isMoving() const {
    return moving;
}

bool Elevator::doorsOpen() const {
    return doorsOpenState;
}

Request::Direction Elevator::getCurrentDirection() const {
    return direction;
}

void Elevator::processRequests() {
    if (requests.empty()) {
        direction = Request::NONE;
        return;
    }
    
    // Process the first request in the queue
    const Request& nextRequest = requests.front();
    moveToFloor(nextRequest.getFloor());
}

void Elevator::sortRequestsByEfficiency() {
    if (requests.size() <= 1) {
        return;
    }
    
    // Sort requests based on the current direction
    // If going up, handle all up requests first in ascending order
    // If going down, handle all down requests first in descending order
    std::sort(requests.begin(), requests.end(), 
        [this](const Request& a, const Request& b) {
            if (direction == Request::UP) {
                // If both floors are above current, sort by ascending floor
                if (a.getFloor() >= currentFloor && b.getFloor() >= currentFloor) {
                    return a.getFloor() < b.getFloor();
                }
                // If both floors are below current, sort by descending floor
                if (a.getFloor() < currentFloor && b.getFloor() < currentFloor) {
                    return a.getFloor() > b.getFloor();
                }
                // Prioritize floors above current
                return a.getFloor() >= currentFloor && b.getFloor() < currentFloor;
            }
            else if (direction == Request::DOWN) {
                // If both floors are below current, sort by descending floor
                if (a.getFloor() <= currentFloor && b.getFloor() <= currentFloor) {
                    return a.getFloor() > b.getFloor();
                }
                // If both floors are above current, sort by ascending floor
                if (a.getFloor() > currentFloor && b.getFloor() > currentFloor) {
                    return a.getFloor() < b.getFloor();
                }
                // Prioritize floors below current
                return a.getFloor() <= currentFloor && b.getFloor() > currentFloor;
            }
            else {
                // If idle, sort by proximity to current floor
                return std::abs(a.getFloor() - currentFloor) < std::abs(b.getFloor() - currentFloor);
            }
        });
}