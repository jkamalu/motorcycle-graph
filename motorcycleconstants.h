#ifndef MOTORCYCLECONSTANTS_H
#define MOTORCYCLECONSTANTS_H

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

using namespace OpenMesh;

class MotorcycleConstants {

public:

    struct MyTraits : public DefaultTraits {
        FaceAttributes(OpenMesh::Attributes::Color);
    };

    typedef PolyMesh_ArrayKernelT<MyTraits> MyMesh;

};

#endif // MOTORCYCLECONSTANTS_H
