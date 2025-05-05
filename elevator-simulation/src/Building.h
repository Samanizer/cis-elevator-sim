#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include "Elevator.h"
#include "Floor.h"
#include "Request.h"

class Building {
public:
    Building(int numFloors, int numElevators);
    
    void addRequest(const Request& request);
    void update();
    void addFloor();
    void addElevator();
    
    int getNumFloors() const;
    int getNumElevators() const;
    const std::vector<Elevator>& getElevators() const;
    std::vector<Floor>& getFloors();
    
private:
    int numFloors;
    std::vector<Elevator> elevators;
    std::vector<Floor> floors;
    
    Elevator* findBestElevator(const Request& request);
};

#endif // BUILDING_H