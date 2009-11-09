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
#include <vector>
#include <string>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace distmap
{

typedef std::vector<std::string> StringList;

class Ring
{
public:
    typedef std::set<std::string> StringSet;

    Ring();
    ~Ring();

    void add( const std::string& node, uint32_t n = 100 );

    void remove( const std::string& node );

    const std::string& node( const std::string& key );

    void preferenceList( const std::string& key, StringList& nodes, uint32_t n );

private:

    typedef boost::bimap<
                boost::bimaps::multiset_of<uint64_t>,
                boost::bimaps::multiset_of<std::string> >
            StringMap;

    StringSet m_physicalNodes;
    StringMap m_ring;
};

}

#endif /* RING_HPP_ */
