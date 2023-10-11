#include "nigh/nigh_forward.hpp"
#define BOOST_TEST_MODULE test_0
#define BOOST_TEST_DYN_LINK
#include "ompl/base/ScopedState.h"
#include "ompl/base/spaces/SE3StateSpace.h"
#include "ompl/base/spaces/SO3StateSpace.h"
#include "ompl/datastructures/NearestNeighborsGNAT.h"
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <iostream>

#include "dynkdtree/KDTree.h"

// Include the header file for the space that we use.  Euclidean space
// has an L^2 metric, so we include a space file for general L^p
// metric spaces, which includes the L2Space alias.
#include <nigh/lp_space.hpp>
#include <nigh/so3_space.hpp>

// Nigh support several nearest neighbor strategies, we will use the
// one that benchmarks the fastest and supports concurrent queries and
// inserts.
#include <nigh/kdtree_batch.hpp>

// Our actual points use vectors from the Eigen matrix library, since
// include the necessary header for it.
#include <Eigen/Dense>

// int main() {

// TODO: all the tests from the original code.
// Try SO2.
// Try SO3 -- just using the rectangle seems enough?

BOOST_AUTO_TEST_CASE(t_hello) {

  {
    using tree_t = jk::tree::KDTree<std::string, 2>;
    using point_t = Eigen::Vector2d;
    tree_t tree;
    tree.addPoint(point_t(Eigen::Vector2d(1, 2)), "George");
    tree.addPoint(point_t(Eigen::Vector2d(1, 3)), "Harold");
    tree.addPoint(point_t(Eigen::Vector2d(7, 7)), "Melvin");

    // KNN search
    point_t lazyMonsterLocation(Eigen::Vector2d(
        6, 6)); // this monster will always try to eat the closest people
    const std::size_t monsterHeads =
        2; // this monster can eat two people at once
    auto lazyMonsterVictims = tree.searchKnn(lazyMonsterLocation, monsterHeads);
    for (const auto &victim : lazyMonsterVictims) {
      std::cout << victim.payload << " closest to lazy monster, with distance "
                << sqrt(victim.distance) << "!" << std::endl;
    }
  }
}

// SO2

#if 0
BOOST_AUTO_TEST_CASE(t_hello2) {
  using tree_t =
      jk::tree::KDTree<std::string, 1, 32, double, jk::tree::SO2<double>>;

  using point_t = Eigen::VectorXd;
  using V1d = Eigen::Matrix<double, 1, 1>;
  tree_t tree;
  tree.addPoint(point_t(V1d(0)), "George");
  tree.addPoint(point_t(V1d(3.)), "Harold");
  tree.addPoint(point_t(V1d(-3.)), "Melvin");

  // KNN search
  point_t lazyMonsterLocation(V1d(3.1));
  // 6, 6)); // this monster will always try to eat the closest people
  const std::size_t monsterHeads = 2; // this monster can eat two people at once
  auto lazyMonsterVictims = tree.searchKnn(lazyMonsterLocation, monsterHeads);
  for (const auto &victim : lazyMonsterVictims) {
    std::cout << victim.payload << " closest to lazy monster, with distance "
              << victim.distance << "!" << std::endl;
  }
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_hello3)
{
  std::srand(0);
  using tree_t = jk::tree::KDTree<size_t, 1, 4, double, jk::tree::SO2<double>>;

  using V1d = Eigen::Matrix<double, 1, 1>;
  using point_t = V1d;
  tree_t tree;

  std::vector<point_t> points;
  size_t N = 100;
  for (size_t ii = 0; ii < N; ++ii) {
    double i = M_PI * (-1 + 2 * static_cast<double>(rand()) /
                                static_cast<double>(RAND_MAX));
    points.push_back(point_t(V1d(i)));
    tree.addPoint(point_t(V1d(i)), ii);
  }
  double radius = 1;

  for (size_t jj = 0; jj < 100; ++jj) {
    double j = M_PI * (-1 + 2 * static_cast<double>(rand()) /
                                static_cast<double>(RAND_MAX));
    point_t lazyMonsterLocation((V1d(j)));
    auto lazyMonsterVictims = tree.searchBall(lazyMonsterLocation, radius);
    double min_distance = std::numeric_limits<double>::max();
    int counter = 0;
    for (size_t ii = 0; ii < points.size(); ++ii) {
      double d =
          tree.getDistanceFun().distance(points[ii], lazyMonsterLocation);
      if (d < radius) {
        counter++;
      }
    }

    std::cout << "counter/size: " << counter << " " << lazyMonsterVictims.size()
              << std::endl;
    if (counter != lazyMonsterVictims.size()) {
      std::cout << "Error: " << counter << " " << lazyMonsterVictims.size()
                << std::endl;
      throw std::runtime_error("Error");
    }
  }
}

