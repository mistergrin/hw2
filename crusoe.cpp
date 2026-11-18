#include "crusoe.hpp"

vertex::vertex() = default;

vertex::vertex(std::string str, int x, int y, std::string col)
    : name(str), xy({x, y}), c_forward(col)
{}

bool vertex::add_neighbour(size_t vv, const std::string& col) {
    for (const auto& neighbour : neighbours) {
        if (neighbour.first == vv) {
            return false;
        }
    }

    neighbours.push_back({vv, col});
    return true;
}

std::vector<std::pair<size_t, std::string>> vertex::get_neighbour() const {
    return neighbours;
}

std::pair<int, int> vertex::get_xy() const {
    return xy;
}

void vertex::set_color(const std::string& col) {
    c_forward = col;
}

std::string vertex::get_color() const {
    return c_forward;
}

void vertex::set_edge_color(size_t vv, const std::string& col) {
    for (auto& neighbour : neighbours) {
        if (neighbour.first == vv) {
            neighbour.second = col;
            return;
        }
    }
}

std::string vertex::get_edge_color(size_t vv) {
    for (const auto& neighbour : neighbours) {
        if (neighbour.first == vv) {
            return neighbour.second;
        }
    }

    return "#FFFFFF";
}

void graph::add_vertex(int x, int y, const std::string& col) {
    size_t vertex_index = num_elem;
    std::string vertex_name = std::to_string(vertex_index);

    vertex v = vertex(vertex_name, x, y, col);
    vertices.push_back(v);
    num_elem++;
}

void graph::add_edge(size_t v1, size_t v2, const std::string& col ) {
    if (v1 >= vertices.size() || v2 >= vertices.size()) {
        return;
    }

    vertex& vertex_1 = vertices[v1];
    vertex& vertex_2 = vertices[v2];

    vertex_1.add_neighbour(v2, col);
    vertex_2.add_neighbour(v1, col);
}

bool graph::is_edge(size_t v1, size_t v2) const {
    if (v1 >= vertices.size() || v2 >= vertices.size()) {
        return false;
    }
    std::vector<std::pair<size_t, std::string>> vertex_neighbours = vertices[v1].get_neighbour();

    for (const std::pair<size_t, std::string>& neighbour : vertex_neighbours ) {
        if (neighbour.first == v2) {
            return true;
        }
    }
    return false;
}

std::string graph::edge_color(size_t v1, size_t v2) const {
    if (v1 >= vertices.size() || v2 >= vertices.size()) {
        return "#FFFFFF";
    }
    std::vector<std::pair<size_t, std::string>> vertex_neighbours = vertices[v1].get_neighbour();

    for (const std::pair<size_t, std::string>& neighbour : vertex_neighbours ) {
        if (neighbour.first == v2) {
            return neighbour.second;
        }
    }
    return "#FFFFFF";
}

std::string graph::vertex_color(size_t v1) const {
    if (v1 >= vertices.size()) {
        return "#FFFFFF";
    }
    std::string col = vertices[v1].get_color();
    return col;
}

void graph::set_vertex_color(size_t v1, const std::string& col) {
    if (v1 >= vertices.size()) {
        return;
    }
    vertex& vertex_1 = vertices[v1];
    vertex_1.set_color(col);
}

void graph::set_edge_color(size_t v1, size_t v2, const std::string& col) {
    if (v1 >= vertices.size() || v2 >= vertices.size()) {
        return;
    }

    vertex& vertex_1 = vertices[v1];
    vertex_1.set_edge_color(v2, col);

    vertex& vertex_2 = vertices[v2];
    vertex_2.set_edge_color(v1, col);
}

bool graph::empty() const {
    if (vertices.empty()) {
        return true;
    }
    return false;
}

size_t graph::num_edge() const {
    size_t num_neighbours = 0;
    for (const auto& vertex : vertices) {
        num_neighbours += vertex.get_neighbour().size();
    }
    size_t result = num_neighbours / 2;
    return result;
}

size_t graph::size() const {
    return vertices.size();
}

vertex graph::get_vertex(size_t num) const {
    vertex v = vertices[num];
    return v;
}

void graph::is_achievable(size_t from, std::vector<size_t> &achieved) {
    if (from >= vertices.size()) {
        return;
    }
    std::vector<bool> visited(vertices.size(), false);
    achieved.clear();
    std::vector<size_t> to_achieve;
    to_achieve.push_back(from);
    visited[from] = true;

    while (!to_achieve.empty()) {
        size_t current = to_achieve.back();
        to_achieve.pop_back();
        achieved.push_back(current);
        vertex& v = vertices[current];
        for (const std::pair<size_t, std::string>& neighbour : v.get_neighbour()) {
            size_t neighbour_index = neighbour.first;
            if (!visited[neighbour_index]) {
                visited[neighbour_index] = true;
                to_achieve.push_back(neighbour_index);
            }
        }
    }
}

void graph::color_component(std::vector<size_t> cmp, const std::string &col) {
    for (size_t index_vertex : cmp) {
        set_vertex_color(index_vertex, col);
        vertex& v = vertices[index_vertex];
        for (const std::pair<size_t, std::string>& neighbour : v.get_neighbour()) {
            set_edge_color(index_vertex, neighbour.first, col);
        }
    }
}

