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
    // Parse the given .json 
    pt::ptree p;

    try {
        pt::json_parser::read_json("generatedGraph.json", p);
    } catch(const pt::json_parser_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Get nodes and edges
    pt::ptree* nodes = nullptr;
    pt::ptree* edges = nullptr;

    try {
        nodes = &p.get_child("nodes");
        edges = &p.get_child("edges");
    } catch(const pt::ptree_bad_path& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize undirected graph
    UndirectedGraph g;
    for(auto& edge : *edges) {
        try {
        boost::add_edge(
            edge.second.get_child("source").get_value<VertexDescriptor>(),
            edge.second.get_child("target").get_value<VertexDescriptor>(),
            edge.second.get_child("cost").get_value<double>(), 
            g);
        } catch(const pt::ptree_error& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Find the start node "Erde" and the end node "b3-r7-r4nd7"
    auto earth = std::make_pair(false, std::numeric_limits<VertexDescriptor>::max());
    auto destination = std::make_pair(false, std::numeric_limits<VertexDescriptor>::max());

    for(auto node = nodes->begin(); node != nodes->end(); ++node) {
        std::string label;

        // Get the label of the node
        try {
            label = node->second.get_child("label").get_value<std::string>();
        } catch(const pt::ptree_error& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        // Check if it's equal to "Erde" or "b3-r7-r4nd7"
        if(label == "Erde") {
            earth.first = true;
            earth.second = boost::vertex(std::distance(nodes->begin(), node), g);
        } else if(label == "b3-r7-r4nd7") {
            destination.first = true;
            destination.second = boost::vertex(std::distance(nodes->begin(), node), g);
        }

        // Break loop if "Erde" and "b3-r7-r4nd7" have been found
        if(earth.first && destination.first) break;
    }

    // Check if both nodes have been found
    if(!earth.first || !destination.first) {
        std::cerr << std::boolalpha 
            << "In dieser Galaxie gibt es einen oder mehreren der gesuchten Planten nicht. Erde[" 
            << earth.first << "], b3-r7-r4nd7[" 
            << destination.first << "]" 
            << std::endl;
        return EXIT_FAILURE;
    }

    // Run dijkstra's shortest path algorithm
    std::vector<VertexDescriptor> predecessors(boost::num_vertices(g));
    std::vector<double> distances(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, earth.second, boost::predecessor_map(predecessors.data()).distance_map(distances.data()));

    auto totalDistance = distances[destination.second];

    // Backtrace and add start vertex
    std::vector<VertexDescriptor> vertices;
    do {
        vertices.push_back(destination.second);
        destination.second = predecessors[destination.second];
    } while(destination.second != earth.second);
    vertices.push_back(earth.second);

    // Print out vertices in reverse order and total distance
    for(auto vertex = vertices.rbegin(); vertex != vertices.rend(); ++vertex) {
        std::cout << *vertex << '\n';
    }
    std::cout << "totalDistance: " << totalDistance << std::endl;
}