#endif
BOOST_AUTO_TEST_CASE(t_hello4) {
  std::srand(0);
  using tree_t =
      jk::tree::KDTree<size_t, 3, 4, double, jk::tree::R2SO2Squared<double>>;

  using point_t = Eigen::Vector3d;
  tree_t tree;

  std::vector<point_t> points;
  size_t N = 100;
  for (size_t ii = 0; ii < N; ++ii) {
    point_t v = M_PI * point_t::Random();
    points.push_back(point_t(v));
    tree.addPoint(point_t(v), ii);
  }

  double radius = 1.;

  for (size_t jj = 0; jj < 100; ++jj) {

    point_t vv = M_PI * point_t::Random();
    auto lazyMonsterVictims = tree.searchBall(vv, radius);
    double min_distance = std::numeric_limits<double>::max();
    int counter = 0;
    for (size_t ii = 0; ii < points.size(); ++ii) {
      double d = tree.getDistanceFun().distance(points[ii], vv);
      // double d = 0;
      if (d < radius) {
        counter++;
      }
    }

    std::cout << "counter/size: " << counter << " " << lazyMonsterVictims.size()
              << std::endl;
    if (counter != lazyMonsterVictims.size()) {
      std::cout << "Error: " << counter << " " << lazyMonsterVictims.size()
                << std::endl;
      throw std::runtime_error("Error");
    }
  }
}

BOOST_AUTO_TEST_CASE(bench_run_vs_compile) {
  std::srand(0);
  std::cout << "benchmark in c++" << std::endl;

  using TreeRX = jk::tree::KDTree<int, -1>;
  using TreeR4 = jk::tree::KDTree<int, 4>;

  TreeRX treex(4);
  TreeR4 tree4(-1);

  Eigen::MatrixXd X = Eigen::MatrixXd::Random(10000, 4);

  for (size_t i = 0; i < X.rows(); ++i) {
    treex.addPoint(X.row(i), i);
    tree4.addPoint(X.row(i).head<4>(), i);
  }
  int num_neighs = 10;

  Eigen::Vector4d x = Eigen::Vector4d::Random();

  {
    auto t0 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < 10; i++)
      tree4.searchBall(x, .5);
    // treex.searchKnn(x, num_neighs);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt X:" << dt << std::endl;
  }

  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 10; i++)
      tree4.searchBall(x, .5);
    // tree4.searchKnn(x, num_neighs);

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt 4:" << dt << std::endl;
  }
}

// incremental benchmark

BOOST_AUTO_TEST_CASE(bench_run_vs_compile2) {
  std::srand(0);
  std::cout << "benchmark in c++" << std::endl;

  using TreeRX = jk::tree::KDTree<int, -1>;
  using TreeR4 = jk::tree::KDTree<int, 4>;

  TreeRX treex(4);
  TreeR4 tree4(-1);

  Eigen::VectorXd x = Eigen::VectorXd::Random(4);
  Eigen::Vector4d x4 = x;

  auto X = Eigen::MatrixXd::Random(4, 10000);
  int num_neighs = 10;

  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      treex.searchKnn(X.col(i), num_neighs);
      treex.addPoint(X.col(i), i);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt X:" << dt << std::endl;
  }

  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      tree4.searchKnn(X.col(i), num_neighs);
      tree4.addPoint(X.col(i), i);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt X:" << dt << std::endl;
  }
}

struct MyNode {
  int id;
  Eigen::Vector4d point_;

  MyNode(const int &name, const Eigen::Vector4d &pt) : id(name), point_(pt) {}
};

struct MyNodeKey {
  // The functor should return a constant reference to the data
  // member or a copy of it.
  const Eigen::Vector4d &operator()(const MyNode &node) const {
    return node.point_;
  }
};

struct MyNodeQuat {
  int id;
  Eigen::Quaterniond point_;
  MyNodeQuat(const int &name, const Eigen::Quaterniond &pt)
      : id(name), point_(pt) {}
};

struct MyNodeKeyQuat {
  // The functor should return a constant reference to the data
  // member or a copy of it.
  const Eigen::Quaterniond &operator()(const MyNodeQuat &node) const {
    return node.point_;
  }
};

using namespace unc::robotics;