std::vector<size_t> graph::path(size_t v1, size_t v2) {
    std::vector<size_t> result;
    if (v1 >= vertices.size() || v2 >= vertices.size()) {
        return result;
    }
    if (v1 == v2) {
        result.push_back(v1);
        return result;
    }
    std::vector<bool> visited(vertices.size(), false);
    visited[v1] = true;

    size_t head = 0;
    std::vector<size_t> parent(vertices.size());
    std::vector<size_t> to_achieve;
    to_achieve.push_back(v1);

    while (to_achieve.size() > head) {
        size_t current = to_achieve[head];
        head++;

        vertex& v = vertices[current];
        for (const std::pair<size_t, std::string>& neighbour : v.get_neighbour()) {
            size_t neighbour_index = neighbour.first;
            if (!visited[neighbour_index]) {
                parent[neighbour_index] = current;
                visited[neighbour_index] = true;
                to_achieve.push_back(neighbour_index);
            }
        }
    }

    if (!visited[v2]) {
        return result;
    }
    size_t current = v2;
    while (current != v1) {
        result.push_back(current);
        current = parent[current];
    }
    result.push_back(v1);
    std::reverse(result.begin(), result.end());
    return result;
}

void graph::color_path(std::vector<size_t> pth, const std::string &col) {
    if (pth.size() < 2) {
        return;
    }

    for (int i = 0; i < pth.size() - 1; i++) {
        set_edge_color(pth[i], pth[i + 1], col);
    }
}

graph::graph_comp::graph_comp(graph& gg)
    : gg(gg) {
    std::vector<bool> visited(gg.size(), false);
    for (size_t i = 0; i < gg.size(); i++) {
        if (!visited[i]) {
            std::vector<size_t> component;
            gg.is_achievable(i, component);
            for (size_t j = 0; j < component.size(); j++) {
                visited[component[j]] = true;
            }
            components.push_back(component);
        }
    }
}

void graph::graph_comp::color_componennts() {
    std::vector<std::string> colors = {"red", "olive", "orange", "lightblue", "yellow", "pink", "cyan", "purple", "brown", "magenta"};
    for (size_t i = 0; i < components.size(); i++) {
        if (components[i].size() < 2) {
            continue;
        }
        gg.color_component(components[i], colors[i % colors.size()]);
    }
}

size_t graph::graph_comp::count() const {
    return components.size();
}

size_t graph::graph_comp::count_without_one() const {
    size_t result = 0;

    for (size_t i = 0; i < components.size(); i++) {
        if (components[i].size() < 2) {
            continue;
        }
        result += 1;
    }
    return result;
}

size_t graph::graph_comp::max_comp() const {
    if (components.empty()) {
        return 0;
    }

    size_t max_index = 0;
    size_t max_comp = components[0].size();
    for (size_t i = 0; i < components.size(); i++) {
        size_t current_size = components[i].size();
        if (current_size > max_comp) {
            max_index = i;
            max_comp = current_size;
        }
    }
    return max_index;
}

size_t graph::graph_comp::size_of_comp(size_t i) const {
    if (i >= components.size()) {
        return 0;
    }
    return components[i].size();
}

std::vector<size_t> graph::graph_comp::get_component(size_t i) const {
    std::vector<size_t> result;
    if (i >= components.size()) {
        return result;
    }
    return components[i];
}

bool graph::graph_comp::same_comp(size_t v1, size_t v2) const {
    for (const std::vector<size_t>& component : components) {
        bool found_v1 = false;
        bool found_v2 = false;
        for (size_t vertex_index : component) {
            if (vertex_index == v1) {
                found_v1 = true;
            }
            if (vertex_index == v2) {
                found_v2 = true;
            }
        }
        if (found_v1 && found_v2) {
            return true;
        }
    }
    return false;
}


graph::graph_fence::graph_fence(graph &gg, size_t vv, size_t distance) :
gg(gg) {
    if (vv >= gg.size()) {
        return;
    }
    std::vector<bool> visited(gg.size(), false);
    std::vector<size_t> distances(gg.size(), 0);
    std::vector<size_t> queue;
    size_t head = 0;

    visited[vv] = true;
    distances[vv] = 0;
    queue.push_back(vv);
    fence.push_back(vv);
    while (head < queue.size()) {
        size_t current = queue[head];
        head++;
        if (distances[current] >= distance) {
            continue;
        }

        for (const std::pair<size_t, std::string>& neighbour : gg.get_vertex(current).get_neighbour()) {
            size_t neighbour_index = neighbour.first;

            if (!visited[neighbour_index]) {
                visited[neighbour_index] = true;
                distances[neighbour_index] = distances[current] + 1;

                queue.push_back(neighbour_index);
                fence.push_back(neighbour_index);
            }
        }
    }
}

size_t graph::graph_fence::count_stake() const {
    return fence.size();
}

size_t graph::graph_fence::get_stake(size_t i) const {
    if (i >= fence.size()) {
        return 0;
    }
    return fence[i];
}

void graph::graph_fence::color_fence(const std::string &col) {
    for (size_t i = 0; i < fence.size(); i++) {
        gg.set_vertex_color(fence[i], col);
    }
}
