#include "motorcycle.h"

Motorcycle::Motorcycle(MyMesh& polymesh, HalfedgeHandle& handle)
    : polymesh(polymesh),  trace(), handle(handle), origin(handle.idx()){
    this->trace.push_back(this->origin);
}

bool Motorcycle::operator==(const Motorcycle& m) const {
    return this->origin == m.origin;
}

HalfedgeHandle Motorcycle::curr() {
    return this->handle;
}

HalfedgeHandle Motorcycle::next() {
    HalfedgeHandle temp = this->handle;
    temp = this->polymesh.next_halfedge_handle(temp);
    temp = this->polymesh.opposite_halfedge_handle(temp);
    temp = this->polymesh.next_halfedge_handle(temp);
    return temp;
}

HalfedgeHandle Motorcycle::step() {
    this->handle = this->next();
    this->trace.push_back(this->handle.idx());
    return this->handle;
}
