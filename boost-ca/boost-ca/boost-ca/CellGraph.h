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
#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/scoped_ptr.hpp>

struct VertexProperties
{
    bool cellValue;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperties> graph_t;
typedef boost::property_map<graph_t, boost::vertex_index_t>::type CellIndex;
typedef boost::property_map<graph_t, bool VertexProperties::*>::type CellValue;
typedef boost::graph_traits<graph_t>::vertex_iterator CellIterator;
typedef boost::graph_traits<graph_t>::out_edge_iterator OutEdgeIterator;
typedef boost::graph_traits<graph_t>::in_edge_iterator InEdgeIterator;
typedef boost::dynamic_bitset<> GraphState;

class CellGraph
{
    static const size_t blockSize;
    static const size_t keySize;
    
    char* memoryFileBuffer;
    
    graph_t m_graph;
    std::bitset<128> key;
    std::bitset<128> cipherState;
    size_t offset;
    boost::dynamic_bitset<> constant;
    static unsigned int edgeCount;
    bool localLinkFunction(const bool* argv) const
    {
        return (argv[0] + argv[1]) % 2;
    }
    bool localLinkFunction(std::bitset<6> argv) const
    {
        return (argv[0] + argv[1]) % 2;
    }
    
    void computeRound();

    void loadGraphState();
    void loadState(const std::vector<unsigned char>& state);
    void loadCipherState();
    void saveCipherState();
    void evolve();

public:
    CellGraph(size_t vertexCount);
    ~CellGraph();

    GraphState getCurrentState() const;
    
    void loadFromFile(const std::string &graphPath, const std::string& plainTextPath);

    void printState() const;
    void printEdges() const;
    
    void encrypt();
    
    const size_t getVertexCount() const
    {
        return boost::num_vertices(m_graph);
    }
    
};

#endif /* defined(__boost_ca__CellGraph__) */