BOOST_AUTO_TEST_CASE(t_against_nigh) {

  nigh::Nigh<MyNode, nigh::L2Space<double, 4>, MyNodeKey, nigh::NoThreadSafety,
             nigh::KDTreeBatch<32>>
      nn;

  std::srand(0);
  using TreeR4 = jk::tree::KDTree<int, 4>;

  TreeR4 tree4(-1);

  Eigen::VectorXd x = Eigen::VectorXd::Random(4);
  Eigen::Vector4d x4 = x;

  Eigen::MatrixXd X = Eigen::MatrixXd::Random(4, 100000);
  int num_neighs = 10;

  for (size_t i = 0; i < X.cols(); ++i) {
    nn.insert(MyNode(i, X.col(i)));
    tree4.addPoint(X.col(i), i);
  }

  int best = -1;
  {

    std::cout << "linear nn" << std::endl;

    double dist = std::numeric_limits<double>::max();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      double d = (X.col(i) - x).norm();
      if (d < dist) {
        dist = d;
        best = i;
      }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    std::cout << " linear time " << dt << std::endl;
    std::cout << "best is " << best << " with dist " << dist << "dist2 "
              << dist * dist << std::endl;
  }

  int num_experiments = 1;
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    int num_neighs = 10;
    std::vector<std::pair<MyNode, double>> nbh;
    for (size_t i = 0; i < num_experiments; i++) {
      nn.nearest(nbh, x4, num_neighs, 1e8);
    }
    // tree4.searchBall(x, .5);
    // treex.searchKnn(x, num_neighs);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    for (size_t j = 0; j < nbh.size(); ++j) {
      std::cout << nbh[j].first.id << " " << nbh[j].second << std::endl;
    }

    std::cout << "dt nigh:" << dt << std::endl;
  }
  std::vector<TreeR4::DistancePayload> nnt;
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_experiments; i++) {
      nnt = tree4.searchKnn(x, num_neighs);
      // nnt = tree4.searchBall(x, 0.0162168 + 1e-4);
      // 0.0162168
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    for (size_t j = 0; j < nnt.size(); ++j) {
      std::cout << nnt[j].payload << " " << nnt[j].distance << std::endl;
    }
    std::cout << "dt tree:" << dt << std::endl;
  }

  BOOST_TEST(best, nnt[0].payload);

  {

    using TreeR4 = jk::tree::KDTree<int, 4>;
    TreeR4 tree4(-1);
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      auto out = tree4.searchKnn(X.col(i), num_neighs);
      tree4.addPoint(X.col(i), i);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt nigh inc:" << dt << std::endl;
  }
  {

    using TreeR4 = jk::tree::KDTree<int, 4>;
    TreeR4 tree4(-1);
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<MyNode, double>> nbh;
    for (size_t i = 0; i < X.cols(); ++i) {
      nn.insert(MyNode(i, X.col(i)));
      nn.nearest(nbh, x4, num_neighs, 1e8);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "dt tree inc:" << dt << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(t_against_nigh_so3) {
  // compare the two SO3 Implementations

  std::srand(0);
  nigh::Nigh<MyNodeQuat, nigh::SO3Space<double>, MyNodeKeyQuat,
             nigh::NoThreadSafety, nigh::KDTreeBatch<32>>
      nn;

  // using TreeQuat = jk::tree::KDTree<int, 4, 32, double,
  // jk::tree::SO3<double>>;
  using TreeQuat =
      jk::tree::KDTree<int, 4, 32, double, jk::tree::SO3Squared<double>>;

  TreeQuat tree4(-1);

  Eigen::VectorXd x = Eigen::VectorXd::Random(4);
  Eigen::Vector4d x4 = x;
  x4.normalize();

  Eigen::MatrixXd X = Eigen::MatrixXd::Random(4, 1000000);

  bool real_part_positive = true;
  int index_real_part = 3;

  if (real_part_positive) {
    if (x4(index_real_part) < 0) {
      x4 *= -1;
    }
  }

  for (size_t i = 0; i < X.cols(); ++i) {
    X.col(i).normalize();
    if (real_part_positive) {
      if (X.col(i)(index_real_part) < 0) {
        X.col(i) *= -1;
      }
    }
  }

  int num_neighs = 10;

  for (size_t i = 0; i < X.cols(); ++i) {
    Eigen::Vector4d q = X.col(i);
    nn.insert(MyNodeQuat(i, Eigen::Quaterniond(q)));
    tree4.addPoint(q, i);
  }

  int best = -1;
  {

    std::cout << "linear nn" << std::endl;

    double dist = std::numeric_limits<double>::max();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      double d = std::min((X.col(i) - x4).norm(), (X.col(i) + x4).norm());
      if (d < dist) {
        dist = d;
        best = i;
      }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    std::cout << " linear time " << dt << std::endl;
    std::cout << "best is " << best << " with dist " << dist << "dist2 "
              << dist * dist << std::endl;
  }

  std::cout << "quaternion" << std::endl;
  int num_experiments = 1;
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<MyNodeQuat, double>> nbh;
    for (size_t i = 0; i < num_experiments; i++) {
      nn.nearest(nbh, x4, num_neighs, 1e8);
    }
    // tree4.searchBall(x, .5);
    // treex.searchKnn(x, num_neighs);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    for (size_t j = 0; j < nbh.size(); ++j) {
      std::cout << nbh[j].first.id << " "
                << nbh[j].first.point_.coeffs().transpose() << " "
                << nbh[j].second << std::endl;
    }
    std::cout << "dt nigh:" << dt << std::endl;
  }
  std::vector<TreeQuat::DistancePayload> nnt;
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_experiments; i++) {
      // nnt = tree4.searchKnn(x4, num_neighs);
      nnt = tree4.searchBall(x4, 0.000695807 + 1e-8);
      // 0.0263781 + 1e-5);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    for (size_t j = 0; j < nnt.size(); ++j) {
      std::cout << nnt[j].payload << " " << nnt[j].distance << std::endl;
    }

    std::cout << "dt tree:" << dt << std::endl;
  }

  std::cout << "best: " << best << " " << nnt[0].payload << std::endl;
  BOOST_TEST(best == nnt[0].payload);

  // ompl
  {
    ompl::base::StateSpacePtr space(new ompl::base::SO3StateSpace());

    std::vector<ompl::base::State *> states;

    for (size_t i = 0; i < X.cols(); ++i) {
      ompl::base::State *state = space->allocState();

      auto ptr = state->as<ompl::base::SO3StateSpace::StateType>();
      ptr->x = X.col(i)(0);
      ptr->y = X.col(i)(1);
      ptr->z = X.col(i)(2);
      ptr->w = X.col(i)(3);
      states.push_back(state);
    }

    // state->setY(0.2);
    // state->setYaw(0.0);

    ompl::NearestNeighbors<ompl::base::State *> *tt =
        new ompl::NearestNeighborsGNAT<ompl::base::State *>();

    tt->setDistanceFunction(
        [&](auto &a, auto &b) { return space->distance(a, b); });

    for (auto &s : states)
      tt->add(s);

    ompl::base::State *query = space->allocState();

    auto ptr = query->as<ompl::base::SO3StateSpace::StateType>();
    ptr->x = x4(0);
    ptr->y = x4(1);
    ptr->z = x4(2);
    ptr->w = x4(3);

    std::vector<ompl::base::State *> nbh;

    // free memory...
    // tt->nearestK(query, num_neighs, nbh);
    // ompl tree:5.4935e-05

    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_experiments; i++) {
      // tt->nearestR(query, 0.0263789 + 1e-5, nbh);
      tt->nearestK(query, 10, nbh);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "nns " << std::endl;
    for (size_t j = 0; j < nbh.size(); ++j) {
      space->printState(nbh[j]);
      std::cout << tt->getDistanceFunction()(nbh[j], query) << std::endl;
    }
    //
    std::cout << "ompl tree:" << dt << std::endl;

    for (size_t i = 0; i < states.size(); ++i) {
      space->freeState(states[i]);
    }

    space->freeState(query);

    // t

    // ompl::base::SO3StateSpace space;
    //
    // using state_t = ompl::base::ScopedState<ompl::base::SO3StateSpace> ;
  }
}

