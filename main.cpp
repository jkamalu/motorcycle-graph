#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <motorcycle.h>
#include <motorcyclegraph.h>

using namespace std;

int main(int argc, char *argv[]) {

    assert(argc == 2);

    string filename = argv[1];
    
    PolyMesh_ArrayKernelT<> polymesh;

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

    return 0;
}
