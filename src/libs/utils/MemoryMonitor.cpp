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


#include "MemoryMonitor.h"

#ifndef DISABLE_MEMORY_MONITOR

#include <cstdlib>
#include <iostream>
using namespace std;



/** \brief Unique instance */
MemoryMonitor* MemoryMonitor::m_singleton = nullptr;



/** \brief Get the unique instance */
MemoryMonitor& MemoryMonitor::getInstance()
{
    if (m_singleton == nullptr)
    {
        static uint8_t memory_monitor[sizeof(MemoryMonitor)];
        m_singleton = new(memory_monitor) MemoryMonitor();
    }

    return *m_singleton;
}


/** \brief Allocate a block of memory */
void* MemoryMonitor::allocate(const size_t& size)
{
    void* mem = malloc(size + sizeof(AllocatedBlock));
    if (mem != nullptr)
    {
        lock_guard<mutex> guard(m_mutex);

        AllocatedBlock* block = reinterpret_cast<AllocatedBlock*>(mem);
        block->magic = AllocatedBlock::MAGIC;
        block->size = size;
        block->previous = nullptr;
        block->next = m_blocks;
        if (m_blocks != nullptr)
        {
            m_blocks->previous = block;
        }
        m_blocks = block;

        uint8_t* allocated_memory = reinterpret_cast<uint8_t*>(mem);
        mem = &allocated_memory[sizeof(AllocatedBlock)];

        m_mem_stats.allocated_blocks_count++;
        if (m_mem_stats.allocated_blocks_count > m_mem_stats.max_allocated_blocks_count)
        {
            m_mem_stats.max_allocated_blocks_count = m_mem_stats.allocated_blocks_count;
        }

        m_mem_stats.allocated_mem_size += size;
        if (m_mem_stats.allocated_mem_size > m_mem_stats.max_allocated_mem_size)
        {
            m_mem_stats.max_allocated_mem_size = m_mem_stats.allocated_mem_size;
        }
    }
    return mem;
}

/** \brief De-allocate a block of memory */
void MemoryMonitor::deallocate(void* mem_block)
{
    if (mem_block != nullptr)
    {
        uint8_t* allocated_memory = reinterpret_cast<uint8_t*>(mem_block);
        mem_block = &allocated_memory[-1 * static_cast<int>(sizeof(AllocatedBlock))];

        AllocatedBlock* block = reinterpret_cast<AllocatedBlock*>(mem_block);
        if (block->magic == AllocatedBlock::MAGIC)
        {
            lock_guard<mutex> guard(m_mutex);

            if (block->next != nullptr)
            {
                block->next->previous = block->previous;
            }
            if (block->previous == nullptr)
            {
                m_blocks = block->next;
            }
            else
            {
                block->previous->next = block->next;
            }

            m_mem_stats.allocated_blocks_count--;
            m_mem_stats.allocated_mem_size -= block->size;
        }

        free(mem_block);
    }
}

/** \brief Get the current memory statistics */
void MemoryMonitor::getMemoryStats(MemoryStats& mem_stats)
{
    lock_guard<mutex> guard(m_mutex);

    mem_stats = m_mem_stats;
}



/** \brief New operator */
void* operator new (std::size_t size)
{
    return MemoryMonitor::getInstance().allocate(size);
}

/** \brief New operator */
void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
    (void)nothrow_value;
    return MemoryMonitor::getInstance().allocate(size);
}

/** \brief New[] operator */
void* operator new[](std::size_t size)
{
    return MemoryMonitor::getInstance().allocate(size);
}

/** \brief New[] operator */
void* operator new[](std::size_t size, const std::nothrow_t& nothrow_value) noexcept
{
    (void)nothrow_value;
    return MemoryMonitor::getInstance().allocate(size);
}

/** \brief Delete operator */
void operator delete (void* ptr) noexcept
{
    MemoryMonitor::getInstance().deallocate(ptr);
}

/** \brief Delete operator */
void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) noexcept
{
    (void)nothrow_constant;
    MemoryMonitor::getInstance().deallocate(ptr);
}

/** \brief Delete[] operator */
void operator delete[](void* ptr) noexcept
{
    MemoryMonitor::getInstance().deallocate(ptr);
}

/** \brief Delete[] operator */
void operator delete[](void* ptr, const std::nothrow_t& nothrow_constant) noexcept
{
    (void)nothrow_constant;
    MemoryMonitor::getInstance().deallocate(ptr);
}

#endif // DISABLE_MEMORY_MONITOR
