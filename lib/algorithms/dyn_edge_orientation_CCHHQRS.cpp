#include "dyn_edge_orientation_CCHHQRS.h"
#include <cmath>
#include "random_functions.h"


dyn_edge_orientation_CCHHQRS::dyn_edge_orientation_CCHHQRS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                                           DeltaOrientationsResult& result)
    : dyn_edge_orientation(GOrientation, config, result) {
        m_adj.resize(GOrientation->number_of_nodes());
        vertices.resize(GOrientation->number_of_nodes());
        for (int i = 0; i < GOrientation->number_of_nodes(); i++){
                vertices[i].node = i;
                vertices[i].in_edges = Buckets(config, GOrientation->number_of_nodes());
                DEdge uu(&vertices[i], &vertices[i]);
                uu.mirror = NULL;
                edges.insert(make_pair(make_pair(i, i), uu));
                DEdge* new_uu = &edges.at(make_pair(i, i));
                vertices[i].self_loop = new_uu;
                add(new_uu);
        }
}

void dyn_edge_orientation_CCHHQRS::handleInsertion(NodeID source, NodeID target){
        if (source == target){ return; }
        Vertex *u = &vertices[source];
        Vertex *v = &vertices[target];
        // ignore existing edges (simple graphs only)
        if (edges.count(make_pair(source, target)) == 1){return;}
        if (edges.count(make_pair(target, source)) == 1){return;}
        // make the edges
        DEdge uv(u, v);
        DEdge vu(v, u);

        edges.insert(make_pair(make_pair(source, target), uv));
        edges.insert(make_pair(make_pair(target, source), vu));
        DEdge * new_uv = &edges.at(make_pair(source, target));
        DEdge * new_vu = &edges.at(make_pair(target, source));
        new_uv->mirror = new_vu;
        new_vu->mirror = new_uv;



        for (unsigned i = 0; i < config.b; i++){
                if (u->out_degree <= v->out_degree){
                        insert_directed(new_uv);
                }
                else{
                        insert_directed(new_vu);
                }
        }
}

void dyn_edge_orientation_CCHHQRS::handleDeletion(NodeID source, NodeID target){
        if (source == target){ return; }
        Vertex u = vertices[source];
        Vertex v = vertices[target];
        DEdge* uv = &edges.at(make_pair(source, target));
        DEdge* vu = &edges.at(make_pair(target, source));
        while (uv->count + vu->count > 0){
                if (uv->count >= vu->count){
                        delete_directed(uv);
                }
                else { delete_directed(vu); }
        }
}

void dyn_edge_orientation_CCHHQRS::insert_directed(DEdge *uv){ // NOLINT(*-no-recursion)
        add(uv);
        DEdge *uw_min = uv->source->out_edges.back();

        // find the out neighbour of minimum out degree
        for (DEdge* uw: uv->source->out_edges){
                // check threshod + ignore the self loop
                if (uw->target->out_degree < uw_min->target->out_degree && uw->target->node != uv->source->node){
                        uw_min = uw;
                }
        }

        if (uv->source->out_degree > std::max(config.b / 4, (1 + config.lambda) * uw_min->target->out_degree + config.theta)){
                remove(uw_min);
                insert_directed(uw_min->mirror);
        }
        else{
                for(DEdge* uw: uv->source->out_edges){
                        uw->target->in_edges.update(uw);
                }
        }
}

void dyn_edge_orientation_CCHHQRS::delete_directed(DEdge *uv){ // NOLINT(*-no-recursion)
        remove(uv);
        DEdge * ux = uv->source->in_edges.buckets[uv->source->in_edges.max_bucketID].bucket_elements.front();
        if (ux->target->out_degree > max(config.b / 4, (1 + config.lambda) * ux->source->out_degree + config.theta)){
                add(ux);
                delete_directed(ux->mirror);
        }
        else{
                for(DEdge* uw: uv->source->out_edges){
                        uw->target->in_edges.update(uw);
                }
        }
}

void dyn_edge_orientation_CCHHQRS::add(DEdge *uv){
        uv->source->out_degree++;
        uv->count++;
        if (uv->count == 1) {    // new edge
                uv->source->out_edges.push_back(uv);
                uv->location_out_neighbours = prev(uv->source->out_edges.end());
                // insert u in the in neighbours of v
                uv->target->in_edges.add(uv);
        }
}

void dyn_edge_orientation_CCHHQRS::remove(DEdge *uv){
        // decrement the multiplicity of the edge
        uv->count--;
        // if the new multiplicity is 0, remove the element.
        if (uv->count == 0){
                uv->source->out_edges.erase(uv->location_out_neighbours);
                uv->location_out_neighbours = uv->source->out_edges.end();
                uv->target->in_edges.remove(uv);
        }
        if (uv->count < 0){
                cerr << "Error: edge count below 0" << endl;
        }
        uv->source->out_degree--;
}