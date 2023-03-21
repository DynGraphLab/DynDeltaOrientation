//
//

#ifdef USEILP
#include <algorithm>
#include <iostream>
#include <memory>

#include "ilp.h"
#include "DeltaOrientationsConfig.h"
#include "dyn_graph_access.h"
#include "graph_access.h"
#include "gurobi_c++.h"

void solveUsingILP(graph_access& orientation, dyn_graph_access& G, const DeltaOrientationsConfig& config) {
    std::cout <<  "Solving using ILP"  << std::endl;
    try {
        auto env = std::make_shared<GRBEnv>();
        GRBModel model = GRBModel(*env);

        // set model
        model.set(GRB_StringAttr_ModelName, "Minimizing Out-Degree");
        model.set(GRB_DoubleParam_TimeLimit, config.time_limit);
        model.set(GRB_DoubleParam_MIPGap, 0);

        // set decision variables for edges
        std::vector<GRBVar> edges(2*G.number_of_edges());
        for (auto& edgeVar : edges) {
            edgeVar = model.addVar(0.0, 1.0, 0, GRB_BINARY);
        }

        graph_access G_undirected;
        G.convert_to_graph_access(G_undirected);

        // add constraints
        GRBVar phi = model.addVar(0.0, 1.0, 0, GRB_INTEGER);  // std::numeric_limits<NodeID>::max();
        phi.set(GRB_DoubleAttr_UB, G.maxDegree());

        for (NodeID node = 0; node < G.number_of_nodes(); ++node) {
            GRBLinExpr outDeg = 0;
            forall_out_edges (G_undirected, e, node) {
                outDeg += edges[e];  // sum up potential outgoing edges
                if (node < G_undirected.getEdgeTarget(e)) {
                    NodeID target = G_undirected.getEdgeTarget(e);
                    forall_out_edges (G_undirected, rev_e, target) {
                        if (G_undirected.getEdgeTarget(rev_e) == node) {

                            GRBLinExpr oneDirectedEdgeExist = edges[e] + edges[rev_e];
                            model.addConstr(oneDirectedEdgeExist == 1, "either directed or reversed edge must exist");
                            break;
                        }
                    }
                    endfor
                }
            }
            model.addConstr(outDeg <= phi, "outDeg is bounded by phi");
            endfor
        }

        // set objective
        model.setObjective(GRBLinExpr(phi), GRB_MINIMIZE);

        // optimize model
        model.optimize();

        // set solution
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            orientation.start_construction(G_undirected.number_of_nodes(), G_undirected.number_of_edges());
            for (NodeID node = 0; node < G_undirected.number_of_nodes(); ++node) {
                orientation.new_node();
                forall_out_edges (G_undirected, e, node) {
                    if (round(edges[e].get(GRB_DoubleAttr_X)) == 1) {
                        orientation.new_edge(node, G_undirected.getEdgeTarget(e));
                    }
                }
                endfor
            }
            orientation.finish_construction();
            std::cout << "solution of ILP: " << phi.get(GRB_DoubleAttr_X) << std::endl;
        } else {
            std::cout << "No solution found by ILP solver" << std::endl;
            exit(EXIT_FAILURE);
        }

    } catch (GRBException& e) {
        std::cout <<  "Oooops something bad happend with Gurobi: " << e.getMessage() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void solveUsingRelaxedILP(graph_access& orientation, dyn_graph_access& G, const DeltaOrientationsConfig& config) {
    try {
        // Todo a bit hacky but it allows us to easier map between decision variables and edges in AdjArray
        //  because we get an contiguously edge vector
        std::cout <<  "TODO update"  << std::endl;
        exit(0);
        graph_access GStatic;
        G.convert_to_graph_access(GStatic);

        auto env = std::make_shared<GRBEnv>();
        GRBModel model = GRBModel(*env);

        // define variables
        // for each (edge,endpoint) a variable is needed for setting orientation
        std::vector<GRBVar> edges(GStatic.number_of_edges());

        // set model
        model.set(GRB_StringAttr_ModelName, "Minimizing Out-Degree");
        model.set(GRB_DoubleParam_TimeLimit, config.time_limit);
        model.set(GRB_DoubleParam_MIPGap, 0);

        // set decision variables
        for (auto& edgeVar : edges) {
            edgeVar = model.addVar(0.0, 1.0, 0, GRB_CONTINUOUS);
        }

        // add constraints
        GRBVar maxOutDeg = model.addVar(0.0, 1.0, 0, GRB_CONTINUOUS);  // std::numeric_limits<NodeID>::max();
        // ToDo if we observe a performance decrease
        //  we maybe need to determine a better upper bound for maxOutDeg
        //  e.g. determine current maxDegree
        maxOutDeg.set(GRB_DoubleAttr_UB, G.maxDegree());

        DELTAORI_LOG("nodes: " << G.number_of_nodes());
        for (NodeID node = 0; node < GStatic.number_of_nodes(); ++node) {
            GRBLinExpr outDeg = 0;
            NodeID currentOutDeg = 0;
            forall_out_edges (GStatic, e, node) {
                // Todo This now the hacky thing: e is also the position in edges
                outDeg += edges[e];  // sum up potential outgoing edges
                ++currentOutDeg;
                if (node < GStatic.getEdgeTarget(e)) {
                    // edges[e].set(GRB_Double)
                    auto target = GStatic.getEdgeTarget(e);
                    forall_out_edges (GStatic, rev_e, target) {
                        if (GStatic.getEdgeTarget(rev_e) == node) {
                            GRBLinExpr oneDirectedEdgeExist = edges[e] + edges[rev_e];
                            DELTAORI_DEBUG(DELTAORI_LOG("add oneDirectedEdgeExist bound"));
                            model.addConstr(oneDirectedEdgeExist == 1, "either directed or reversed edge must exist");
                            break;
                        }
                    }
                    endfor
                }
            }
            DELTAORI_DEBUG(DELTAORI_LOG("add outDeg bound"));
            model.addConstr(outDeg <= maxOutDeg, "outDeg is bounded by maxOutDeg");
            endfor
        }

        // set objective
        model.setObjective(GRBLinExpr(maxOutDeg), GRB_MINIMIZE);

        // optimize model
        model.optimize();

        // set solution
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL || model.get(GRB_IntAttr_Status) == GRB_TIME_LIMIT) {
            DELTAORI_LOG("found ILP solution");
            orientation.start_construction(GStatic.number_of_nodes(), GStatic.number_of_edges() / 2);
            DELTAORI_DEBUG(NodeID maxFoundOutDeg = 0;);
            // Todo set oriented edges
            for (NodeID node = 0; node < GStatic.number_of_nodes(); ++node) {
                orientation.new_node();
                DELTAORI_DEBUG(NodeID outDeg = 0;);
                forall_out_edges (GStatic, e, node) {
                    // create the oriented edge if model decided this way
                    // DELTAORI_LOG(edges[e].get(GRB_DoubleAttr_X));
                    if (round(edges[e].get(GRB_DoubleAttr_X)) == 1) {
                        orientation.new_edge(node, GStatic.getEdgeTarget(e));
                        DELTAORI_DEBUG(++outDeg;);
#ifndef DELTAORI_NDEBUG
                        forall_out_edges (GStatic, rev_e, GStatic.getEdgeTarget(e)) {
                            if (GStatic.getEdgeTarget(rev_e) == node) {
                                DELTAORI_ASSERT(edges[rev_e].get(GRB_DoubleAttr_X) == 0.5 ||round(edges[rev_e].get(GRB_DoubleAttr_X)) == 0);
                                break;
                            }
                        }
                        endfor
#endif
                    }
                }
                endfor
                DELTAORI_DEBUG(if (outDeg > maxFoundOutDeg) { maxFoundOutDeg = outDeg; });
            }
            // DELTAORI_LOG("max found out deg: " << maxFoundOutDeg);
            DELTAORI_ASSERT(maxOutDeg.get(GRB_DoubleAttr_X) == maxFoundOutDeg);
            orientation.finish_construction();

            //DELTAORI_LOG("MINIMUM maxOutDegree (ILP): " << maxOutDeg.get(GRB_DoubleAttr_X));
            std::cout << "maxOutDegree (ILP Relaxed): " << maxOutDeg.get(GRB_DoubleAttr_X) << std::endl;
        } else {
            std::cout << "No solution found by ILP solver" << std::endl;
            exit(EXIT_FAILURE);
        }

    } catch (GRBException& e) {
        exit(EXIT_FAILURE);
    }
}

#endif
