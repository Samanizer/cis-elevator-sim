#include "Request.h"

Request::Request(int floor, Direction direction) 
    : requestedFloor(floor), requestedDirection(direction) {
}

int Request::getFloor() const {
    return requestedFloor;
}

Request::Direction Request::getDirection() const {
    return requestedDirection;
}