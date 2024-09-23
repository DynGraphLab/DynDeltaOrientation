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
#include "dyn_edge_orientation_KFlips.h"
#include "dyn_edge_orientation_Path.h"
#include "dyn_edge_orientation_BFSCS.h"
#include "dyn_edge_orientation_RWalk.h"
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

        if( config.num_queries ) {
                // actually do some queries
                srand(config.seed);
                random_functions::setSeed(config.seed);
                std::vector< std::pair< NodeID, NodeID > > queries;
                for( unsigned i = 0; i < config.num_queries; i++) {
                        NodeID u = random_functions::nextInt(0, n-1);
                        NodeID v = random_functions::nextInt(0, n-1);
                        while( v == u ) {
                                v = random_functions::nextInt(0, n-1);
                        }
                        std::pair< NodeID, NodeID > p;
                        p.first = u;
                        p.second = v;

                        queries.push_back(p);
                }
                t.restart();

                int num = 0;

                for( unsigned i = 0; i < config.num_queries; i++) {
                        num += algorithm->adjacent(queries[i].first, queries[i].second);
                }

                double elapsed_per_query = t.elapsed()/config.num_queries;
                std::cout <<  "time per query  \t" << fixed << setprecision(20) <<  elapsed_per_query << std::endl;


                srand(config.seed);
                random_functions::setSeed(config.seed);
                std::vector< std::pair< NodeID, NodeID > > queries_adjcent;
                for( unsigned i = 0; i < config.num_queries; i++) {
                        queries_adjcent.push_back(edge_sequence[random_functions::nextInt(0, edge_sequence.size()-1)].second);
                }
                t.restart();

                int num2 = 0;

                for( unsigned i = 0; i < config.num_queries; i++) {
                        num2 += algorithm->adjacent(queries_adjcent[i].first, queries_adjcent[i].second);
                }

                elapsed_per_query = t.elapsed()/config.num_queries;
                std::cout <<  "t*ime per adjqry \t" << fixed << setprecision(20) <<  elapsed_per_query << std::endl;
        
                std::cout <<  "hashnum " <<  num  << std::endl;
                std::cout <<  "hashadj " <<  num2  << std::endl;

        
        }
        //// store orientation in file
        graph_access final_orientation;
        G->convert_to_graph_access(final_orientation);
        DELTAORI_ASSERT(G->number_of_edges() == final_orientation.number_of_edges());
        if( config.output_filename.size() != 0) {
                std::cout <<  "writing orientation to " <<  config.output_filename << std::endl;
                graphIO.writeDirectedGraph(final_orientation, config.output_filename);
        }


        return 0;
}
