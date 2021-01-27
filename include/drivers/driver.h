#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H
#include <common/types.h>
namespace myos
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
        private:
            Driver *drivers[256];
            int numDriver;

        public:
            DriverManager();
            ~DriverManager();
            void AddDriver(Driver *);
            void ActivateAll();
        };
    } // namespace drivers
} // namespace myos
#endif