#include "gamedata.h"

namespace {

bool has_negative_values(std::vector<int> v) {
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it < 0) return true;
  }
  return false;
}

std::vector<std::vector<int> > make_distance_mat(std::vector<std::vector<int> > neighbours) {
  std::vector<int> distance_row(neighbours.size(), -1);
  std::vector<std::vector<int> > distances = std::vector<std::vector<int> >(neighbours.size(), distance_row);

  for (int region = 0; region < neighbours.size(); ++region) {
    distances[region][region] = 0;
    while (has_negative_values(distances[region])) {
      for (int test_region = 0; test_region < neighbours.size(); ++test_region) {
        if (distances[region][test_region] >= 0) continue;
        //TODO, this is not correct!!!!
        for (int neigh_region = 0; neigh_region < neighbours.size(); ++neigh_region) {
          if (distances[region][neigh_region] >= 0) {
            distances[region][test_region] = distances[region][neigh_region] + 1;
            distances[test_region][region] = distances[region][test_region];
          }
        }
      }
    }
  }
  return distances;
}

std::vector<std::vector<bool> > make_neighbour_mat(std::vector<std::vector<int> > neighbours) {
  std::vector<bool> neigbour_row(neighbours.size(), false);
  std::vector<std::vector<bool> > neighbours_mat = std::vector<std::vector<bool> >(neighbours.size(), neigbour_row);

  for (int r = 0; r < neighbours.size(); ++r) {
    for (std::vector<int>::iterator it = neighbours[r].begin(); it != neighbours[r].end(); ++it) {
      neighbours_mat[r][*it] = true;
    }
  }

  return neighbours_mat;
}

}

GameData::GameData(CanonicalGameSetup setup): super_award(setup.super_award),
                                              region_super(setup.region_super),
                                              neighbours(make_neighbour_mat(setup.neighbours)),
                                              neighbour_ids(setup.neighbours),
                                              distances(make_distance_mat(setup.neighbours)),
                                              super_n(setup.super_award.size()),
                                              region_n(setup.region_super.size()),
                                              init_regions(setup.init_options),
                                              owner(std::vector<Owner>(setup.region_super.size(), NEUTRAL)),
                                              occupancy(std::vector<int>(setup.region_super.size(), 2)) {

  super_region_names.push_back("North America");
  super_region_names.push_back("South America");
  super_region_names.push_back("Europe");
  super_region_names.push_back("Africa");
  super_region_names.push_back("Asia");
  super_region_names.push_back("Australia");

}


int GameData::get_enemy_neighbour_armies(int region) const {
  int num_neighbours = 0;
  for (size_t r = 0; r < region_n; ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] == ME) continue;
    num_neighbours += occupancy[r];
  }
  return num_neighbours;
}

bool GameData::has_enemy_neighbours(int region) const {
  for (size_t r = 0; r < region_n; ++r) {
    if (!neighbours[region][r]) continue;
    if (owner[r] != ME) return true;
  }
  return false;
}