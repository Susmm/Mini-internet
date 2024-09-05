/*
Implement a simplified version of the OSPF (Open Shortest Path First) routing protocol in C/C++. 
The goal of this assignment is to build a routing table for a network of routers and simulate the routing of packets between them.

Specifications:
1. Implement a Router class in C/C++ with the following attributes:
    ● router_id: A unique identifier for the router (an integer).
    ● neighbours: A list of neighbouring routers, represented as pointers to other Router objects.
    ● routing_table: A data structure to store routing information.
    ● add_neighbor(Router* neighbour): A method to add a neighbouring router to the current router's list of neighbours.
    ● update_routing_table(): A method to calculate the routing table for the current router using the OSPF algorithm (For demonstration purposes, 
    you can assume static routing here)
    ● print_routing_table(): A method to print the routing table for the current router.
2. Implement the OSPF algorithm within the update_routing_table() method to compute the shortest path to each router in the network using 
   Dijkstra's algorithm or a similar approach.
3. Create a network of routers and connect them by adding neighbours to each router.
4. Simulate the routing of packets from one router to another by calling the update_routing_table() method and then using the routing table to 
   determine the path.
5. Print the routing table for each router to demonstrate the routing information.
*/
/*
N = 9 E = 14
0 1 4
0 7 8
1 7 11
1 2 8
7 8 7
7 6 1
2 8 2
8 6 6
2 3 7
2 5 4
6 5 2
3 5 14
3 4 9
5 4 10

*/
#include<bits/stdc++.h>
using namespace std;

int num_of_Routers;
int num_of_edges;
void dijkstra(int);

class Router{
   public :

   int router_id;
   vector<Router*> neighbours;
   unordered_map<int,int> routing_table;
   unordered_map<int,vector<int>> routing_path;
   
   void add_neighbor(Router* neighbour, int cost){
      neighbours.push_back(neighbour);
      routing_table[neighbour->getId()] = cost;
   }
   void update_routing_table(){
         dijkstra(router_id);
   }
   void print_routing_table(){
      cout << "\nRouting Table of Node " << router_id << " :\nRouterId \t cost \t path" << endl;
      for(auto it : routing_table){
         cout << it.first << "\t\t" << it.second << "\t" << router_id << "-->";
         for(auto nexthop : routing_path[it.first]){
            cout << nexthop ;
            if(nexthop != it.first) cout << "-->";
         }
         cout << endl;
      }
   }

   void setId(int _id){
      router_id = _id;
   }

   int getId(){
      return router_id;
   }

   int getCost(int neigId){
      return routing_table[neigId];
   }

};
Router router[100];

int minDistance(vector<int> &dist, vector<bool> &sptSet)
{
	int min = INT_MAX, min_index;

	for (int v = 0; v < num_of_Routers; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}

void dijkstra(int src){
   vector<int> dist(num_of_Routers,INT_MAX);
   vector<bool> sptSet(num_of_Routers,false);
   dist[src] = 0;

   for(int count = 0; count < num_of_Routers - 1; count++){
      int u = minDistance(dist, sptSet);
      sptSet[u] = 1;
      
      for(auto it : router[u].neighbours){
         int v = it->getId();
         int curr_cost = router[u].getCost(v);
         if (!sptSet[v] and dist[u] != INT_MAX and dist[u] + curr_cost < dist[v]){
               dist[v] = dist[u] + curr_cost;
               router[src].routing_path[v] = router[src].routing_path[u];
               router[src].routing_path[v].push_back(v);
            }	
         } 
      }

   for(int i = 0; i < num_of_Routers; i++){
      if(i == src or dist[i] == INT_MAX) continue;
      router[src].routing_table[i] = dist[i];
   }
}


int main(){
   // configure_network();
      cout << "Enter number of routers in the network : ";      cin >> num_of_Routers;  
      cout << "Enter number of egdes (connections) : ";         cin >> num_of_edges;
      // Router router[num_of_Routers];

      for(int i = 0; i < num_of_Routers; i++){
         router[i].setId(i);
      }
      
      cout << "Enter edges and their cost in the bolow format :\nnode[u] node[v] cost\n";
      for(int i = 0; i < num_of_edges; i++){
         int u, v, w;
         cin >> u >> v >> w;
         if(u==v){
         	i--;
         	cout << "\nIndistinct neighbour!\n";
         	continue;
         }
         if(u>=num_of_Routers || v>=num_of_Routers){
         	i--;
         	cout << "\nInvalid router id!\n";
         	continue;
         }
         router[u].add_neighbor(&router[v],w);
         router[v].add_neighbor(&router[u],w);
      }

   // for(int i = 0; i < num_of_Routers; i++){
   //    cout << "Neighbours of node " << i << " : ";
   //    for(auto it : router[i].neighbours){
   //       cout << it->getId() << " ";
   //    }
   //    cout << endl;
   // }

   int src,dest;
   
   cout << "\nSimulating packet routing\n\nEnter source node: ";
   
   cin >> src;
   
   cout << "Enter destination node: ";
   
   cin >> dest;
   		 
   router[src].update_routing_table();
   router[src].print_routing_table();
   
   if(router[src].routing_table.find(dest) == router[src].routing_table.end()){
   		cout << "\nDestination node unreachable!\n" ;
   		return 0;
   }
   
   for(int i = 0; i < router[src].routing_path[dest].size(); i++){
      router[router[src].routing_path[dest][i]].update_routing_table();
      router[router[src].routing_path[dest][i]].print_routing_table();
   }
   cout<<"\nPacket transmitted successfully..\n";
   
   return 0;
}
