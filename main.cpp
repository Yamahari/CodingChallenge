#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

using EdgeWeightProperty = boost::property<boost::edge_weight_t, double>;
using UndirectedGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS,boost::no_property, EdgeWeightProperty>; 
using VertexDescriptor = boost::graph_traits<UndirectedGraph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<UndirectedGraph>::edge_descriptor;

int main() {
    pt::ptree p;
    pt::json_parser::read_json("generatedGraph.json", p);

    auto& nodes = p.get_child("nodes");
    auto& edges = p.get_child("edges");

    std::ptrdiff_t d0;
    std::ptrdiff_t d1;

    // Find the start node "Erde" and the end node "b3-r7-r4nd7"
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if(node->second.get_child("label").get_value<std::string>() == "Erde") {
            d0 = std::distance(nodes.begin(), node);
        } else if(node->second.get_child("label").get_value<std::string>() == "b3-r7-r4nd7") {
            d1 = std::distance(nodes.begin(), node);
        }
    }

    // Initialize undirected graph
    UndirectedGraph g;
    for(auto& edge : edges) {
        boost::add_edge(
            edge.second.get_child("source").get_value<int>(),
            edge.second.get_child("target").get_value<int>(),
            edge.second.get_child("cost").get_value<double>(), 
            g);
    }
    VertexDescriptor earth = boost::vertex(d0, g);
    VertexDescriptor destiny = boost::vertex(d1, g);

    // Run dijkstra's shortest path algorithm
    std::vector<VertexDescriptor> predecessors(boost::num_vertices(g));
    std::vector<double> distances(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, earth, boost::predecessor_map(&predecessors[0]).distance_map(&distances[0]));

    double totalDistance = distances[destiny];

    // Backtrace and add start vertex
    std::vector<VertexDescriptor> vertices;
    do {
        vertices.push_back(destiny);
        destiny = predecessors[destiny];
    } while(destiny != earth);
    vertices.push_back(earth);

    // Print out vertices in reverse order and total distance
    for(auto vertex = vertices.rbegin(); vertex != vertices.rend(); ++vertex) {
        std::cout << *vertex << '\n';
    }
    std::cout << "totalDistance: " << totalDistance << std::endl;
}
