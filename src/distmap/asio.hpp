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

class InternalSharedBuffer: public std::vector<char>, public IntrusiveBase<
        InternalSharedBuffer>
{
public:
    InternalSharedBuffer( size_t size ) :
        std::vector<char>( size )
    {
    }

    friend class SharedBuffer;
};

typedef boost::intrusive_ptr<InternalSharedBuffer> InternalSharedBufferPtr;

class SharedBuffer
{
public:
    SharedBuffer( size_t size ) :
        m_data( new InternalSharedBuffer( size ) ), m_buffer( asio::buffer(
                *m_data ) )
    {
    }

    // Implement the ConstBufferSequence requirements.
    typedef asio::mutable_buffer value_type;
    typedef asio::mutable_buffer* iterator;
    typedef const asio::mutable_buffer* const_iterator;

    const asio::mutable_buffer* begin() const
    {
        return &m_buffer;
    }

    const asio::mutable_buffer* end() const
    {
        return &m_buffer + 1;
    }

    char* data()
    {
        return &(*m_data)[0];
    }

    size_t size() const
    {
        return m_data->size();
    }

    void resize( size_t newSize )
    {
        m_data->resize( newSize );
    }

    void append( const void* ptr, size_t size )
    {
        m_data->insert( m_data->end(), (const char*) ptr, (const char*) ptr
                + size );
    }

private:
    InternalSharedBufferPtr m_data;
    asio::mutable_buffer m_buffer;
};

}

#endif /* ASIO_HPP_ */