// 20 times faster...
BOOST_AUTO_TEST_CASE(t_se3) {

  std::srand(0);
  // nigh::Nigh<MyNodeQuat, nigh::SO3Space<double>, MyNodeKeyQuat,
  //            nigh::NoThreadSafety, nigh::KDTreeBatch<32>>
  // nn;

  // using TreeQuat = jk::tree::KDTree<int, 4, 32, double,
  // jk::tree::SO3<double>>;
  using TreeR3SO3 =
      jk::tree::KDTree<int, 7, 32, double, jk::tree::R3SO3<double>>;

  using TreeR3SO3X =
      jk::tree::KDTree<int, -1, 32, double, jk::tree::Combined<double>>;

  // template <class Payload, int Dimensions, std::size_t BucketSize = 32,
  //           typename Scalar = double,
  //           typename Distance = L2Squared<Scalar, Dimensions>>
  //

  // SE3Squared<double>>;
  // SO3Squared<double>>;

  TreeR3SO3 tree(-1);

  using Space = jk::tree::Combined<double>::Space;
  //
  // std::vector<Space> spaces;

  // TODO: test this!! How i am going to give this as input? -- it is not a

  std::vector<Space> spaces;
  spaces.push_back(jk::tree::L2<double>());
  spaces.push_back(jk::tree::SO3<double>());
  // jk::tree::L2<double>());
  //
  // {jk::tree::L2<double>(), jk::tree::SO3<double>()}

  // jk::tree::Combined<double> combi_space(spaces, {3, 4});
  jk::tree::Combined<double> combi_space({"L2:3", "SO3"});
  // spaces, {3, 4});
  TreeR3SO3X treex(7, combi_space);

  int nx = 7;
  Eigen::VectorXd x = Eigen::VectorXd::Random(nx);
  using V7d = Eigen::Matrix<double, 7, 1>;
  V7d x7;
  x7 = x;
  x7.tail(4).normalize();

  int num_points = 1000000;
  Eigen::MatrixXd X = Eigen::MatrixXd::Random(nx, num_points);

  bool real_part_positive = true;
  int index_real_part = 3;

  if (real_part_positive) {
    if (x7.tail(4)(index_real_part) < 0) {
      x7.tail(4) *= -1;
    }
  }

  for (size_t i = 0; i < X.cols(); ++i) {
    X.col(i).tail(4).normalize();
    if (real_part_positive) {
      if (X.col(i).tail(4)(index_real_part) < 0) {
        X.col(i).tail(4) *= -1;
      }
    }
  }

  int num_neighs = 10;

  for (size_t i = 0; i < X.cols(); ++i) {
    V7d q = X.col(i);
    tree.addPoint(q, i);
    treex.addPoint(q, i);
  }

  int best = -1;
  {

    std::cout << "linear nn" << std::endl;

    double dist = std::numeric_limits<double>::max();
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < X.cols(); ++i) {
      double d = tree.getDistanceFun().distance(X.col(i), x7.head<7>());
      if (d < dist) {
        dist = d;
        best = i;
      }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    std::cout << " linear time " << dt << std::endl;
    std::cout << "best is " << best << " with dist " << dist << "dist2 "
              << dist * dist << std::endl;
  }

  int num_experiments = 1;
#if 0
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<MyNodeQuat, double>> nbh;
    for (size_t i = 0; i < num_experiments; i++) {
      nn.nearest(nbh, x4, num_neighs, 1e8);
    }
    // tree4.searchBall(x, .5);
    // treex.searchKnn(x, num_neighs);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    for (size_t j = 0; j < nbh.size(); ++j) {
      std::cout << nbh[j].first.id << " "
                << nbh[j].first.point_.coeffs().transpose() << " "
                << nbh[j].second << std::endl;
    }
    std::cout << "dt nigh:" << dt << std::endl;
  }
