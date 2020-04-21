
#ifndef __TINYOS__DRIVERS__DRIVER_H
#define __TINYOS__DRIVERS__DRIVER_H

namespace tinyos
{
    namespace drivers
    {
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
        public:
            Driver *drivers[255];
            int numDrivers;

        public:
            void AddDriver(Driver*);
            DriverManager();

            void ActivateAll();
        };
    }
}
#endif