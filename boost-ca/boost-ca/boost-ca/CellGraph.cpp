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
#include <boost/graph/adjacency_iterator.hpp>

using namespace boost::graph;
using boost::lexical_cast;
using boost::property_map;
using boost::tie;
using boost::vertex_index;

using std::cout;
using std::endl;
using std::ifstream;
using std::list;
using std::string;
using std::vector;

CellGraph::CellGraph(size_t vertexCount):m_graph(new graph_t(vertexCount))
{
    // TODO: add correct method to fill the cells
    CellIterator vi, viend;
    CellValue value = get(&VertexProperties::cellValue, *(m_graph.get()));

    for (tie(vi,viend) = vertices(*m_graph.get()); vi != viend; ++vi)
        value[*vi] = rand()%2;
}

void CellGraph::printState()
{
    boost::print_vertices(*m_graph.get(), get(&VertexProperties::cellValue, *(m_graph.get())));
}

void CellGraph::printEdges()
{
    boost::print_edges(*m_graph.get(), get(vertex_index, *m_graph.get()));
}

GraphState CellGraph::getCurrentState()
{
    CellIterator vi, viend;
    CellValue value = get(&VertexProperties::cellValue, *(m_graph.get()));
    GraphState state;
    state.clear();
    
    for (tie(vi,viend) = vertices(*m_graph.get()); vi != viend; ++vi)
        state.push_back(value[*vi]);
    return state;
}

void CellGraph::evolve()
{
    CellValue value = get(&VertexProperties::cellValue, *m_graph.get());
    CellIterator vi, vi_end;
    graph_t::adjacency_iterator ni, ni_end;
    
    bool* vars = new bool[edgeCount];
    for (tie(vi,vi_end) = vertices(*m_graph.get()); vi != vi_end; ++vi)
    {
        int i = 0;
        for (tie(ni, ni_end) = adjacent_vertices(*vi, *m_graph.get()); ni != ni_end; ++ni)
        {
            vars[i] = (value[*ni]);
            ++i;
        }
        value[*vi] = localLinkFunction(edgeCount, vars);
    }
    delete [] vars;
}

void CellGraph::loadFromFile(const string &path)
{
    ifstream file(path);
    int currentVertexIndex = 0;
    unsigned int lastEdgeCount = 0;
    while (file.peek() != EOF)
    {
        string adjacencyString;
        getline(file, adjacencyString);
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
            add_edge(vertex(currentVertexIndex, *m_graph.get()), vertex(index, *m_graph.get()), *m_graph.get());
        }
        vertexStringList.clear();
        currentVertexIndex++;
    }
}