#endif

  double radius_search = .5;
  std::vector<TreeR3SO3::DistancePayload> nnt;
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_experiments; i++) {
      // nnt = tree.searchKnn(x7, num_neighs);
      nnt = tree.searchBall(x7, radius_search);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    for (size_t j = 0; j < nnt.size(); ++j) {
      std::cout << nnt[j].payload << " " << nnt[j].distance << std::endl;
    }

    std::cout << "my tree:" << dt << std::endl;
  }

  std::cout << "best: " << best << " " << nnt[0].payload << std::endl;
  BOOST_TEST(best == nnt[0].payload);

  // my tree dynamic
  {
    std::vector<TreeR3SO3X::DistancePayload> nntx;
    {

      // std::cout << tree.getDistanceFun().distance(x7, x7) <<std::endl;
      // std::cout << tree.getDistanceFun().distance(x7, X.col(0)) <<std::endl;
      std::cout << treex.getDistanceFun().distance(x7, X.col(309717))
                << std::endl;
      std::cout << treex.getDistanceFun().distanceToRect(x7, X.col(309717),
                                                         X.col(309717))
                << std::endl;
      // 118527)) <<std::endl;

      auto t0 = std::chrono::high_resolution_clock::now();
      for (size_t i = 0; i < num_experiments; i++) {
        // nnt = tree.searchKnn(x7, num_neighs);
        nntx = treex.searchBall(x7, radius_search);
      }

      auto t1 = std::chrono::high_resolution_clock::now();
      auto dt =
          1.e-9 *
          std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

      for (size_t j = 0; j < nnt.size(); ++j) {
        std::cout << nntx[j].payload << " " << nntx[j].distance << std::endl;
      }

      std::cout << "my tree dynamic:" << dt << std::endl;
    }

    std::cout << "best: " << best << " " << nnt[0].payload << std::endl;
    BOOST_TEST(best == nntx[0].payload);

    for (size_t k = 0; k < std::min(int(nntx.size()), 5); k++) {
      BOOST_TEST(nntx[0].payload == nnt[0].payload);
      BOOST_TEST(nntx[0].distance == nnt[0].distance,
                 boost::test_tools::tolerance(1e-6));
    }
  }

  // ompl
  {
    ompl::base::StateSpacePtr space(new ompl::base::SE3StateSpace());

    std::vector<ompl::base::State *> states;

    for (size_t i = 0; i < X.cols(); ++i) {
      ompl::base::State *state = space->allocState();

      auto ptr = state->as<ompl::base::SE3StateSpace::StateType>();
      ptr->setXYZ(X.col(i)(0), X.col(i)(1), X.col(i)(2));
      ptr->rotation().x = X.col(i)(3 + 0);
      ptr->rotation().y = X.col(i)(3 + 1);
      ptr->rotation().z = X.col(i)(3 + 2);
      ptr->rotation().w = X.col(i)(3 + 3);
      states.push_back(state);
    }

    // state->setY(0.2);
    // state->setYaw(0.0);

    ompl::NearestNeighbors<ompl::base::State *> *tt =
        new ompl::NearestNeighborsGNAT<ompl::base::State *>();

    tt->setDistanceFunction(
        [&](auto &a, auto &b) { return space->distance(a, b); });

    for (auto &s : states)
      tt->add(s);

    ompl::base::State *query = space->allocState();

    auto ptr = query->as<ompl::base::SE3StateSpace::StateType>();

    ptr->setXYZ(x7(0), x7(1), x7(2));
    ptr->rotation().x = x7(3 + 0);
    ptr->rotation().y = x7(3 + 1);
    ptr->rotation().z = x7(3 + 2);
    ptr->rotation().w = x7(3 + 3);

    std::vector<ompl::base::State *> nbh;

    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_experiments; i++) {
      // tt->nearestK(query, num_neighs, nbh);
      tt->nearestR(query, radius_search, nbh);
      // .num_neighs, nbh);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt =
        1.e-9 *
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "nns " << std::endl;
    for (size_t j = 0; j < nbh.size(); ++j) {
      space->printState(nbh[j]);
      std::cout << tt->getDistanceFunction()(nbh[j], query) << std::endl;
    }
    //
    std::cout << "ompl tree:" << dt << std::endl;

    for (size_t i = 0; i < states.size(); ++i) {
      space->freeState(states[i]);
    }
    space->freeState(query);
  }

  // TODO: incremental benchmark
}

