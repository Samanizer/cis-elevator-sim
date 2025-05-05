#ifndef FLOOR_H
#define FLOOR_H

#include <vector>
#include "Request.h"

class Floor {
public:
    Floor(int floorNumber);
    void requestElevator(int direction);
    void addRequest(const Request& request);
    void clearRequests();
    int getFloorNumber() const;
    const std::vector<Request>& getRequests() const;
    bool hasUpRequest() const;
    bool hasDownRequest() const;
    void addUpRequest();
    void addDownRequest();
    void clearUpRequest();
    void clearDownRequest();

private:
    int floorNumber;
    std::vector<Request> requests;
    bool upButtonPressed;
    bool downButtonPressed;
};

#endif // FLOOR_H