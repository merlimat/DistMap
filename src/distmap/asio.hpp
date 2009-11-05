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

template<typename T, typename Pool>
class IntrusivePoolBase
{
    uint32_t m_counter;
    Pool& m_pool;
    T* m_next;

public:
    IntrusivePoolBase( Pool& pool ) :
        m_counter( 0 ), m_pool( pool ), m_next( 0 )
    {
    }

    boost::intrusive_ptr<T> ptr()
    {
        return boost::intrusive_ptr<T>( static_cast<T*> ( this ) );
    }

    void setNext( T* next )
    {
        m_next = next;
    }

    T* next() const
    {
        return m_next;
    }

    friend inline void intrusive_ptr_add_ref( IntrusivePoolBase<T,Pool>* p )
    {
        p->m_counter++;
    }

    friend inline void intrusive_ptr_release( IntrusivePoolBase<T,Pool>* p )
    {
        p->m_counter--;
        if ( p->m_counter == 0 )
        {
            p->m_pool.release( static_cast<T*> ( p ) );
        }
    }
};

class InternalSharedBuffer: public std::vector<char>, public IntrusiveBase<
        InternalSharedBuffer>
{
};

typedef boost::intrusive_ptr<InternalSharedBuffer> InternalSharedBufferPtr;

class SharedBuffer
{
public:
    SharedBuffer( size_t size = 1024 ) :
        m_data( new InternalSharedBuffer() )
    {
        m_data->resize( size );
        m_buffer = asio::buffer( *m_data );
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
        return begin() + 1;
    }

    void resize( size_t size )
    {
        m_data->resize( size );
        m_buffer = asio::buffer( *m_data );
    }

    size_t size() const
    {
        return m_data->size();
    }

    const char* data() const
    {
        return &(*m_data)[0];
    }

    char* data()
    {
        return &(*m_data)[0];
    }

private:
    InternalSharedBufferPtr m_data;
    asio::mutable_buffer m_buffer;
};

inline size_t readMessageSize( const SharedBuffer& buffer )
{
    const uint32_t* size = (const uint32_t*) buffer.data();
    return ntohl( *size );
}

template<typename Msg>
inline SharedBuffer writeMessage( const Msg& msg )
{
    size_t size = msg.ByteSize();
    SharedBuffer buffer( size );
    msg.SerializeToArray( buffer.data(), buffer.size() );
    return buffer;
}

template<typename Msg>
inline SharedBuffer writeMessageWithSize( const Msg& msg )
{
    size_t size = msg.ByteSize() + sizeof(uint32_t);
    SharedBuffer buffer( size );

    *(uint32_t*) buffer.data() = htonl( size );
    msg.SerializeToArray( buffer.data() + sizeof(uint32_t), buffer.size()
            - sizeof(uint32_t) );
    return buffer;
}

template<typename Msg>
inline void readMessage( const SharedBuffer& buffer, Msg& msg )
{
    msg.ParseFromArray( buffer.data(), buffer.size() );
}

template<typename Msg>
inline void readMessageWithSize( const SharedBuffer& buffer, Msg& msg )
{
    msg.ParseFromArray( buffer.data() + sizeof(uint32_t), buffer.size()
            - sizeof(uint32_t) );
}

}

#endif /* ASIO_HPP_ */
