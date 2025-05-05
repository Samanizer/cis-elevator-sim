#ifndef REQUEST_H
#define REQUEST_H

class Request {
public:
    enum Direction { UP, DOWN, NONE };
    
    Request(int floor, Direction direction);
    
    int getFloor() const;
    Direction getDirection() const;
    
private:
    int requestedFloor;
    Direction requestedDirection;
};

#endif // REQUEST_H