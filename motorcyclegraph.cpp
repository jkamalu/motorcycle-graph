#include "motorcyclegraph.h"

#define foreach BOOST_FOREACH

MotorcycleGraph::MotorcycleGraph(MyMesh& polymesh) :
    V("V"),
    M("M"),
    E("E"),
    v_manager(polymesh, this->V),
    m_manager(polymesh, this->M),
    e_manager(polymesh, this->E),
    polymesh(polymesh) {

    Motorcycles motorcycles;

    for (MyMesh::EdgeIter e_iter = this->polymesh.edges_begin(); e_iter != this->polymesh.edges_end(); ++e_iter) {
        this->e_manager[*e_iter] = this->polymesh.is_boundary(*e_iter);
    }

    int motorcycle_count = 0;
    for (MyMesh::VertexIter v_iter = this->polymesh.vertices_begin(); v_iter != this->polymesh.vertices_end(); ++v_iter) {
        bool is_extraordinary = !this->is_ordinary(*v_iter);
        this->v_manager[*v_iter] = is_extraordinary || this->polymesh.is_boundary(*v_iter);
        this->m_manager[*v_iter] = is_extraordinary;
        // TODO: add face assignment here
        motorcycle_count += is_extraordinary;
        if (is_extraordinary) {
            for (MyMesh::VertexOHalfedgeIter h_iter = this->polymesh.voh_begin(*v_iter); h_iter != this->polymesh.voh_end(*v_iter); ++h_iter) {
                HalfedgeHandle halfedge = *h_iter;

                EdgeHandle edge = this->polymesh.edge_handle(halfedge);
                this->e_manager[edge] = true;

                assert(motorcycles.count(halfedge.idx()) == 0);
                motorcycles.emplace(halfedge.idx(), std::unique_ptr<Motorcycle>(new Motorcycle(this->polymesh, halfedge)));
            }
        }
    }

    std::cout << "# Extraordinary = " << motorcycle_count << std::endl;
    std::cout << "# Vertices = " << this->polymesh.n_vertices() << std::endl;
    std::cout << "# Faces = " << this->polymesh.n_faces() << std::endl;

    std::cout << "Initialized " << motorcycles.size() << " motorcycles." << std::endl;

    this->propagate_motorcycles(motorcycles);

    std::vector<Perimeter> perimeters = this->extract_perimeters();

    std::cout << "Extracted " << perimeters.size() << " perimeters." << std::endl;

}

/*
 * Returns whether a vertex handle is one of...
 *   - a non-boundary vertex incident with four edges
 *   - a boundary vertex incident with at most three edges
 *
 */
bool MotorcycleGraph::is_ordinary(VertexHandle v) {
    int out_degree = 0;
    for (MyMesh::VertexVertexIter iter = this->polymesh.vv_begin(v); iter != this->polymesh.vv_end(v); ++iter) {
        out_degree++;
    }
    bool non_boundary_quad = !this->polymesh.is_boundary(v) && out_degree == 4;
    bool boundary_sub_quad = this->polymesh.is_boundary(v) && out_degree < 4;
    return non_boundary_quad or boundary_sub_quad;
}

