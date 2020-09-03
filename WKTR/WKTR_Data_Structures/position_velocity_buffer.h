/***************************************************************************

             WINKAM TM strictly confidential 2019

 ***************************************************************************/
#ifndef POSITION_VELOCITY_BUFFER_H
#define POSITION_VELOCITY_BUFFER_H

#include <map>
#include <deque>
#include <stdint.h>
#include "position_sample.h"

namespace WKTR // WINKAM library. Digital Signal Processing
{

/* Class Position_Velocity_Buffer provides position and velocity data buffering.
 * It buffers data of all objects (players, balls).
 * Struct Object_Buffer is buffer for position or velocity of single player/ball.
 */
struct Object_Buffer
{
    std::deque<Position_Sample> m_buffer;
    void resize(unsigned int size) noexcept;
};

// struct Velocity_Sample has the same structure as Position_Sample; (m_x, m_y, m_z) is velocity in meters per second (millimeters per millisecond)
typedef Position_Sample Velocity_Sample;

// We define type begin_end_buf_iterators to code simplification
typedef std::pair<std::deque<Position_Sample>::const_iterator, std::deque<Position_Sample>::const_iterator> begin_end_buf_iterators;

class Position_Velocity_Buffer
{
private:
    // We use separate maps for position and velocity since sampling frequencies can be different
    std::map<uint32_t, Object_Buffer> m_position_buffer_map; // position buffer map: key is object id, value is buffer
    std::map<uint32_t, Object_Buffer> m_velocity_buffer_map; // velocity buffer map: key is object id, value is buffer

    int m_position_buffer_max_size = 361; // maximum possible size of position buffer (360 = 3 sec * 120 Hz + 1 sample)
    int m_velocity_buffer_max_size = 76;  // maximum possible size of position buffer (76  = 3 sec * 25 Hz  + 1 sample)

protected:
    /**
     * Gets iterators to begin and end of buffer. Use to traverse buffer from newest samples to oldest
     * @param ptr_buffer_map is pointer to buffer map
     * @param id_object is object (player/ball) id
     * @param is_find is output flag
     *              true  if this object is in map     (means that iterators can be used)
     *              false if this object is not in map (means that iterators can be used)
     * @return iterator of buffer begin and iterator of  buffer end
     */
    begin_end_buf_iterators get_buffer_begin_end(const std::map<uint32_t, Object_Buffer>* ptr_buffer_map, uint32_t id_object, bool *is_find) const noexcept;

    /**
     * Resizes buffers when changing max buffer size via set_..._buffer_max_size
     * Removes oldest samples if new size is smaller than old size
     * @param ptr_buffer_map is pointer to buffer map
     * @param size is new size
     */
    void resize_buffers(std::map<uint32_t, Object_Buffer>*  ptr_buffer_map, int size) noexcept;

    /**
     * Appends new sample to buffer
     * @param id_object is object (player/ball) id
     * @param max_size is maximum possible size of buffer
     * @param sample is position or velocity sample
     */
    void append_sample(std::map<uint32_t, Object_Buffer>* ptr_buffer_map, unsigned int max_size, uint32_t id_object, Position_Sample sample) noexcept;

public:
    Position_Velocity_Buffer();

    /**
     * Appends new position sample to buffer
     * @param id_object is object (player/ball) id
     * @param sample is filtered input position sample (X, Y, Z in millimeters and timestamp in milliseconds)
     */
    void append_position_sample(uint32_t id_object, Position_Sample input_sample) noexcept;

    /**
     * Appends new velocity sample to buffer
     * @param id_object is object (player/ball) id
     * @param sample is input velocity sample (X, Y, Z in meters per second and timestamp in milliseconds)
     */
    void append_velocity_sample(uint32_t id_object, Velocity_Sample input_sample) noexcept;

    /**
     * Gets iterators to begin and end of buffer. Use to traverse buffer from newest samples to oldest
     * @param id_object is object (player/ball) id
     * @param is_find is output flag
     *              true  if this object is in map     (means that iterators can be used)
     *              false if this object is not in map (means that iterators can be used)
     * @return iterator of buffer begin and iterator of  buffer end
     */
    begin_end_buf_iterators get_position_buffer_begin_end(uint32_t id_object, bool* is_find) const noexcept;

    /**
     * Gets iterators to begin and end of buffer. Use to traverse buffer from newest samples to oldest
     * @param id_object is object (player/ball) id
     * @param is_find is output flag
     *              true  if this object is in map     (means that iterators can be used)
     *              false if this object is not in map (means that iterators can be used)
     * @return iterator of buffer begin and iterator of  buffer end
     */
    begin_end_buf_iterators get_velocity_buffer_begin_end(uint32_t id_object, bool* is_find) const noexcept;

    /**
     * @brief Sets maximum size of position buffer.
     * Removes oldest samples if new size is smaller than old size.
     * @param size is maximum size; default value is 361 (3 sec * 120 Hz + 1 sample)
     */
    void set_position_buffer_max_size(unsigned int size = 361) noexcept;

    /**
     * @brief Sets maximum size of velocity buffer.
     * Removes oldest samples if new size is smaller than old size.
     * @param size is maximum size; default value is 76 (3 sec * 25 Hz + 1 sample)
     */
    void set_velocity_buffer_max_size(unsigned int size = 76) noexcept;

    /**
     * Gets maximum size of position buffer
     * @return size
     */
    unsigned int get_position_buffer_max_size() const noexcept;

    /**
     * Gets maximum size of velocity buffer
     * @return size
     */
    unsigned int get_velocity_buffer_max_size() const noexcept;

    /**
     * Clears buffers
     */
    void clear() noexcept;

    ~Position_Velocity_Buffer();
};

}

#endif // POSITION_VELOCITY_BUFFER_H
