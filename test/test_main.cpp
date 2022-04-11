#include <iostream>

#include "lattice/lattice_base.h"
#include "lattice/square2d.h"

// #include "random.h"
// #include "hubbard.h"

// #define EIGEN_USE_MKL_ALL
// #define EIGEN_VECTORIZE_SSE4_2
// #include <Eigen/Core>

int main() {    


    Lattice::Square2d mylattice(4);
    mylattice.initial();

    std::cout << mylattice.SpaceDim() << std::endl;
    std::cout << mylattice.SpaceSize() << std::endl;
    std::cout << mylattice.TotalSiteNum() << std::endl;

    std::cout << mylattice.site2index({2,3}) << std::endl;
    auto site = mylattice.index2site(11);
    for (long unsigned int i = 0; i < site.size(); ++i) {
        std::cout << site[i] << std::endl;
    }

    std::cout << mylattice.HoppingMatrix() << std::endl;

    Lattice::LatticeBase* lattice = new Lattice::Square2d(3);
    lattice->initial();
    std::cout << lattice->HoppingMatrix() << std::endl;

    return 0;
}