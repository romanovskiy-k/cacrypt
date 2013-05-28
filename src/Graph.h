#define MAX_LINE_LENGTH 500000

class Graph {
	unsigned int num_verts;
	unsigned int num_edges;
	unsigned int adj_list_length;
	unsigned int *edge_offsets;
	unsigned int *edge_list;
	unsigned int max_degree;
	int graph_type;

	bool if_delete_arrays;
public:
	Graph();
	~Graph();
	void LoadMetisGraph(const char *filename);
	void SaveMetisGraph(const char *filename);
	unsigned int GetNumVertices();
	unsigned int GetNumEdges();

	unsigned int *GetEdgeOffsets();
	unsigned int *GetEdgeList();

	unsigned int * *GetEdgeOffsetsPtr();
	unsigned int * *GetEdgeListPtr();

	unsigned int *GetVertexLengths(unsigned int *cost, unsigned int source);
	int GetMetisGraphType();
	unsigned int GetAdjacencyListLength();
};