void MotorcycleGraph::propagate_motorcycles(Motorcycles& motorcycles) {

    std::unordered_set<int> v_seen{};

    // go until every motorcycle has crashed
    while (!motorcycles.empty()) {

        // build positions: map of vertex index to set of halfedge indices
        // which uniquely define the origin of some motorcycles
        std::unordered_map<int,  std::unordered_set<int> > positions;
        foreach (int origin, motorcycles | boost::adaptors::map_keys) {
            std::unique_ptr<Motorcycle>& motorcycle = motorcycles.at(origin);
            HalfedgeHandle h_curr = motorcycle->curr();
            VertexHandle v_curr = this->polymesh.to_vertex_handle(h_curr);
            HalfedgeHandle h_next = motorcycle->step();
            this->e_manager[this->polymesh.edge_handle(h_next)] = true;
            positions[v_curr.idx()].insert(motorcycle->origin);
        }

        // handle the event at every location one by one, removing
        // motorcycles which have crashed from the map
        foreach (int position, positions | boost::adaptors::map_keys) {
            VertexHandle v_curr = this->polymesh.vertex_handle(position);
            bool seen = v_seen.count(position) == 1;
            bool is_boundary = this->polymesh.is_boundary(v_curr);
            bool is_tertiary = positions[position].size() > 2;
            bool is_extraordinary = !this->is_ordinary(v_curr);
            if (seen || is_boundary || is_tertiary || is_extraordinary) {
                this->v_manager[v_curr] = true;
                for (const int& origin : positions[position]) {
                    motorcycles.erase(origin);
                }
            } else if (positions[position].size() == 2) {
                std::unordered_set<int>::iterator m_iter = positions[v_curr.idx()].begin();
                std::unique_ptr<Motorcycle>& m1 = motorcycles.at(*m_iter++);
                std::unique_ptr<Motorcycle>& m2 = motorcycles.at(*m_iter++);
                if (m1->next() == this->polymesh.opposite_halfedge_handle(m2->curr())) {
                    for (const int& origin : positions[position]) {
                        motorcycles.erase(origin);
                    }
                } else {
                    this->v_manager[v_curr] = true;
                    int m1_next_out = this->polymesh.next_halfedge_handle(m1->curr()).idx();
                    int m2_opposite = this->polymesh.opposite_halfedge_handle(m2->curr()).idx();
                    if (m1_next_out == m2_opposite) {
                        motorcycles.erase(m2->origin);
                    } else {
                        motorcycles.erase(m1->origin);
                    }
                }
            }
        }

        // update the set we use to keep track of seen vertex indices
        foreach (int position, positions | boost::adaptors::map_keys) {
            v_seen.insert(position);
        }

    }
}

std::vector<MotorcycleGraph::Perimeter> MotorcycleGraph::extract_perimeters() {

    // TODO: desc.
    std::vector<std::pair<int, int> > seeds;
    for (const VertexHandle& v_handle : this->polymesh.vertices()) {
        if (this->v_manager[v_handle]) {
            MyMesh::VertexOHalfedgeCWIter iter = this->polymesh.voh_cwbegin(v_handle);
            while  (iter != this->polymesh.voh_cwend(v_handle)) {
                bool is_boundary = this->polymesh.is_boundary(v_handle);
                bool is_edge  = e_manager[this->polymesh.edge_handle(*iter)];
                if (!is_boundary && is_edge) {
                    seeds.push_back(std::make_pair(v_handle.idx(), iter->idx()));
                }
                ++iter;
            }
        }
    }

    // TODO: desc.
    std::unordered_set<int> h_seen{};
    std::vector<Perimeter> perimeters;
    for (const std::pair<int, int>& seed : seeds) {
        h_seen.insert(seed.second);
        VertexHandle v_seed = this->polymesh.vertex_handle(seed.first);
        VertexHandle v_curr = v_seed;
        HalfedgeHandle h_curr = this->polymesh.halfedge_handle(seed.second);
        bool good_seed = true;
        Perimeter perimeter;
        while (this->polymesh.to_vertex_handle(h_curr) != v_seed) {
            std::vector<int> interior_halfedges;
            HalfedgeHandle temp = this->polymesh.next_halfedge_handle(h_curr);
            while (true) {
                interior_halfedges.push_back(temp.idx());
                if (e_manager[this->polymesh.edge_handle(temp)]) {
                    break;
                }
                temp = this->polymesh.opposite_halfedge_handle(temp);
                temp = this->polymesh.next_halfedge_handle(temp);
            }
            assert(temp != h_curr);
            v_curr = this->polymesh.to_vertex_handle(h_curr);
            h_curr = temp;
            if (h_seen.count(h_curr.idx()) == 1) {
                good_seed = false;
                break;
            }
            h_seen.insert(h_curr.idx());
            perimeter.push_back(std::make_pair(v_curr.idx(), interior_halfedges));
        }
        if (good_seed) {
            perimeters.push_back(perimeter);
        }
    }

    return perimeters;

}
