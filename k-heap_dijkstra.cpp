#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <utility>
#include <cmath>

class k_heap {
private:
  int* heap_arr;
  int* positions; 
  std::vector<int> dist; 
  int capacity;
  int current_size;
  int k;

  void swap(int i, int j) {
    int temp = heap_arr[i];
    heap_arr[i] = heap_arr[j];
    heap_arr[j] = temp;

    positions[heap_arr[i]] = i;
    positions[heap_arr[j]] = j;
  }

  int parent(int i) const {
    return (i - 1) / k;
  }
  
  int child(int i, int j) const {
    return k * i + j + 1;
  }

  bool is_leaf(int i) {
    return child(i, 0) >= current_size;
  }

  void heapify_up(int i) {
    if (root(i)) return;

    if (key(parent(i)) > key(i)) {
      swap(i, parent(i));
      heapify_up(parent(i));
    }
  }

  void heapify_down(int i) {
    if (is_leaf(i)) return;

    int smallest_child = child(i, 0);
    for (int j = 1; j < k; j++) {
      int child_idx = child(i, j);
      if (child_idx < current_size && key(child_idx) < key(smallest_child)) {
        smallest_child = child_idx;
      }
    }

    if (key(smallest_child) < key(i)) {
      swap(i, smallest_child);
      heapify_down(smallest_child);
    }
  }

public:
  k_heap(int cap, int k_value) {
    if (k_value < 2) {
      throw std::invalid_argument("k must be at least 2");
    }
    capacity = cap;
    heap_arr = new int[capacity];
    positions = new int[capacity];
    dist.resize(capacity, std::numeric_limits<int>::max());
    current_size = 0;
    k = k_value; 

    for (int i = 0; i < capacity; i++) {
      positions[i] = -1;
    }
  }

  ~k_heap() {
    delete[] heap_arr;
    delete[] positions;
  }

  int key(int i) {
    return dist[heap_arr[i]];
  }

  bool root(int i) {
    return i == 0;
  }

  void insert(int vertex) {
    heap_arr[current_size] = vertex;
    positions[vertex] = current_size;
    heapify_up(current_size);
    current_size++;
  }

  void deleteKey(int i) {
    if (i >= current_size) {
      throw std::out_of_range("Index out of range");
    }

    int vertex_to_remove = heap_arr[i];
    heap_arr[i] = heap_arr[current_size - 1];
    positions[heap_arr[i]] = i;
    positions[vertex_to_remove] = -1;
    current_size--;

    if (i > 0 && key(i) < key(parent(i))) {
      heapify_up(i);
    } else {
      heapify_down(i);
    }
  }

  void update(int vertex, int new_value) {
    
    if (positions[vertex] == -1) {
      dist[vertex] = new_value;
      insert(vertex);
    } else {
      int old_value = dist[vertex];
      dist[vertex] = new_value;
      
      int i = positions[vertex];
      if (new_value < old_value) {
        heapify_up(i);
      } else if (new_value > old_value) {
        heapify_down(i);
      }
    }
  }

  void deleteMin() {
    if (current_size <= 0) {
      throw std::runtime_error("Heap is empty");
    }
    deleteKey(0);
  }

  int getMin() {
    if (current_size <= 0) {
      throw std::runtime_error("Heap is empty");
    }
    return heap_arr[0];
  }

  bool isEmpty() {
    return current_size == 0;
  }

  int size() {
    return current_size;
  }
  
  int getDistance(int vertex) {
    return dist[vertex];
  }
  
  void setDistance(int vertex, int distance) {
    dist[vertex] = distance;
  }
  
  bool isInHeap(int vertex) {
    return positions[vertex] != -1;
  }
};

typedef std::vector<std::vector<std::pair<int, int>>> graph;


/*Based on  https://github.com/mrpritt/Caminho_Mais_Curto/blob/main/src/leia.cpp*/
void read_dimacs(std::istream& in, unsigned& n, unsigned& m, graph& a) {
  std::string line = "", dummy;
  while (line.substr(0, 4) != "p sp")
    getline(in, line);
 
  // (1) get nodes and edges
  std::stringstream linestr;
  linestr.str(line);
  linestr >> dummy >> dummy >> n >> m;
  a.resize(n + 1);
  unsigned i = 0;
  while (i++ < m) {
    getline(in, line);
    if (line.substr(0, 2) == "a ") {
      std::stringstream arc(line);
      unsigned u, v, w;
      char ac;
      arc >> ac >> u >> v >> w;
      // processa arco (u,v) com peso w
      a[u].push_back(std::make_pair(v, w));
    }
  }
}

int dijkstra(const graph& graph, int src, int dest, int k_value) {
  int n = graph.size() - 1;
  
  k_heap Q(n + 1, k_value);
  
  for (int v = 1; v <= n; v++) {
    Q.setDistance(v, std::numeric_limits<int>::max());
  }
  Q.setDistance(src, 0);
  
  std::vector<bool> visited(n + 1, false);
  
  Q.insert(src);
  
  while (!Q.isEmpty()) {
    int v = Q.getMin();
    Q.deleteMin();
    
    visited[v] = true;
    
    if (v == dest) {
      return Q.getDistance(v);
    }
    
    for (const auto& edge : graph[v]) {
      int u = edge.first;
      int dvu = edge.second;
      
      if (!visited[u]) {
        int dv = Q.getDistance(v);
        int du = Q.getDistance(u);
        
        if (du == std::numeric_limits<int>::max()) {
          Q.setDistance(u, dv + dvu);
          Q.insert(u);
        }
        else if (dv + dvu < du) {
          Q.update(u, dv + dvu);
        }
      }
    }
  }
  return -1;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " source destination [k-value]" << std::endl;
    return 1;
  }
  
  int source = std::stoi(argv[1]);
  int destination = std::stoi(argv[2]);
  int k_value = 2; 
  
  if (argc == 4) {
    k_value = std::stoi(argv[3]);
  }
  
  unsigned n, m;
  graph adj_list;
  read_dimacs(std::cin, n, m, adj_list);
  
  int shortestDistance = dijkstra(adj_list, source, destination, k_value);

  if(shortestDistance == -1) {std::cout << "inf";} else {std::cout<<shortestDistance;}
  
  return 0;
}