BOOST_AUTO_TEST_CASE(t_original_example) {
  std::cout << "Example starting..." << std::endl;
  // setup
  using tree_t = jk::tree::KDTree<std::string, 2>;
  using point_t = Eigen::Vector2d;
  tree_t tree;
  tree.addPoint(point_t(1, 2), "George");
  tree.addPoint(point_t(1, 3), "Harold");
  tree.addPoint(point_t(7, 7), "Melvin");

  // KNN search
  point_t lazyMonsterLocation(
      6, 6); // this monster will always try to eat the closest people
  const std::size_t monsterHeads = 2; // this monster can eat two people at once
  auto lazyMonsterVictims = tree.searchKnn(lazyMonsterLocation, monsterHeads);
  for (const auto &victim : lazyMonsterVictims) {
    std::cout << victim.payload << " closest to lazy monster, with distance "
              << sqrt(victim.distance) << "!" << std::endl;
  }

  // ball search
  point_t stationaryMonsterLocation(
      8, 8); // this monster doesn't move, so can only eat people that are close
  const double neckLength = 6.0; // it can only reach within this range
  auto potentialVictims =
      tree.searchBall(stationaryMonsterLocation,
                      neckLength * neckLength); // metric is SquaredL2
  std::cout << "Stationary monster can reach any of " << potentialVictims.size()
            << " people!" << std::endl;

  // hybrid KNN/ball search
  auto actualVictims = tree.searchCapacityLimitedBall(
      stationaryMonsterLocation, neckLength * neckLength, monsterHeads);
  std::cout << "The stationary monster will try to eat ";
  for (const auto &victim : actualVictims) {
    std::cout << victim.payload << " and ";
  }
  std::cout << "nobody else." << std::endl;
  std::cout << "Example completed" << std::endl;
}

