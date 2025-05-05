#include "Floor.h"
#include "Request.h"
#include <vector>

Floor::Floor(int floorNumber)
    : floorNumber(floorNumber), upButtonPressed(false), downButtonPressed(false) {
}

void Floor::requestElevator(int direction) {
    // Create a request and add it to the list
    Request request(floorNumber, static_cast<Request::Direction>(direction));
    addRequest(request);
}

void Floor::addRequest(const Request& request) {
    requests.push_back(request);
}

void Floor::clearRequests() {
    requests.clear();
}

int Floor::getFloorNumber() const {
    return floorNumber;
}

const std::vector<Request>& Floor::getRequests() const {
    return requests;
}

bool Floor::hasUpRequest() const {
    return upButtonPressed;
}

bool Floor::hasDownRequest() const {
    return downButtonPressed;
}

void Floor::addUpRequest() {
    upButtonPressed = true;
}

void Floor::addDownRequest() {
    downButtonPressed = true;
}

void Floor::clearUpRequest() {
    upButtonPressed = false;
}

void Floor::clearDownRequest() {
    downButtonPressed = false;
}