/*
 * asio.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef ASIO_HPP_
#define ASIO_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace distmap
{

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace ph = boost::asio::placeholders;

using boost::bind;
namespace sys = boost::system;

namespace ptime = boost::posix_time;

template<typename T>
class IntrusiveBase
{
    uint32_t m_counter;
public:
    IntrusiveBase() :
        m_counter( 0 )
    {
    }

    boost::intrusive_ptr<T> ptr()
    {
        return boost::intrusive_ptr<T>( static_cast<T*> ( this ) );
    }

    friend inline void intrusive_ptr_add_ref( IntrusiveBase<T>* p )
    {
        p->m_counter++;
    }

    friend inline void intrusive_ptr_release( IntrusiveBase<T>* p )
    {
        p->m_counter--;
        if ( p->m_counter == 0 )
        {
            delete static_cast<T*> ( p );
        }
    }
};

class InternalSharedBuffer: public asio::streambuf, public IntrusiveBase<
        InternalSharedBuffer>
{
public:
    InternalSharedBuffer()
    {
    }

    friend class SharedBuffer;
};

typedef boost::intrusive_ptr<InternalSharedBuffer> InternalSharedBufferPtr;

class SharedBuffer
{
public:
    SharedBuffer() :
        m_data( new InternalSharedBuffer() )
    {
    }

    asio::streambuf::mutable_buffers_type prepare( size_t size )
    {
        return m_data->prepare( size );
    }

    InternalSharedBuffer* buffer()
    {
        return m_data.get();
    }

private:
    InternalSharedBufferPtr m_data;
};

class ConstSharedBuffer
{
public:
    ConstSharedBuffer() :
        m_data( new InternalSharedBuffer() ), m_buffer()
    {
    }

    // Implement the ConstBufferSequence requirements.
    typedef asio::const_buffer value_type;
    typedef asio::const_buffer* iterator;
    typedef const asio::const_buffer* const_iterator;

    const asio::const_buffer* begin() const
    {
        return &m_buffer;
    }

    const asio::const_buffer* end() const
    {
        return begin() + 1;
    }

    void finished()
    {
        m_buffer = m_data->data();
    }

    InternalSharedBuffer* buffer()
    {
        return m_data.get();
    }

private:
    InternalSharedBufferPtr m_data;
    asio::const_buffer m_buffer;
};

}

#endif /* ASIO_HPP_ */
