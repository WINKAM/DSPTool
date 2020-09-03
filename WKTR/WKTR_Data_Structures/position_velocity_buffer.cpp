/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#include "position_velocity_buffer.h"

namespace WKTR
{

Position_Velocity_Buffer::Position_Velocity_Buffer()
{
   clear();
}

void Position_Velocity_Buffer::append_position_sample(uint32_t id_object, Position_Sample input_sample) noexcept
{
    append_sample(&m_position_buffer_map, m_position_buffer_max_size, id_object, input_sample);
}

void Position_Velocity_Buffer::append_velocity_sample(uint32_t id_object, Velocity_Sample input_sample) noexcept
{
    append_sample(&m_velocity_buffer_map, m_velocity_buffer_max_size, id_object, input_sample);
}

begin_end_buf_iterators Position_Velocity_Buffer::get_position_buffer_begin_end(uint32_t id_object, bool *is_find) const noexcept
{
    return get_buffer_begin_end(&m_position_buffer_map, id_object, is_find);
}

begin_end_buf_iterators Position_Velocity_Buffer::get_velocity_buffer_begin_end(uint32_t id_object, bool *is_find) const noexcept
{
    return get_buffer_begin_end(&m_velocity_buffer_map, id_object, is_find);
}

void Position_Velocity_Buffer::set_position_buffer_max_size(unsigned int size) noexcept
{
    m_position_buffer_max_size = size;
    resize_buffers(&m_position_buffer_map, size);
}

void Position_Velocity_Buffer::set_velocity_buffer_max_size(unsigned int size) noexcept
{
    m_velocity_buffer_max_size = size;
    resize_buffers(&m_velocity_buffer_map, size);
}

unsigned int Position_Velocity_Buffer::get_position_buffer_max_size() const noexcept
{
    return m_position_buffer_max_size;
}

unsigned int Position_Velocity_Buffer::get_velocity_buffer_max_size() const noexcept
{
    return m_velocity_buffer_max_size;
}

void Position_Velocity_Buffer::clear() noexcept
{
    m_position_buffer_map.clear();
    m_velocity_buffer_map.clear();
}

void Position_Velocity_Buffer::append_sample(std::map<uint32_t, Object_Buffer>* ptr_buffer_map, unsigned int max_size, uint32_t id_object, Position_Sample sample) noexcept
{
    std::map<uint32_t, Object_Buffer>::iterator iterator_buffer = ptr_buffer_map->find(id_object);
    if (iterator_buffer == ptr_buffer_map->end())
    {
        Object_Buffer buffer;
        buffer.m_buffer.emplace_front(sample);
        ptr_buffer_map->insert({id_object, buffer});
    }
    else
    {
        if (iterator_buffer->second.m_buffer.size() >= max_size)
        {
            iterator_buffer->second.m_buffer.pop_back();
        }
        iterator_buffer->second.m_buffer.emplace_front(sample);
    }
}

begin_end_buf_iterators Position_Velocity_Buffer::get_buffer_begin_end(const std::map<uint32_t, Object_Buffer>* ptr_buffer_map, uint32_t id_object, bool *is_find) const noexcept
{
    begin_end_buf_iterators result;
    std::map<uint32_t, Object_Buffer>::const_iterator iterator_buffer = ptr_buffer_map->find(id_object);
    if (iterator_buffer != ptr_buffer_map->end())
    {
        *is_find = true;
        result.first = iterator_buffer->second.m_buffer.cbegin();
        result.second = iterator_buffer->second.m_buffer.cend();
    }
    else
    {
        *is_find = false;
    }
    return result;
}

void Position_Velocity_Buffer::resize_buffers(std::map<uint32_t, Object_Buffer>* ptr_buffer_map, int size) noexcept
{
    for (auto& buf : *ptr_buffer_map)
    {
        buf.second.resize(size);
    }
}

Position_Velocity_Buffer::~Position_Velocity_Buffer()
{
    clear();
}

void Object_Buffer::resize(unsigned int size) noexcept
{
    if (size < m_buffer.size())
    {
        // remove oldest elements
        m_buffer.resize(size);
    }
}

}
