//
//  main.cpp
//  boost-ca
//
//  Created by Кирилл Романовский on 13.03.13.
//  Copyright (c) 2013 mstu. All rights reserved.
//
#include <algorithm>
#include <string>
#include <iostream>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>

using namespace std;
using namespace boost;

struct VertexProperties {
    std::size_t index;
    bool cell_value;
};

bool local_link_fun(const vector<bool>& vars)
{
    return (vars[0] + vars[1]) % 2;
}

typedef adjacency_list<vecS, vecS, directedS, VertexProperties> CellGraph;

void print_state(CellGraph& g)
{
//    property_map<CellGraph, std::size_t VertexProperties::*>::type id = get(&VertexProperties::index, g);
    property_map<CellGraph, bool VertexProperties::*>::type val = get(&VertexProperties::cell_value, g);
    
    graph_traits<CellGraph>::vertex_iterator i, end;
    graph_traits<CellGraph>::out_edge_iterator ei, edge_end;
    vector<bool> cell_values;
    cell_values.clear();
    for (boost::tie(i,end) = vertices(g); i != end; ++i) {
//        cout << id[*i] << "[" << val[*i] << "] ";
        cell_values.push_back(val[*i]);
//        for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
//        {
//            cout << " ----> " << id[target(*ei, g)] << "[" << val[target(*ei, g)] << "]  ";
//        }
//        cout << endl;
    }
    for (vector<bool>::iterator it = cell_values.begin(); it != cell_values.end(); ++it) {
        cout << "[" << *it << "]";
    }
    cout << endl;
}

void evolve(CellGraph& g, int iteration_count)
{
    property_map<CellGraph, bool VertexProperties::*>::type val = get(&VertexProperties::cell_value, g);
    graph_traits<CellGraph>::vertex_iterator i, end;
    graph_traits<CellGraph>::out_edge_iterator ei, edge_end;
    for (int step = 0; iteration_count == 0 || step < iteration_count; ++ step)
    {
        for (boost::tie(i,end) = vertices(g); i != end; ++i) {
            vector<bool> vars;
            for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
                vars.push_back(val[target(*ei, g)]);
            val[*i] = local_link_fun(vars);
            vars.clear();
        }
        cout << step << ": ";
        print_state(g);
    }
}

int main(int argc, const char * argv[])
{
    srand ((unsigned int)time(NULL));
    const int V = 4;
    CellGraph g(V);
    property_map<CellGraph, std::size_t VertexProperties::*>::type id = get(&VertexProperties::index, g);
    property_map<CellGraph, bool VertexProperties::*>::type val = get(&VertexProperties::cell_value, g);

    boost::graph_traits<CellGraph>::vertex_iterator vi, viend;
    int vnum = 0;
    
    for (boost::tie(vi,viend) = vertices(g); vi != viend; ++vi)
    {
        id[*vi] = vnum++;
        val[*vi] = rand()%2;
    }
    add_edge(vertex(0, g), vertex(1, g), g);
    add_edge(vertex(0, g), vertex(2, g), g);
    add_edge(vertex(1, g), vertex(2, g), g);
    add_edge(vertex(1, g), vertex(3, g), g);

    add_edge(vertex(2, g), vertex(0, g), g);
    add_edge(vertex(2, g), vertex(3, g), g);
    add_edge(vertex(3, g), vertex(0, g), g);
    add_edge(vertex(3, g), vertex(1, g), g);
    print_edges(g, id);
    print_state(g);
    
    evolve(g, 0);

    cout << endl;
    return 0;
}

