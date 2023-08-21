//
//

#include <random_functions.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "parse_parameters.h"
#include "DynamicAlgorithmMeta.h"
#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "dyn_edge_orientation_BFSCS.h"
#include "dyn_edge_orientation_ILP.h"
#include "algorithm_factory.h"
#include "graph_io.h"
#include "timer.h"
#include "ilp.h"

int main(int argc, char** argv) {


        DeltaOrientationsConfig config;

        std::string graph_filename;
        int ret_code = parse_parameters(argc, argv, config, graph_filename);

        if(ret_code) {
                return 0;
        }


        srand(config.seed);
        random_functions::setSeed(config.seed);

        // read sequence
        auto graphIO = graph_io();
        timer t; t.restart();

        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence; 
        std::pair<int,int> p = graphIO.readEdgeSequence( graph_filename, edge_sequence);
        int n = p.first;
        std::cout <<  "io took " <<  t.elapsed()  << std::endl; 

        //configure algorithm based on input parameters
        std::shared_ptr<dyn_graph_access> G = std::make_shared<dyn_graph_access>(n);
        DeltaOrientationsResult res;
        std::shared_ptr<dyn_edge_orientation> algorithm = getdyn_edge_orientation_instance(config.algorithmType, G, config, res);

        t.restart();
        // run algorithm
        if(algorithm != NULL) 
                run_dynamic_algorithm(G, edge_sequence, algorithm);

        std::cout <<  "time \t\t " <<  t.elapsed()  << std::endl;

        //call some postprocessing
        algorithm->end();

        std::cout <<  "maxOutDegree \t " <<  G->maxDegree()  << std::endl;

        //// store orientation in file
        graph_access final_orientation;
        G->convert_to_graph_access(final_orientation);
        if( config.output_filename.size() != 0) {
                std::cout <<  "writing orientation to " <<  config.output_filename << std::endl;
                graphIO.writeDirectedGraph(final_orientation, config.output_filename);
        }


        return 0;
}
