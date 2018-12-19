#ifndef MOTORCYCLE_H
#define MOTORCYCLE_H

#include <boost/functional/hash.hpp>
#include <boost/core/noncopyable.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

using namespace OpenMesh;

class Motorcycle : boost::noncopyable {

private:

    typedef PolyMesh_ArrayKernelT<> MyMesh;

    MyMesh polymesh;
    std::vector<int> trace;
    mutable HalfedgeHandle handle;

public:

    const int origin;

    Motorcycle(MyMesh& polymesh, HalfedgeHandle& current);
    HalfedgeHandle curr();
    HalfedgeHandle next();
    HalfedgeHandle step();

    bool operator==(const Motorcycle& m) const;

};

namespace std {
    template <>
    struct hash<Motorcycle> {
        std::size_t operator()(const Motorcycle& m) const {
            return std::hash<int>()(m.origin);
        }
    };
};

#endif // MOTORCYCLE_H
