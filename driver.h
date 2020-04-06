
#ifndef __DRIVER_H
#define __DRIVER_H


class Driver
{
public:
    Driver();
    ~Driver();

    virtual void Activate();
    virtual int Reset();
    virtual void Deactivate();
};

class DriverManager
{
private:
    Driver *drivers[255];
    int numDrivers;

public:
    void AddDriver(Driver*);
    DriverManager();

    void ActivateAll();
};

#endif