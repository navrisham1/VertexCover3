// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <list>
#include <cstdint>

/*****************************
  Graph class declaration
    Stores the graph information and prints the path if there is one
***************************/
class Graph
{
private:
    unsigned vertex_count=0;
    std::list<unsigned> *adjacency_list;

    // private helper to add an edge to the graph
    void addEdge(unsigned first, unsigned second);

public:
    Graph(unsigned vertex_count, std::vector< std::pair<unsigned,unsigned> > edges); // Constructor
    bool printPath(unsigned from, unsigned to); // checks if there is a path and prints it
};


/*****************************
  Graph class implementation
***************************/
Graph::Graph(unsigned vertex_count, std::vector< std::pair<unsigned,unsigned> > edges)
{
    this->vertex_count = vertex_count;
    adjacency_list = new std::list<unsigned> [vertex_count];
    for( unsigned i=0; i < edges.size(); i++)
    {
        addEdge(edges[i].first, edges[i].second);
    }
}

void Graph::addEdge(unsigned first, unsigned second)
{
    //our graph is undirected, so vertices are adjacent to each other
    this->adjacency_list[first].push_back(second);
    this->adjacency_list[second].push_back(first);
}


bool Graph::printPath(unsigned from, unsigned to) {

    // special case handling
    if (from == to) {
        std::cout << from << std::endl; //Per prof. we only output one vertex when start and dest. are the same
        return true;
    }

    // create some variables
    unsigned current_from;
    bool *visited = new bool[vertex_count];
    int *parents = new int[vertex_count];
    std::list<unsigned> vertex_queue;
    std::list<unsigned>::iterator i;

    // initialize variables
    for (unsigned i = 0; i < vertex_count; i++) {
        visited[i] = false;
        parents[i] = -1;
    }
    visited[from] = true;
    vertex_queue.push_back(from);

    // conduct breadth first search (BFS)
    while (!vertex_queue.empty()) {
        current_from = vertex_queue.front();
        vertex_queue.pop_front();

        // go over all adjacent vertices of current_from
        for (i = this->adjacency_list[current_from].begin(); i != this->adjacency_list[current_from].end(); ++i) {
            if (*i == to) { //destination found
                parents[*i] = current_from;

                //backtrack the path and  print it
                std::string path = "\n";
                while( to != from ) {
                    path.insert(0, std::to_string(to));
                    path.insert(0, "-");

                    if(parents[to]==-1) {
                        throw "Invlid parent node -1. Something went terribly wrong. This should never happen.";
                    }
                    to = parents[to];
                }
                std::cout << to << path;
                return true;
            }
            else if (!visited[*i]) {// seeing this vertex the first time
                visited[*i] = true;
                parents[*i] = current_from;
                vertex_queue.push_back(*i);
            }
        }
    }
    return false;
}


/****************************
 * UserInterface class definition
 ****************************/
class UserInterface
{
private:
    bool debug_mode = false;
    std::istringstream input_stream;
    std::string current_line;

    std::string readLine();
    char getCharInput();
    int getIntInput();
    bool isValidChar(char value, char check_for, bool throw_exception);
    bool isValidVertex(int value, unsigned min_value, unsigned max_vertex);
    unsigned getVertexInput();
    std::vector< std::pair<unsigned,unsigned> > getEdgesInput(unsigned max_vertex);
    std::pair<unsigned,unsigned> getPathRequest(unsigned max_vertex);
    void debugOutput(std::size_t vertex_count, std::vector< std::pair<unsigned,unsigned> > edges, std::pair<unsigned, unsigned> path_request);

public:
    UserInterface();
    void debugOn();
    void debugOff();
    int run();
};

/****************************
 * UserInterface class implementation
 ****************************/
UserInterface::UserInterface()
{
}

void UserInterface::debugOn()
{
    this->debug_mode = true;
}

void UserInterface::debugOff()
{
    this->debug_mode = false;
}

std::string UserInterface::readLine()
{
    std::getline(std::cin, this->current_line);
    this->input_stream.clear();
    this->input_stream.str( this->current_line );
    if(this->debug_mode){
        std::cout << "\tLine Input: " << this->current_line << " (size = " << this->current_line.size() << ")\n";
    }
    return this->current_line;
}

char UserInterface::getCharInput()
{
    char char_input;
    this->input_stream >> char_input;
    if(this->input_stream.fail()) {
        throw "Failed to read a character from input stream.";
    }
    return char_input;
}

int UserInterface::getIntInput()
{
    int int_input;
    this->input_stream >> int_input;
    if(this->input_stream.fail()) {
        throw "Failed to read an integer from input stream.";
    }
    return int_input;
}

