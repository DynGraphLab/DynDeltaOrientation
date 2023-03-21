/******************************************************************************
 * graph_io.h

 *****************************************************************************/

#ifndef WIS_GRAPHIO_H_
#define WIS_GRAPHIO_H_

#include <data_structure/dyn_graph_access.h>
#include <data_structure/graph_access.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

#include "definitions.h"

class graph_io {
   public:
    graph_io();
    virtual ~graph_io();

    static int readGraphWeighted(graph_access& G, std::string filename);

    static int writeGraphWeighted(graph_access& G, std::string filename);

    static int writeGraph(graph_access& G, std::string filename);
    static int writeDirectedGraph(graph_access& G, std::string filename);

    static int readPartition(graph_access& G, std::string filename);

    static void writePartition(graph_access& G, std::string filename);

    template <typename vectortype>
    static void writeVector(std::vector<vectortype>& vec, std::string filename);

    template <typename vectortype>
    static void readVector(std::vector<vectortype>& vec, std::string filename);

    std::pair<int, int> readEdgeSequence(std::string file,
                                         std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence);

 };

template <typename vectortype>
void graph_io::writeVector(std::vector<vectortype>& vec, std::string filename) {
    std::ofstream f(filename.c_str());
    for (unsigned i = 0; i < vec.size(); ++i) {
        f << vec[i] << std::endl;
    }

    f.close();
}

template <typename vectortype>
void graph_io::readVector(std::vector<vectortype>& vec, std::string filename) {
    std::string line;

    // open file for reading
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "Error opening vectorfile" << filename << std::endl;
        return;
    }

    unsigned pos = 0;
    std::getline(in, line);
    while (!in.eof()) {
        if (line[0] == '%') {  // Comment
            continue;
        }

        vectortype value = (vectortype)atof(line.c_str());
        vec[pos++] = value;
        std::getline(in, line);
    }

    in.close();
}

#endif /*WIS_GRAPHIO_H_*/