BOOST_AUTO_TEST_CASE(t_orig_accuracy) {
  // GIVEN: a tree, a bunch of random points to put in it, and dumb brute force
  // methods to compare results to

  std::cout << "Accuracy tests starting..." << std::endl;
  static const int dims = 4;
  std::vector<Eigen::Vector4d> points;
  using tree_t = jk::tree::KDTree<int, dims>;
  tree_t tree;
  int count = 0;
  std::srand(1234567);

  auto bruteforceKNN = [&](const Eigen::Vector4d searchLoc,
                           size_t K) -> std::vector<std::pair<double, int>> {
    std::vector<std::pair<double, int>> dists;
    for (std::size_t i = 0; i < points.size(); i++) {
      double distance = tree.getDistanceFun().distance(searchLoc, points[i]);
      dists.emplace_back(distance, i);
    }
    size_t actualK = std::min(points.size(), K);
    std::partial_sort(dists.begin(), dists.begin() + actualK, dists.end());
    dists.resize(actualK);
    return dists;
  };

  auto bruteforceRadius =
      [&](const Eigen::Vector4d &searchLoc,
          double radius) -> std::vector<std::pair<double, int>> {
    std::vector<std::pair<double, int>> dists;
    for (std::size_t i = 0; i < points.size(); i++) {
      double distance = tree.getDistanceFun().distance(searchLoc, points[i]);
      dists.emplace_back(distance, i);
    }
    std::sort(dists.begin(), dists.end());
    auto iter = std::lower_bound(dists.begin(), dists.end(),
                                 std::make_pair(radius, int(0)));
    std::size_t inliers = std::distance(dists.begin(), iter);
    dists.resize(inliers);
    return dists;
  };

  auto randomPoint = []() {
    // std::array<double, dims> loc;
    Eigen::Vector4d loc;
    for (std::size_t j = 0; j < dims; j++) {
      loc[j] = double(rand()) / RAND_MAX;
    }
    return loc;
  };

  // THEN: the tree size should match
  if (tree.size() != 0) {
    std::cout << "Count doesn't match!!!" << std::endl;
  }

  auto searcher = tree.searcher();
  for (std::size_t j = 0; j < 2000; j++) {
    Eigen::Vector4d loc = randomPoint();

    // WHEN: we search for the KNN with the tree and the brute force
    const std::size_t k = 50;
    auto tnn = tree.searchKnn(loc, k);
    auto bnn = bruteforceKNN(loc, k);
    auto snn = searcher.search(loc, 1e9, k, tree.getDistanceFun());

    // THEN: the returned result sizes should match
    if (tnn.size() != bnn.size() || snn.size() != bnn.size() ||
        bnn.size() > std::min(k, points.size())) {
      std::cout << "Searched for " << k << ", found " << tnn.size()
                << std::endl;
    }

    if (bnn.size() > 0) {
      auto nn = tree.search(loc);
      if (nn.payload != bnn[0].second) {
        std::cout << "1nn payloads not equal" << std::endl;
      }
      if (std::abs(bnn[0].first - nn.distance) > 1e-10) {
        std::cout << "1nn distances not equal" << std::endl;
      }
    }

    // AND: the entries should match - both index, and distance
    for (std::size_t i = 0; i < tnn.size(); i++) {
      if (std::abs(bnn[i].first - tnn[i].distance) > 1e-10) {
        std::cout << "distances not equal" << std::endl;
      }
      if (std::abs(bnn[i].first - snn[i].distance) > 1e-10) {
        std::cout << "distances not equal" << std::endl;
      }
      if (bnn[i].second != tnn[i].payload) {
        std::cout << "payloads not equal" << std::endl;
      }
      if (bnn[i].second != snn[i].payload) {
        std::cout << "payloads not equal" << std::endl;
      }
    }

    // WHEN: we add the point we searched for to the tree for next time
    tree.addPoint(loc, count++);
    points.push_back(loc);

    // THEN: the tree size should match
    if (tree.size() != points.size()) {
      std::cout << "Count doesn't match!!!" << std::endl;
    }
  }

  tree_t tree2;

  for (std::size_t j = 0; j < points.size(); j++) {
    tree2.addPoint(points[j], j, false);
  }
  tree2.splitOutstanding();

  for (std::size_t j = 0; j < points.size(); j++) {
    Eigen::Vector4d loc = randomPoint();
    const double radius = 0.7;

    auto tnn = tree2.searchBall(loc, radius);
    auto bnn = bruteforceRadius(loc, radius);
    if (tnn.size() != bnn.size()) {
      std::cout << "Brute force results are not the same size as tree results"
                << std::endl;
      continue;
    }

    if (tnn.size() && tnn.back().distance > radius) {
      std::cout << "Searched for max radius " << radius << ", found "
                << tnn.back().distance << std::endl;
    }
    for (std::size_t i = 0; i < tnn.size(); i++) {
      if (std::abs(bnn[i].first - tnn[i].distance) > 1e-10) {
        std::cout << "distances not equal" << std::endl;
        BOOST_TEST(false);
      }
      if (bnn[i].second != tnn[i].payload) {
        std::cout << "payloads not equal" << std::endl;
        BOOST_TEST(false);
      }
    }
  }

  std::cout << "Accuracy tests completed" << std::endl;
}

