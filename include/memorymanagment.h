#ifndef __MYOS__MEMORYMANAGMENT__H
#define __MYOS__MEMORYMANAGMENT__H
#include <common/types.h>
namespace myos
{
    class MemoryChunk
    {
        public:
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        myos::common::size_t size;
    };

    class MemoryManager
    {
    protected:
        MemoryChunk *first;

    public:
        static MemoryManager *activeMemoryManager;
        MemoryManager(myos::common::size_t first, myos::common::size_t size);
        ~MemoryManager();

        void *malloc(myos::common::size_t size);
        void free(void *ptr);
    };
} // namespace myos

void* operator new(unsigned size);
void* operator new[](unsigned size);
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);
void operator delete(void* ptr);
void operator delete[](void* ptr);
#endif
