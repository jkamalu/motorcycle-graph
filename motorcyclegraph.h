#ifndef MOTORCYCLEGRAPH_H
#define MOTORCYCLEGRAPH_H

#include <motorcycle.h>

#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include <boost/range/adaptor/map.hpp>
#include <boost/foreach.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

using namespace OpenMesh;

class MotorcycleGraph {

private:
    typedef PolyMesh_ArrayKernelT<> MyMesh;
    typedef std::unordered_map<int, std::unique_ptr<Motorcycle> > Motorcycles;
    typedef std::vector<std::pair<int, std::vector<int> > > Perimeter;

    const char V[2];// = "V";
    const char M[2];// = "M";
    const char E[2];// = "E";

    PropertyManager<VPropHandleT<bool>, MyMesh> v_manager;
    PropertyManager<VPropHandleT<bool>, MyMesh> m_manager;
    PropertyManager<EPropHandleT<bool>, MyMesh> e_manager;

    MyMesh polymesh;

    bool is_ordinary(VertexHandle v);
    void propagate_motorcycles(Motorcycles& motorcycles);
    std::vector<Perimeter> extract_perimeters();

public:
    MotorcycleGraph(MyMesh& polymesh);

};

#endif // MOTORCYCLEGRAPH_H