BOOST_AUTO_TEST_CASE(t_orig_duplicate) {
  std::cout << "Duplicate tests started" << std::endl;
  std::srand(0);

  // GIVEN: the same point added to the tree lots and lots of times (multiple
  // buckets worth)
  static const int dims = -1;
  static const int runtime_dim = 11;

  using point_t = Eigen::VectorXd;

  auto randomPoint = []() {
    point_t loc(runtime_dim);
    // std::array<double, dims> loc;
    for (std::size_t j = 0; j < runtime_dim; j++) {
      loc[j] = double(rand()) / RAND_MAX;
    }
    return loc;
  };
  using tree_t = jk::tree::KDTree<int, dims>;
  tree_t tree(runtime_dim);

  point_t loc = randomPoint();
  for (int i = 0; i < 5000; i++) {
    tree.addPoint(loc, i, false);
  }
  auto almostLoc = loc;
  almostLoc[0] = std::nextafter(loc[0], 1e9);
  tree.addPoint(
      almostLoc, tree.size(),
      false); // and another point, just so not the entire treee is one point

  // WHEN: the tree is split and queried
  tree.splitOutstanding();
  auto tnn = tree.searchKnn(loc, 80);

  // THEN: it should still behave normally - correct K for KNN, no crashes, good
  // code coverage, etc
  if (tnn.size() != 80) {
    std::cout << "Incorrect K: " << tnn.size() << std::endl;
    BOOST_TEST(false);
  }
  std::cout << "Duplicate tests completed" << std::endl;
}

#define DURATION                                                               \
  double(((previous = current) * 0 + (current = std::clock()) - previous) /    \
         double(CLOCKS_PER_SEC))

BOOST_AUTO_TEST_CASE(t_orig_performance) {

  std::cout << "Performance tests starting..." << std::endl;
  std::clock_t previous = std::clock(), current = previous;

  static const int dims = 2;
  std::cout << "adding ";
  using point_t = Eigen::Vector2d;
  std::vector<point_t> points;
  jk::tree::KDTree<int, dims, 8> tree;

  int count = 0;
  std::srand(1234567);

  auto randomPoint = []() {
    point_t loc;
    for (std::size_t j = 0; j < dims; j++) {
      loc[j] = double(rand()) / RAND_MAX;
    }
    return loc;
  };

  for (int i = 0; i < 400 * 1000; i++) {
    point_t loc = randomPoint();
    tree.addPoint(loc, count++, false);

    points.push_back(loc);
  }

  std::vector<point_t> searchPoints;
  for (int i = 0; i < 100 * 1000; i++) {
    searchPoints.push_back(randomPoint());
  }

  std::cout << DURATION << "s" << std::endl;
  std::cout << "splitting ";
  tree.splitOutstanding();
  std::cout << DURATION << "s" << std::endl;
  for (int j = 0; j < 3; j++) {
    std::cout << "searching " << (j + 1) << " ";

    for (auto p : searchPoints) {
      const int k = 3;
      auto nn = tree.searchKnn(p, k);

      if (nn.size() != k) {
        std::cout << nn.size() << " instead of " << k << " ERROR" << std::endl;
      }
    }
    std::cout << DURATION << "s" << std::endl;
  }
  for (int j = 0; j < 3; j++) {
    std::cout << "bulk searching " << (j + 1) << " ";

    const int k = 3;
    auto searcher = tree.searcher();
    for (auto p : searchPoints) {
      const auto &nn = searcher.search(p, std::numeric_limits<double>::max(), k,
                                       tree.getDistanceFun());

      if (nn.size() != k) {
        std::cout << nn.size() << " instead of " << k << " ERROR" << std::endl;
      }
    }
    std::cout << DURATION << "s" << std::endl;
  }
  std::cout << "Performance tests completed" << std::endl;
}