bool UserInterface::isValidChar(char value, char check_for, bool throw_exception = true)
{
    if(value != check_for)
    {
        if( throw_exception )
        {
            std::string error(1,value);
            error.append(" is not a valid input character. ");
            error.append(1,check_for);
            error.append(" expected");
            throw error;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool UserInterface::isValidVertex(int value, unsigned min_value, unsigned max_vertex)
{
    if (value < (int) min_value || value > (int) max_vertex)
    {
        std::string error(std::to_string(value) + " is not a valid vertex. Expected value between " + std::to_string(min_value) +
                          " and " + std::to_string(max_vertex));
        throw error;
    }
    return true;
}

unsigned UserInterface::getVertexInput()
{
    int vertex_count;
    vertex_count = this->getIntInput();
    isValidVertex(vertex_count, 1, INT_MAX);
    return vertex_count;
}

std::vector< std::pair<unsigned,unsigned> > UserInterface::getEdgesInput(unsigned max_vertex)
{
    std::vector< std::pair<unsigned,unsigned> > edges;
    char command, delimiter;
    int vertex;

    command = this->getCharInput();
    isValidChar(command, 'E');

    delimiter = this->getCharInput();
    isValidChar(delimiter, '{');

    while (!this->input_stream.eof())
    {
        std::pair<unsigned,unsigned> edge;
        delimiter = this->getCharInput();
        if( isValidChar(delimiter, '}', false) ){
            break;
        }
        else {
            isValidChar(delimiter, '<');
        }

        vertex = this->getIntInput();
        isValidVertex(vertex, 0, max_vertex);
        edge.first = vertex;

        delimiter = this->getCharInput();
        isValidChar(delimiter, ',');

        vertex = this->getIntInput();
        isValidVertex(vertex, 0, max_vertex);
        edge.second = vertex;

        delimiter = this->getCharInput();
        isValidChar(delimiter, '>');

        edges.push_back(edge);

        delimiter = this->getCharInput();
        if( isValidChar(delimiter, '}', false) )
        {
            break;
        }
        else
        {
            isValidChar(delimiter, ',');
        }
    }//while

    std::cout << this->current_line << std::endl;
    return edges;
}

std::pair<unsigned,unsigned> UserInterface::getPathRequest(unsigned max_vertex)
{
    std::pair<unsigned,unsigned> path_request;
    int value;
    value = this->getIntInput();
    isValidVertex(value, 0, max_vertex);
    path_request.first = value;

    value = this->getIntInput();
    isValidVertex(value, 0, max_vertex);
    path_request.second = value;

    return path_request;
}

void UserInterface::debugOutput(std::size_t vertex_count, std::vector< std::pair<unsigned,unsigned> > edges, std::pair<unsigned, unsigned> path_request)
{
    std::cout << "Vertex Count: " << vertex_count << "\n";
    std::cout << "\nEdges:\n";
    for( std::size_t i=0; i < edges.size(); i++)
    {
        std::cout << "\t" << i+1 << ") " << edges[i].first << "," << edges[i].second << std::endl;
    }
    std::cout << "\nPath Request:\n\t" << path_request.first << " --> " << path_request.second << "\n\n";
}

int UserInterface::run()
{
    std::vector< std::pair<unsigned,unsigned> > edges;
    std::pair<unsigned, unsigned> path_request;
    std::size_t vertex_count = 0;
    char command;
    bool read_edges = true;
    unsigned counter = 1;

    while (!std::cin.eof())
    {
        std::string line;
        try
        {
            if(debug_mode) {
                std::cout << "----------- Run Count: " << counter++ << "------------\n";
            }
            line = this->readLine();
            if (line.size()==0) {
                continue; //TBD: Do we need to throw an error?
            }
            command = this->getCharInput();
            if(command=='V')
            {
                vertex_count = 0;
                edges.clear();
                read_edges = true;
                vertex_count = this->getVertexInput();

                std::cout << "V " << vertex_count << std::endl;

                while(!std::cin.eof() && read_edges) {
                    try{
                        this->readLine(); //read next line for edges
                        edges = this->getEdgesInput(vertex_count-1);
                        read_edges = false;
                    }
                    catch(std::string error) {
                        std::cerr << "Error: " << error << std::endl;
                    }
                }
            }
            else if (command == 's')
            {
                path_request = this->getPathRequest(vertex_count-1);

                if(this->debug_mode){
                    this->debugOutput(vertex_count, edges, path_request);
                    std::cout << "\nPath output:\n\t";
                }

                // Let us create a graph and find the path
                Graph graph(vertex_count, edges);
                if( !graph.printPath(path_request.first,path_request.second))
                {
                    std::string error("There is no path from " + std::to_string(path_request.first) +
                                      " to " + std::to_string(path_request.second));
                    throw error;
                }
            }
            else {
                std::string error(1, command);
                error.append(" is not a valid command. Valid commands here are V or s.");
                throw error;
            }

        }
        catch(std::string error)
        {
            std::cerr << "Error: " << error << std::endl;
        }
        catch(const char* error)
        {
            std::cerr << "Error: " << error << std::endl;
        }
    }//while

    return 0;
}

int main(int argc, char **argv)
{
    UserInterface ui;

    //check for debug mode
    if( argc == 2 ) {
        std::string arg_1(argv[1]);
        if( arg_1.compare("-v")==0 ) {
            ui.debugOn();
        }
    }
    return ui.run();
}
