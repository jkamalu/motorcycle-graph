#ifndef MOTORCYCLEGRAPH_H
#define MOTORCYCLEGRAPH_H

#include <motorcycleconstants.h>
#include <motorcycle.h>

#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>

#include <boost/range/adaptor/map.hpp>
#include <boost/foreach.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

using namespace OpenMesh;

class MotorcycleGraph {

private:

    typedef std::unordered_map<int, std::unique_ptr<Motorcycle> > Motorcycles;
    typedef std::vector<std::pair<int, std::vector<int> > > Perimeter;

    const char V[2];
    const char M[2];
    const char E[2];
    const char F[2];

    PropertyManager<VPropHandleT<bool>, MotorcycleConstants::MyMesh> v_manager;
    PropertyManager<VPropHandleT<bool>, MotorcycleConstants::MyMesh> m_manager;
    PropertyManager<EPropHandleT<bool>, MotorcycleConstants::MyMesh> e_manager;
    PropertyManager<FPropHandleT<size_t>, MotorcycleConstants::MyMesh> f_manager;

    IO::Options wopt;

    MotorcycleConstants::MyMesh polymesh;

    bool is_ordinary(VertexHandle v);
    void propagate_motorcycles(Motorcycles& motorcycles);
    std::vector<Perimeter> extract_perimeters();
    void assign_patches(std::vector<Perimeter>& perimeters);

    std::vector<Perimeter> sort_perimeters(std::vector<Perimeter>& perimeters) {
        std::unordered_map<int, std::vector<Perimeter> > index;
        for (auto p : perimeters) {
            index[p.size()].push_back(p);
        }

        std::vector<int> keys;
        for (int i : index | boost::adaptors::map_keys) {
            keys.push_back(i);
        }
        sort(keys.begin(), keys.end());

        std::vector<Perimeter> sorted;
        for (std::vector<int>::iterator iter = keys.end(); iter != keys.begin(); iter--) {
            for (auto p : index[*iter]) {
                sorted.push_back(p);
            }
        }
        return sorted;
    }

public:

    MotorcycleGraph(MotorcycleConstants::MyMesh& polymesh);
    void save_mesh();

};

#endif // MOTORCYCLEGRAPH_H
