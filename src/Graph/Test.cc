
/**
 * genLinearGraph - generate a graph that is a linklist
 * Example : 1 -> 2 -> 3 -> ...
 */ 
// Graph* genLinearGraph(int p_size) {
//   Graph* g = new Graph();
//   int *c;
//   Node *n1;
//   Node *n2;
  
//   for(int i = 0; i < p_size; i++) {
//     c = new int();
//     *c = i;
    
//     n1 = g->new_node(c);
    
//     if(i != 0) {
//       g->new_edge(n2, n1);
//     }
//     n2 = n1;
//   }
  
//   cout << " # Nodes " << g->num_nodes() << endl;
//   cout << " # Edges " << g->num_edges() << endl;
  
//   return g;
// }


// void testLinearGraph() {
//   Graph* g = genLinearGraph(200);
//   g->name = "easy200";
//   NodeIterator iter = g->nodes_begin();
//   Node* n1 = (*iter++);
//   Node* n2 = (*iter);
//   cout << " number of nodes " << g->num_nodes() << endl;
//   cout << " degree of node " << n1->getKey() << " is " << g->degree(n1) << endl;
//   cout << " degree of node " << n2->getKey() << " is " << g->degree(n2) << endl;

//   list< Edge* > edges = g->incident_edges(n1);
//   Edge *inc_edge;

//   cout << " edges and nodes adjacent to node " << n1->getKey() << endl;

//   for(EdgeIterator iter = edges.begin(); iter != edges.end(); ++iter) {
//     inc_edge = *iter;
//     cout << " edge incident is " << inc_edge->getKey() << endl;
//     Node *adj_node = g->opposite(n1, inc_edge);
//     cout << " degree of node " << adj_node->getKey() << " is " << g->degree(adj_node) << endl;
//   }


//   list< Node* > nodes = g->adjacent_nodes(n2);
//   cout << " nodes adjacent to node " << n2->getKey() << endl;

//   for(NodeIterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
//     cout << " degree of node " << (*iter)->getKey() << " is " << g->degree((*iter)) << endl;
//   }

//   g->create_dot_file("test.dot");
// }


// void testGraph() {
//   Graph g("Test_Graph");

//   Node* n1 = g.new_node();
//   Node* n2 = g.new_node();
//   Edge* e1 = g.new_edge(n2, n1);
//   Node* n3 = g.new_node();
//   Edge* e2 = g.new_edge(n3, n2);
//   Edge* e3 = g.new_edge(n1, n3);

//   cout << " number of nodes " << g.num_nodes() << endl;
//   cout << " degree of node " << n1->getKey() << " is " << g.degree(n1) << endl;
//   cout << " degree of node " << n2->getKey() << " is " << g.degree(n2) << endl;

//   list< Edge* > edges = g.incident_edges(n1);
//   Edge *inc_edge;

//   cout << " edges and nodes adjacent to node " << n1->getKey() << endl;

//   for(EdgeIterator iter = edges.begin(); iter != edges.end(); ++iter) {
//     inc_edge = *iter;
//     cout << " edge incident is " << inc_edge->getKey() << endl;
//     Node *adj_node = g.opposite(n1, inc_edge);
//     cout << " degree of node " << adj_node->getKey() << " is " << g.degree(adj_node) << endl;
//   }


//   list< Node* > nodes = g.adjacent_nodes(n2);
//   cout << " nodes adjacent to node " << n2->getKey() << endl;

//   for(NodeIterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
//     cout << " degree of node " << (*iter)->getKey() << " is " << g.degree((*iter)) << endl;
//   }

//   g.create_dot_file("test.dot");

// }
