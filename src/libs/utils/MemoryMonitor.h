/*
Copyright(c) 2019 Cedric Jimenez

This file is part of cantools.

cantools is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cantools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with cantools.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#define DISABLE_MEMORY_MONITOR
#ifndef DISABLE_MEMORY_MONITOR

#include <cstdint>
#include <mutex>


/** \brief Utility class to monitor memory consumption */
class MemoryMonitor
{
    public:

        /** \brief Memory stats */
        struct MemoryStats
        {
            /** \brief Allocated memory size */
            size_t allocated_mem_size;
            /** \brief Number of alocated memory blocks */
            size_t allocated_blocks_count;

            /** \brief Maximum allocated memory size */
            size_t max_allocated_mem_size;
            /** \brief Maximum Number of alocated memory blocks */
            size_t max_allocated_blocks_count;
        };


        /** \brief Get the unique instance */
        static MemoryMonitor& getInstance();


        /** \brief Allocate a block of memory */
        void* allocate(const size_t& size);

        /** \brief De-allocate a block of memory */
        void deallocate(void* mem_block);
        
        /** \brief Get the current memory statistics */
        void getMemoryStats(MemoryStats& mem_stats);




    private:

        /** \brief Allocated block header */
        struct AllocatedBlock
        {
            /** \brief Magic */
            uint32_t magic;
            /** \brief Block size in bytes */
            size_t size;
            /** \brief Previous block */
            AllocatedBlock* previous;
            /** \brief Next block */
            AllocatedBlock* next;


            /** \brlef Block magic */
            static const uint32_t MAGIC = 0xDEADC0DEu;
        };


        /** \brief Memory monitor mutex */
        std::mutex m_mutex;

        /** \brief List of allocated block */
        AllocatedBlock* m_blocks;

        /** \brief Memory statistics */
        MemoryStats m_mem_stats;


        /** \brief Unique instance */
        static MemoryMonitor* m_singleton;
};

#endif // DISABLE_MEMORY_MONITOR

#endif // MEMORYMONITOR_H
