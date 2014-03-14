#include "util.h"
#include "gamedata.h"

namespace {

bool has_negative_values(std::vector<int> v) {
  for (auto value : v) {
    if (value < 0) return true;
  }
  return false;
}

std::vector<RegionVector> make_regions_by_super(reg_t num_supers, RegionVector region_super) {
  std::vector<RegionVector> regions_by_super(num_supers);
  for (auto r : range(region_super.size())) {
    regions_by_super[region_super[r]].push_back(r);
  }
  return regions_by_super;
}

std::vector<std::vector<int> > make_distance_mat(std::vector<RegionVector> neighbours) {
  std::vector<int> distance_row(neighbours.size(), -1);
  std::vector<std::vector<int> > distances = std::vector<std::vector<int> >(neighbours.size(), distance_row);

  for (size_t region = 0; region < neighbours.size(); ++region) {
    distances[region][region] = 0;
    while (has_negative_values(distances[region])) {
      for (size_t test_region = 0; test_region < neighbours.size(); ++test_region) {
        if (distances[region][test_region] >= 0) continue;
        for (RegionVector::iterator it = neighbours[test_region].begin(); it != neighbours[test_region].end(); ++it) {
          if (distances[region][*it] >= 0) {
            distances[region][test_region] = distances[region][*it] + 1;
            distances[test_region][region] = distances[region][test_region];
          }
        }
      }
    }
  }
  return distances;
}

std::vector<std::vector<bool> > make_neighbour_mat(std::vector<RegionVector> neighbours) {
  std::vector<bool> neigbour_row(neighbours.size(), false);
  std::vector<std::vector<bool> > neighbours_mat = std::vector<std::vector<bool> >(neighbours.size(), neigbour_row);

  for (auto r : range(neighbours.size())) {
    for (auto neighbour : neighbours[r]) {
      neighbours_mat[r][neighbour] = true;
    }
  }

  return neighbours_mat;
}

}

GameData::GameData(CanonicalGameSetup setup): super_award(setup.super_award),
                                              region_super(setup.region_super),
                                              regions_by_super(make_regions_by_super(setup.super_award.size(), setup.region_super)),
                                              neighbours(make_neighbour_mat(setup.neighbours)),
                                              neighbour_ids(setup.neighbours),
                                              distances(make_distance_mat(setup.neighbours)),
                                              super_n(setup.super_award.size()),
                                              region_n(setup.region_super.size()),
                                              init_regions(setup.init_options),
                                              owner(std::vector<Owner>(setup.region_super.size(), NEUTRAL)),
                                              occupancy(ArmyVector(setup.region_super.size(), 2)),
                                              visible(std::vector<bool>(setup.region_super.size(), false)) {
}


army_t GameData::get_enemy_neighbour_armies(reg_t region) const {
  army_t num_neighbours = 0;
  for (auto r : range(region_n)) {
    if (!neighbours[region][r]) continue;
    if (owner[r] == ME) continue;
    num_neighbours += occupancy[r];
  }
  return num_neighbours;
}

bool GameData::has_enemy_neighbours(reg_t region) const {
  for (auto r : range(region_n)) {
    if (!neighbours[region][r]) continue;
    if (owner[r] != ME) return true;
  }
  return false;
}

army_t GameData::count_neighbour_armies(reg_t region, Owner owner_) const {
  army_t count = 0;
  for (auto neighbour : neighbour_ids[region]) {
    if (owner[neighbour] == owner_)
      count += occupancy[neighbour];
  }
  return count;
}
