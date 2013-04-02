//
//  CellGraph.cpp
//  boost-ca
//
//  Created by Кирилл Романовский on 17.03.13.
//  Copyright (c) 2013 mstu. All rights reserved.
//

#include "CellGraph.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <list>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/graph/adjacency_iterator.hpp>

using namespace boost::graph;

using boost::dynamic_bitset;
using boost::lexical_cast;
using boost::property_map;
using boost::tie;
using boost::vertex_index;

using std::bitset;
using std::cout;
using std::endl;
using std::ifstream;
using std::list;
using std::string;
using std::vector;

unsigned int CellGraph::edgeCount = 0;
const size_t CellGraph::blockSize = 128;
const size_t CellGraph::keySize = 128;

CellGraph::CellGraph(size_t vertexCount):
//    m_graph(new graph_t(vertexCount)),
    m_graph(vertexCount),
    key(0x1234567890),
    constant(vertexCount - (blockSize/2 + keySize/2), 0x42123345643ul)
{
    // TODO: add correct method to fill the cells
    CellIterator vi, viend;

    for (tie(vi,viend) = vertices(m_graph); vi != viend; ++vi)
        m_graph[*vi].cellValue = 0;//rand()%2;
        
    cout << "key: " << key << endl;
    cout << "round constant: "<< constant << endl;
    
    offset = 0;
}

CellGraph::~CellGraph()
{
    delete [] memoryFileBuffer;
}

void CellGraph::printState() const
{
    boost::print_vertices(m_graph, get(&VertexProperties::cellValue, m_graph));
}

void CellGraph::printEdges() const
{
    boost::print_edges(m_graph, get(vertex_index, m_graph));
}

GraphState CellGraph::getCurrentState() const
{
    CellIterator vi, viend;
    GraphState state(getVertexCount());
    
    int i = 0;
    for (tie(vi,viend) = vertices(m_graph); vi != viend; ++vi)
        state[i++] = m_graph[*vi].cellValue;
    return state;
}

void CellGraph::evolve()
{
    CellIterator vi, vi_end;
    graph_t::adjacency_iterator ni, ni_end;
    
    unsigned long arg = 0;
    for (tie(vi,vi_end) = vertices(m_graph); vi != vi_end; ++vi)
    {
        int i = 0;
        for (tie(ni, ni_end) = adjacent_vertices(*vi, m_graph); ni != ni_end; ++ni)
        {
            arg |= m_graph[*ni].cellValue;
            arg <<= 1;
            ++i;
        }
        m_graph[*vi].cellValue = arg % 2;
    }
}


//void CellGraph::evolve()
//{
//    CellValue value = get(&VertexProperties::cellValue, m_graph);
//    CellIterator vi, vi_end;
//    graph_t::adjacency_iterator ni, ni_end;
//    
//    bool* vars = new bool[edgeCount];
//    for (tie(vi,vi_end) = vertices(m_graph); vi != vi_end; ++vi)
//    {
//        int i = 0;
//        for (tie(ni, ni_end) = adjacent_vertices(*vi, m_graph); ni != ni_end; ++ni)
//        {
//            vars[i] = (value[*ni]);
//            ++i;
//        }
//        localLinkFunction(vars);
//    }
//    delete [] vars;
//}

void CellGraph::loadFromFile(const string &graphPath, const string& plainTextPath)
{
    ifstream edgeFile(graphPath);
    int currentVertexIndex = 0;
    unsigned int lastEdgeCount = 0;
    while (edgeFile.peek() != EOF)
    {
        string adjacencyString;
        getline(edgeFile, adjacencyString);
        if (adjacencyString.empty()) continue;
        
        list<string> vertexStringList;
        boost::split(vertexStringList, adjacencyString, boost::is_any_of(","), boost::token_compress_on);
        edgeCount = static_cast<unsigned int>(vertexStringList.size());
        // TODO: create exceptions
        if (lastEdgeCount != 0 && lastEdgeCount != edgeCount)
            throw "Edge count differs!";
        lastEdgeCount = edgeCount;
        BOOST_FOREACH(string vertexString, vertexStringList)
        {
            int index = lexical_cast<int>(vertexString);
            add_edge(vertex(currentVertexIndex, m_graph), vertex(index, m_graph), m_graph);
        }
        vertexStringList.clear();
        currentVertexIndex++;
    }
    edgeFile.close();
    
    ifstream dataFile(plainTextPath, ifstream::binary);
    dataFile.seekg (0, dataFile.end);
    size_t length = dataFile.tellg();
    dataFile.seekg (0, dataFile.beg);
    memoryFileBuffer = new char[length];
    dataFile.read(memoryFileBuffer, length);
    dataFile.close();
    
}

void CellGraph::loadGraphState()
{
    CellIterator vi, vi_end;
    tie(vi,vi_end) = vertices(m_graph);
    while (*vi < blockSize/2 && vi != vi_end)
    {
        m_graph[*vi].cellValue = cipherState[*vi + blockSize/2];
        vi++;
#ifdef DEBUG
        cout << *vi << " ";
#endif
    }
    for (int i = 0; *vi >= blockSize/2 && *vi < blockSize/2 + keySize/2 && vi != vi_end; ++vi, ++i) {
        m_graph[*vi].cellValue = key[i];
#ifdef DEBUG 
        cout << *vi << " ";
#endif
    }
    for (int i = 0; *vi >= blockSize/2 + keySize/2 && vi != vi_end; ++vi, ++i) {
        m_graph[*vi].cellValue = constant[i];
#ifdef DEBUG 
        cout << *vi << " ";
#endif
    }
}

void CellGraph::loadState(const vector<unsigned char>& state)
{
}

void CellGraph::loadCipherState()
{
    size_t nibble = offset;
    for (size_t byte = 0; byte < blockSize/8; ++byte)
    {
        char currentByte = (memoryFileBuffer[nibble++]);
        for (int i =0; i < 8; ++i)
        {
            cipherState[byte*8 + i] = currentByte % 2;
            currentByte /= 2;
        }
    }
#ifdef DEBUG
    cout << endl << "loaded cipher state: " << cipherState << endl;
#endif
}

void CellGraph::saveCipherState()
{
    CellIterator vi, viend;
    tie(vi,viend) = vertices(m_graph);
    
    for (size_t bit = 0; bit < blockSize/2; ++bit, ++vi)
    {
        cipherState[blockSize/2 + bit] = cipherState[bit];
        cipherState[bit] = m_graph[*vi].cellValue;
    }
#ifdef DEBUG
    cout << endl << "saved cipher state: " << cipherState << endl;
#endif
}

void CellGraph::computeRound()
{
    loadCipherState();
    loadGraphState();
    evolve();
    saveCipherState();
}

void CellGraph::encrypt()
{
    computeRound();
}
