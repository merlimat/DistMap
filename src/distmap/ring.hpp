/*
 * ring.hpp
 *
 *  Created on: Nov 9, 2009
 *      Author: mat
 */

#ifndef RING_HPP_
#define RING_HPP_

#include <distmap/util/hash.hpp>

#include <set>
#include <string>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace distmap
{

class Ring
{
public:
    Ring();
    ~Ring();

    void add( const std::string& node, uint32_t n = 100 );

    void remove( const std::string& node );

    const std::string& node( const std::string& key );

private:
    typedef std::set<std::string> StringSet;
    typedef boost::bimap<
                boost::bimaps::multiset_of<uint64_t>,
                boost::bimaps::multiset_of<std::string> >
            StringMap;

    StringSet m_physicalNodes;
    StringMap m_ring;
};

}

#endif /* RING_HPP_ */
