#include <motorcycleconstants.h>
#include <motorcycle.h>
#include <motorcyclegraph.h>

#include <iostream>

#include <OpenMesh/Core/IO/MeshIO.hh>

using namespace std;

int main(int argc, char *argv[]) {

    assert(argc == 2);

    string filename = argv[1];
    
    MotorcycleConstants::MyMesh polymesh;

    try {
        if (!OpenMesh::IO::read_mesh(polymesh, filename)) {
            cerr << "Cannot read mesh from " << filename << endl;
            return 1;
        }
    }
    catch(exception& x) {
        cerr << x.what() << endl;
        return 1;
    }

    MotorcycleGraph motorcycleGraph(polymesh);
    motorcycleGraph.save_mesh();

    return 0;
}
