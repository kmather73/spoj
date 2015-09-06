#include <iostream>
#include <queue>
#include <set>

using std::cout; 
using std::cin;
using std::priority_queue;
using std::set;

class ISS;

class Graph
{
	public:
		Graph(int num_vertices);
		~Graph();
		int get_num_vertices() const;
		int get_weight(int vertex) const;
		void set_weight(int vertex, int weight);
		void add_edge(int v1, int v2);
		bool are_neighbors(int v1, int v2) const;
		int max_iss_weight() const;

	private:
		int _num_vertices;
		int *_weights;

		// Lookup matrix -- [v1][v2] = are v1 and v2 neighbors?
		bool **_neighbors;
};

class ISS
{
	public:
		ISS(const Graph *graph);
		ISS(const ISS &copy);
		~ISS();
		bool operator<(const ISS &iss) const;
		void add_vertex(int vertex);
		int get_weight() const;
		int theoretical_max_weight() const;
		int next_unchecked_vertex() const;
		bool can_make_superset(int vertex) const;
		void push_supersets(priority_queue<ISS*> &queue) const;

	private:
		const Graph *_graph;
		set<int> _vertices;
		int *_supersets;
};

Graph::Graph(int num_vertices)
{
	this->_num_vertices = num_vertices;
	this->_weights = new int[num_vertices + 1];
	this->_neighbors = new bool*[num_vertices + 1];

	for (int i = 1; i <= num_vertices; i++)
	{
		this->_neighbors[i] = new bool[num_vertices + 1];

		for (int j = 1; j <= this->_num_vertices; j++)
		{
			this->_neighbors[i][j] = false;
		}
	}
}

Graph::~Graph()
{
	delete this->_weights;

	for (int i = 1; i <= this->_num_vertices; i++)
	{
		delete this->_neighbors[i];
	}

	delete this->_neighbors;
}

int Graph::get_num_vertices() const
{
	return this->_num_vertices;
}

int Graph::get_weight(int vertex) const
{
	return this->_weights[vertex];
}

void Graph::set_weight(int vertex, int weight)
{
	this->_weights[vertex] = weight;
}

void Graph::add_edge(int v1, int v2)
{
	this->_neighbors[v1][v2] = true;
	this->_neighbors[v2][v1] = true;
}

bool Graph::are_neighbors(int v1, int v2) const
{
	return this->_neighbors[v1][v2];
}

int Graph::max_iss_weight() const
{
	// Track ISS's that might be subsets of larger ISS's
	priority_queue<ISS*> unchecked;
	int best = 0;

	// Start with the trivial ISS's -- single vertices
	for (int vertex = 1; vertex <= this->_num_vertices; vertex++)
	{
		ISS *iss = new ISS(this);
		iss->add_vertex(vertex);

		int weight = iss->get_weight();

		if (weight > best)
		{
			best = weight;
		}

		unchecked.push(iss);
	}

	// Heuristic search based on largest theoretical weight
	while (!unchecked.empty())
	{
		ISS *iss = unchecked.top();
		int weight = iss->get_weight();

		unchecked.pop();

		if (iss->get_weight() > best)
		{
			best = weight;
		}

		if (iss->theoretical_max_weight() > best)
		{
			iss->push_supersets(unchecked);
		}

		delete iss;
	}

	return best;
}

ISS::ISS(const Graph *graph) : _vertices()
{
	this->_graph = graph;
	this->_supersets = new int[graph->get_num_vertices() + 1];

	for (int i = 1; i <= graph->get_num_vertices(); i++)
	{
		this->_supersets[i] = -1;
	}
}

ISS::ISS(const ISS &copy) : _vertices(copy._vertices)
{
	this->_graph = copy._graph;
	this->_supersets = new int[copy._graph->get_num_vertices() + 1];

	for (int i = 1; i <= copy._graph->get_num_vertices(); i++)
	{
		// Vertices we know won't make a superset from the subset also won't
		// make a superset from this set, but ones that did for the subset may
		// not for this set.
		this->_supersets[i] = copy._supersets[i] == 0 ? 0 : -1;
	}
}

ISS::~ISS()
{
	delete this->_supersets;
}

bool ISS::operator<(const ISS &iss) const
{
	return this->theoretical_max_weight() < iss.theoretical_max_weight();
}

void ISS::add_vertex(int vertex)
{
	this->_vertices.insert(vertex);
}

int ISS::get_weight() const
{
	int weight = 0;

	for (set<int>::iterator it = this->_vertices.begin(); 
	     it != this->_vertices.end(); it++)
	{
		weight += this->_graph->get_weight(*it);
	}

	return weight;
}

/**
 * Calculate the theoretical maximum weight of a superset of this ISS, assuming
 * all vertices greater than the largest vertex in this ISS have not been
 * checked yet.
 */
int ISS::theoretical_max_weight() const
{
	int possible = 0;
	int start = this->next_unchecked_vertex();

	for (int vertex = start; vertex <= this->_graph->get_num_vertices();
	     vertex++)
	{
		if (this->can_make_superset(vertex))
		{
			possible += this->_graph->get_weight(vertex);
		}
	}

	return this->get_weight() + possible;
}

/**
 * Assuming this ISS is a subset of a larger ISS, return the next vertex that
 * should be checked to see if a large ISS can be constructed.
 */
int ISS::next_unchecked_vertex() const
{
	// Sets are sorted highest-last
	return *this->_vertices.rbegin() + 1;
}

bool ISS::can_make_superset(int vertex) const
{
	if (this->_supersets[vertex] == -1)
	{
		bool result = true;

		for (set<int>::iterator it = this->_vertices.begin();
			 it != this->_vertices.end(); it++)
		{
			if (this->_graph->are_neighbors(*it, vertex))
			{
				result = false;
				break;
			}
		}

		// Cache result
		this->_supersets[vertex] = result;
	}

	return this->_supersets[vertex];
}

/**
 * Push supersets to the given priority queue under the same assumption as
 * next_unchecked_vertex().
 */
void ISS::push_supersets(priority_queue<ISS*> &queue) const
{
	for (int vertex = this->next_unchecked_vertex(); 
	     vertex <= this->_graph->get_num_vertices(); vertex++)
	{
		if (this->can_make_superset(vertex))
		{
			ISS *superset = new ISS(*this);
			superset->add_vertex(vertex);

			queue.push(superset);
		}
	}
}

int main()
{
	int num_cases;
	cin >> num_cases;

	for (int i = 0; i < num_cases; i++)
	{
		int num_vertices, num_edges;
		cin >> num_vertices >> num_edges;

		Graph g(num_vertices);

		for (int vertex = 1; vertex <= num_vertices; vertex++)
		{
			int weight;
			cin >> weight;

			g.set_weight(vertex, weight);
		}

		for (int j = 0; j < num_edges; j++)
		{
			int v1, v2;
			cin >> v1 >> v2;

			g.add_edge(v1, v2);
		}

		cout << g.max_iss_weight() << '\n';
	}

	return 0;
}
