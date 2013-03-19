//
//  CellGraph.h
//  boost-ca
//
//  Created by Кирилл Романовский on 17.03.13.
//  Copyright (c) 2013 mstu. All rights reserved.
//

#ifndef __boost_ca__CellGraph__
#define __boost_ca__CellGraph__

#include <iostream>
#include <vector>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/scoped_ptr.hpp>

struct VertexProperties
{
    // TODO: get rid of index property
    // see http://www.boost.org/doc/libs/1_42_0/libs/graph/test/layout_test.cpp
    bool cellValue;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperties> graph_t;
typedef boost::property_map<graph_t, boost::vertex_index_t>::type CellIndex;
typedef boost::property_map<graph_t, bool VertexProperties::*>::type CellValue;
typedef boost::graph_traits<graph_t>::vertex_iterator CellIterator;
typedef boost::graph_traits<graph_t>::out_edge_iterator OutEdgeIterator;
typedef boost::graph_traits<graph_t>::in_edge_iterator InEdgeIterator;
typedef std::vector<bool> GraphState;

class CellGraph
{
    boost::scoped_ptr<graph_t> m_graph;
    unsigned int edgeCount;
    bool localLinkFunction(int argc, const bool* argv)
    {
        return (argv[0] + argv[1]) % 2;
    }

public:
    CellGraph(size_t vertexCount);

    GraphState getCurrentState();
    void evolve();
    
    void loadFromFile(const std::string& path);

    void printState();
    void printEdges();
};

#endif /* defined(__boost_ca__CellGraph__) */
