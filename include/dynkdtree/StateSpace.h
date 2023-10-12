
#include <algorithm>
#include <array>
#include <cmath>
#include <cwchar>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <set>
#include <variant>
#include <vector>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

namespace dynotree {
template <typename T, typename Scalar>
void choose_split_dimension_default(const T &lb, const T &ub, int &ii,
                                    Scalar &width) {
  for (std::size_t i = 0; i < lb.size(); i++) {
    Scalar dWidth = ub[i] - lb[i];
    if (dWidth > width) {
      ii = i;
      width = dWidth;
    }
  }
}

template <typename Scalar, int Dimensions = -1> struct RnL1 {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, Dimensions, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, Dimensions, 1>>;

  Eigen::Matrix<Scalar, Dimensions, 1> lb;
  Eigen::Matrix<Scalar, Dimensions, 1> ub;

  void set_bounds(cref_t lb_, cref_t ub_) {
    lb = lb_;
    ub = ub_;
  }

  inline void sample_uniform(ref_t x) const {
    x.setRandom();
    x.array() += 1;
    x /= .2;
    x = lb + (ub - lb).cwiseProduct(x);
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    assert(t >= 0);
    assert(t <= 1);
    out = from + t * (to - from);
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    Scalar d = 0;
    Scalar dist = 0;

    if constexpr (Dimensions == Eigen::Dynamic) {

      assert(x.size());
      assert(ub.size());
      assert(lb.size());
      assert(x.size() == ub.size());
      assert(x.size() == lb.size());

      for (size_t i = 0; i < x.size(); i++) {
        Scalar xx = std::max(lb(i), std::min(ub(i), x(i)));
        Scalar dif = xx - x(i);
        dist += std::abs(dif);
      }
    } else {
      for (size_t i = 0; i < Dimensions; i++) {
        Scalar xx = std::max(lb(i), std::min(ub(i), x(i)));
        Scalar dif = xx - x(i);
        dist += std::abs(dif);
      }
    }
    return dist;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    assert(x.size());
    assert(y.size());
    return (x - y).cwiseAbs().sum();
  }
};

template <typename Scalar> struct Time {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 1, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 1, 1>>;
  Eigen::Matrix<Scalar, 1, 1> lb;
  Eigen::Matrix<Scalar, 1, 1> ub;

  void sample_uniform(ref_t x) const {
    assert(lb(0) >= 0);
    x(0) = double(rand()) / RAND_MAX * (ub(0) - lb(0)) + lb(0);
  }

  void set_bounds(cref_t lb_, cref_t ub_) {
    assert(lb_.size() == 1);
    assert(ub_.size() == 1);
    assert(ub(0) >= lb(0));
    lb = lb_;
    ub = ub_;
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {

    assert(t >= 0);
    assert(t <= 1);
    assert(to(0) >= from(0));

    Eigen::Matrix<Scalar, 1, 1> d = to - from;
    out = from + t * d;
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  // if the ub is small, return inf
  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    if (ub(0) < x(0)) {
      return std::numeric_limits<Scalar>::max();
    } else if (x(0) > lb(0)) {
      return 0;
    } else {
      return lb(0) - x(0);
    }
  }

  // from x to y
  // if y is smaller, return inf
  inline Scalar distance(cref_t x, cref_t y) const {

    if (y(0) < x(0)) {
      return std::numeric_limits<Scalar>::max();
    } else {
      return y(0) - x(0);
    }
  }
};
template <typename Scalar, int Dimensions> struct RnTime {

  // TODO: continue here!!
};

template <typename Scalar> struct SO2 {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 1, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 1, 1>>;

  void sample_uniform(ref_t x) const {

    x(0) = (double(rand()) / (RAND_MAX + 1.)) * 2. * M_PI - M_PI;
  }

  void set_bounds(cref_t lb_, cref_t ub_) {
    std::stringstream ss;
    ss << "so2 has no bounds " << __FILE__ << ":" << __LINE__;
    throw std::runtime_error(ss.str());
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {

    assert(t >= 0);
    assert(t <= 1);

    Eigen::Matrix<Scalar, 1, 1> d = to - from;

    if (d(0) > M_PI) {
      d(0) -= 2 * M_PI;
    } else if (d(0) < -M_PI) {
      d(0) += 2 * M_PI;
    }

    out = from + t * d;

    if (out(0) > M_PI) {
      out(0) -= 2 * M_PI;
    } else if (out(0) < -M_PI) {
      out(0) += 2 * M_PI;
    }
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    assert(x(0) >= -M_PI);
    assert(x(0) <= M_PI);

    assert(lb(0) >= -M_PI);
    assert(lb(0) <= M_PI);

    assert(ub(0) >= -M_PI);
    assert(ub(0) <= M_PI);

    if (x(0) >= lb(0) && x(0) <= ub(0)) {
      return 0;
    } else if (x(0) > ub(0)) {
      Scalar d1 = x(0) - ub(0);
      Scalar d2 = lb(0) - (x(0) - 2 * M_PI);
      assert(d2 >= 0);
      assert(d1 >= 0);
      return std::min(d1, d2);
    } else if (x(0) < lb(0)) {
      Scalar d1 = lb(0) - x(0);
      Scalar d2 = (x(0) + 2 * M_PI) - ub(0);
      assert(d2 >= 0);
      assert(d1 >= 0);
      return std::min(d1, d2);
    } else {
      assert(false);
      return 0;
    }
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    assert(x(0) >= -M_PI);
    assert(y(0) >= -M_PI);

    assert(x(0) <= M_PI);
    assert(y(0) <= M_PI);

    Scalar dif = x(0) - y(0);
    if (dif > M_PI) {
      dif -= 2 * M_PI;
    } else if (dif < -M_PI) {
      dif += 2 * M_PI;
    }
    Scalar out = std::abs(dif);
    return out;
  }
};

template <typename Scalar> struct SO2Squared {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 1, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 1, 1>>;

  Eigen::Matrix<Scalar, 1, 1> lb;
  Eigen::Matrix<Scalar, 1, 1> ub;

  SO2<Scalar> so2;

  inline void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    so2.interpolate(from, to, t, out);
  }

  inline void sample_uniform(ref_t x) const { so2.sample_uniform(x); }

  inline void set_bounds(cref_t lb_, cref_t ub_) {
    std::stringstream ss;
    ss << "so2 has no bounds " << __FILE__ << ":" << __LINE__;
    throw std::runtime_error(ss.str());
  }

  inline void choose_split_dimension(cref_t lb, cref_t ub, int &ii,
                                     Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    Scalar d = so2.distance_to_rectangle(x, lb, ub);
    return d * d;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    Scalar d = so2.distance(x, y);
    return d * d;
  }
};

template <typename Scalar, int Dimensions = -1> struct RnSquared {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, Dimensions, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, Dimensions, 1>>;

  Eigen::Matrix<Scalar, Dimensions, 1> lb;
  Eigen::Matrix<Scalar, Dimensions, 1> ub;

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    assert(t >= 0);
    assert(t <= 1);
    out = from + t * (to - from);
  }

  void set_bounds(cref_t lb_, cref_t ub_) {
    lb = lb_;
    ub = ub_;
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii,
                              Scalar &width) const {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  void sample_uniform(ref_t x) const {

    x.setRandom();
    x.array() += 1;
    x /= .2;
    x = lb + (ub - lb).cwiseProduct(x);

    //
    // x.setRandom();
    // x.array() += 1;
    // x /= .2;
    // x = lb + (ub - lb).cwiseProduct(x);
    // x.setRandom();
    // x = lb + (ub - lb).cwiseProduct(x.array() + 1.) / 2.;
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    Scalar d = 0;
    Scalar dist = 0;

    if constexpr (Dimensions == Eigen::Dynamic) {

      assert(x.size());
      assert(ub.size());
      assert(lb.size());
      assert(x.size() == ub.size());
      assert(x.size() == lb.size());

      for (size_t i = 0; i < x.size(); i++) {
        Scalar xx = std::max(lb(i), std::min(ub(i), x(i)));
        Scalar dif = xx - x(i);
        dist += dif * dif;
      }
    } else {
      for (size_t i = 0; i < Dimensions; i++) {
        Scalar xx = std::max(lb(i), std::min(ub(i), x(i)));
        Scalar dif = xx - x(i);
        dist += dif * dif;
      }
    }

    // Issue: memory allocation
    // Eigen::Matrix<Scalar, Dimensions, 1> bb = x;
    // dist = (x - bb.cwiseMax(lb).cwiseMin(ub)).squaredNorm();

    return dist;
  }

  inline Scalar distance(cref_t &x, cref_t &y) const {

    assert(x.size());
    assert(y.size());

    // if constexpr (Dimensions == Eigen::Dynamic) {
    //   std::cout << "dynamic" << std::endl;
    // } else {
    //   std::cout << "" << std::endl;
    // }

    return (x - y).squaredNorm();
  }
};

template <typename Scalar, int Dimensions = -1> struct Rn {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, Dimensions, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, Dimensions, 1>>;

  RnSquared<Scalar, Dimensions> rn_squared;
  Eigen::Matrix<Scalar, Dimensions, 1> lb;
  Eigen::Matrix<Scalar, Dimensions, 1> ub;

  void set_bounds(cref_t lb_, cref_t ub_) {

    std::cout << "setting bounds " << std::endl;
    std::cout << lb_.transpose() << std::endl;
    std::cout << ub_.transpose() << std::endl;
    lb = lb_;
    ub = ub_;
  }

  inline void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    assert(t >= 0);
    assert(t <= 1);
    out = from + t * (to - from);
  }

  inline void sample_uniform(ref_t x) const {

    std::cout << "bounds are" << std::endl;
    std::cout << lb.transpose() << std::endl;
    std::cout << ub.transpose() << std::endl;

    x.setRandom();
    x.array() += 1;
    x /= 2.;
    x = lb + (ub - lb).cwiseProduct(x);

    // x.setRandom();
    // x = lb + (ub - lb).cwiseProduct(x.array() + 1.) / 2.;
  }

  inline void choose_split_dimension(cref_t lb, cref_t ub, int &ii,
                                     Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    Scalar d = rn_squared.distance_to_rectangle(x, lb, ub);
    return std::sqrt(d);
  };

  inline Scalar distance(cref_t &x, cref_t &y) const {
    Scalar d = rn_squared.distance(x, y);
    return std::sqrt(d);
  }
};

template <typename Scalar> struct R2SO2 {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 3, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 3, 1>>;

  using cref2_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 2, 1>> &;
  using ref2_t = Eigen::Ref<Eigen::Matrix<Scalar, 2, 1>>;

  // Eigen::Matrix<Scalar, 3, 1> lb;
  // Eigen::Matrix<Scalar, 3, 1> ub;

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  Scalar angular_weight = 1.0;

  Rn<Scalar, 2> l2;
  SO2<Scalar> so2;

  void set_bounds(cref2_t lb_, cref2_t ub_) { l2.set_bounds(lb_, ub_); }

  inline void sample_uniform(ref_t x) const {
    l2.sample_uniform(x.template head<2>());
    so2.sample_uniform(x.template tail<1>());
  }

  inline void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    assert(t >= 0);
    assert(t <= 1);
    l2.interpolate(from.template head<2>(), to.template head<2>(), t,
                   out.template head<2>());
    so2.interpolate(from.template tail<1>(), to.template tail<1>(), t,
                    out.template tail<1>());
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    Scalar d1 = l2.distance_to_rectangle(
        x.template head<2>(), lb.template head<2>(), ub.template head<2>());
    Scalar d2 = so2.distance_to_rectangle(
        x.template tail<1>(), lb.template tail<1>(), ub.template tail<1>());
    return d1 + angular_weight * d2;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    Scalar d1 = l2.distance(x.template head<2>(), y.template head<2>());
    Scalar d2 = so2.distance(x.template tail<1>(), y.template tail<1>());
    return d1 + angular_weight * d2;
  };
};

template <typename Scalar> struct R2SO2Squared {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 3, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 3, 1>>;

  using cref2_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 2, 1>> &;
  using ref2_t = Eigen::Ref<Eigen::Matrix<Scalar, 2, 1>>;

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  Scalar angular_weight = 1.0;

  RnSquared<Scalar, 2> rn_squared;
  SO2Squared<Scalar> so2squared;

  void set_bounds(cref2_t lb_, cref2_t ub_) { rn_squared.set_bounds(lb_, ub_); }

  void sample_uniform(ref_t x) const {
    rn_squared.sample_uniform(x.template head<2>());
    so2squared.sample_uniform(x.template tail<1>());
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    Scalar d1 = rn_squared.distance_to_rectangle(
        x.template head<2>(), lb.template head<2>(), ub.template head<2>());
    Scalar d2 = so2squared.distance_to_rectangle(
        x.template tail<1>(), lb.template tail<1>(), ub.template tail<1>());
    return d1 + angular_weight * d2;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    Scalar d1 = rn_squared.distance(x.template head<2>(), y.template head<2>());
    Scalar d2 = so2squared.distance(x.template tail<1>(), y.template tail<1>());
    return d1 + angular_weight * d2;
  };
};

template <typename Scalar> struct SO3Squared {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 4, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 4, 1>>;

  RnSquared<Scalar, 4> rn_squared;

  void sample_uniform(ref_t x) const {
    x = Eigen::Quaterniond().UnitRandom().coeffs();
  }

  void set_bounds(cref_t lb_, cref_t ub_) {
    std::stringstream ss;
    ss << "so3 has no bounds " << __FILE__ << ":" << __LINE__;
    throw std::runtime_error(ss.str());
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    throw std::runtime_error("not implemented interpolate in so3");
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    assert(std::abs(x.norm() - 1) < 1e-6);

    Scalar d1 = rn_squared.distance_to_rectangle(x, lb, ub);
    Scalar d2 = rn_squared.distance_to_rectangle(-1. * x, lb, ub);
    return std::min(d1, d2);
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    assert(x.size() == 4);
    assert(y.size() == 4);

    assert(std::abs(x.norm() - 1) < 1e-6);
    assert(std::abs(y.norm() - 1) < 1e-6);

    Scalar d1 = rn_squared.distance(x, y);
    Scalar d2 = rn_squared.distance(-x, y);
    return std::min(d1, d2);
  };
};

template <typename Scalar> struct SO3 {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 4, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 4, 1>>;

  SO3Squared<Scalar> so3squared;

  void sample_uniform(ref_t x) const { so3squared.sample_uniform(x); }

  void set_bounds(cref_t lb_, cref_t ub_) {
    std::stringstream ss;
    ss << "so3 has no bounds " << __FILE__ << ":" << __LINE__;
    throw std::runtime_error(ss.str());
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {
    throw std::runtime_error("not implemented interpolate in so3");
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    return std::sqrt(so3squared.distance_to_rectangle(x, lb, ub));
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    return std::sqrt(so3squared.distance(x, y));
  };
};

// Rigid Body: Pose and Velocities
template <typename Scalar> struct R9SO3Squared {};

// SE3
template <typename Scalar> struct R3SO3Squared {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 7, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 7, 1>>;
  using cref3_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 3, 1>> &;

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  RnSquared<Scalar, 3> l2;
  SO3Squared<Scalar> so3;

  void set_bounds(cref3_t lb_, cref3_t ub_) { l2.set_bounds(lb_, ub_); }

  inline void sample_uniform(cref3_t lb, cref3_t ub, ref_t x) const {
    l2.sample_uniform(x.template head<3>());
    so3.sample_uniform(x.template tail<4>());
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    Scalar d1 = l2.distance_to_rectangle(
        x.template head<3>(), lb.template head<3>(), ub.template head<3>());

    Scalar d2 = so3.distance_to_rectangle(
        x.template tail<4>(), lb.template tail<4>(), ub.template tail<4>());

    return d1 + d2;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    Scalar d1 = l2.distance(x.template head<3>(), y.template head<3>());
    Scalar d2 = so3.distance(x.template tail<4>(), y.template tail<4>());
    return d1 + d2;
  };
};

template <typename Scalar> struct R3SO3 {

  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 7, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, 7, 1>>;

  using cref3_t = const Eigen::Ref<const Eigen::Matrix<Scalar, 3, 1>> &;
  using ref3_t = Eigen::Ref<Eigen::Matrix<Scalar, 3, 1>>;

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {

    std::stringstream error_msg;
    error_msg << "not implemented interpolate in " << __PRETTY_FUNCTION__;
    throw std::runtime_error(error_msg.str());
  }

  Rn<Scalar, 3> l2;
  SO3<Scalar> so3;

  void set_bounds(cref3_t lb_, cref3_t ub_) { l2.set_bounds(lb_, ub_); }

  void sample_uniform(ref_t x) const {
    l2.sample_uniform(x.template head<3>());
    so3.sample_uniform(x.template tail<4>());
  }

  inline Scalar distance_to_rectangle(cref_t &x, cref_t &lb, cref_t &ub) const {

    Scalar d1 = l2.distance_to_rectangle(
        x.template head<3>(), lb.template head<3>(), ub.template head<3>());

    Scalar d2 = so3.distance_to_rectangle(
        x.template tail<4>(), lb.template tail<4>(), ub.template tail<4>());

    return d1 + d2;
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    Scalar d1 = l2.distance(x.template head<3>(), y.template head<3>());
    Scalar d2 = so3.distance(x.template tail<4>(), y.template tail<4>());
    return d1 + d2;
  };
};

enum class DistanceType {
  RnL1,
  Rn,
  RnSquared,
  SO2,
  SO2Squared,
  SO3,
  SO3Squared
};

inline bool starts_with(const std::string &str, const std::string &prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

// get the substring after : as an integer number
inline int get_number(const std::string &str) {
  std::string delimiter = ":";
  size_t pos = 0;
  std::string token;
  // while ((
  pos = str.find(delimiter);
  if (pos == std::string::npos) {
    throw std::runtime_error("delimiter not found");
  }

  token = str.substr(pos + delimiter.length(), str.size());
  int out = std::stoi(token);
  std::cout << "out " << out << std::endl;
  return out;
}

template <typename Scalar> struct Combined {
  // TODO: test this!! How i am going to give this as input? -- it is not a
  // static function anymore...
  using cref_t = const Eigen::Ref<const Eigen::Matrix<Scalar, -1, 1>> &;
  using ref_t = Eigen::Ref<Eigen::Matrix<Scalar, -1, 1>>;

  using Space =
      std::variant<RnL1<Scalar>, Rn<Scalar>, RnSquared<Scalar>, SO2<Scalar>,
                   SO2Squared<Scalar>, SO3<Scalar>, SO3Squared<Scalar>>;
  std::vector<Space> spaces;
  std::vector<int> dims; // TODO: remove this and get auto from spaces

  Combined(const std::vector<Space> &spaces, const std::vector<int> &dims)
      : spaces(spaces), dims(dims) {
    assert(spaces.size() == dims.size());
  }

  Combined(const std::vector<std::string> &spaces_str) {
    // throw std::runtime_error("not implemented " + __PRETTY_FUNCTION__);

    for (size_t i = 0; i < spaces_str.size(); i++) {

      if (spaces_str.at(i) == "SO2") {
        spaces.push_back(SO2<Scalar>());
        dims.push_back(1);
      } else if (spaces_str.at(i) == "SO2Squared") {
        spaces.push_back(SO2Squared<Scalar>());
        dims.push_back(1);
      } else if (spaces_str.at(i) == "SO3") {
        spaces.push_back(SO3<Scalar>());
        dims.push_back(4);
      } else if (spaces_str.at(i) == "SO3Squared") {
        spaces.push_back(SO3Squared<Scalar>());
      } else if (starts_with(spaces_str.at(i), "RnL1")) {
        spaces.push_back(RnL1<Scalar>());
        int dim = get_number(spaces_str.at(i));
        dims.push_back(dim);
      } else if (starts_with(spaces_str.at(i), "Rn") &&
                 !starts_with(spaces_str.at(i), "RnSquared")) {
        spaces.push_back(Rn<Scalar>());
        int dim = get_number(spaces_str.at(i));
        dims.push_back(dim);
      } else if (starts_with(spaces_str.at(i), "RnSquared")) {
        spaces.push_back(RnSquared<Scalar>());
        int dim = get_number(spaces_str.at(i));
        dims.push_back(dim);
      } else {
        std::stringstream error_msg;
        error_msg << "Unknown space " << spaces_str.at(i) << " in "
                  << __PRETTY_FUNCTION__ << std::endl
                  << __FILE__ << ":" << __LINE__;
        throw std::runtime_error(error_msg.str());
      }
    }
    assert(spaces.size() == dims.size());
  }

  void choose_split_dimension(cref_t lb, cref_t ub, int &ii, Scalar &width) {
    choose_split_dimension_default(lb, ub, ii, width);
  }

  void set_bounds(const std::vector<Eigen::VectorXd> &lbs,
                  const std::vector<Eigen::VectorXd> &ubs) {

    assert(lbs.size() == ubs.size());

    for (size_t i = 0; i < lbs.size(); i++) {
      std::visit(
          [&](auto &obj) {
            if (lbs[i].size())
              obj.set_bounds(lbs[i], ubs[i]);
          },
          spaces[i]);
    }
  }

  void sample_uniform(ref_t x) const {

    assert(spaces.size() == dims.size());
    assert(spaces.size());
    int counter = 0;
    for (size_t i = 0; i < spaces.size(); i++) {
      std::visit(
          [&](const auto &obj) {
            obj.sample_uniform(x.segment(counter, dims[i]));
          },
          spaces[i]);
      counter += dims[i];
    }
  }

  void interpolate(cref_t from, cref_t to, Scalar t, ref_t out) const {

    assert(spaces.size() == dims.size());
    assert(spaces.size());
    Scalar d = 0;
    int counter = 0;
    for (size_t i = 0; i < spaces.size(); i++) {
      std::visit(
          [&](const auto &obj) {
            obj.interpolate(from.segment(counter, dims[i]),
                            to.segment(counter, dims[i]), t,
                            out.segment(counter, dims[i]));
          },
          spaces[i]);
      counter += dims[i];
    }
  }

  inline Scalar distance(cref_t x, cref_t y) const {

    assert(spaces.size() == dims.size());
    assert(spaces.size());
    Scalar d = 0;
    int counter = 0;
    for (size_t i = 0; i < spaces.size(); i++) {
      auto caller = [&](const auto &obj) {
        return obj.distance(x.segment(counter, dims[i]),
                            y.segment(counter, dims[i]));
      };

      d += std::visit(caller, spaces[i]);
      counter += dims[i];
    }
    return d;
  }

  inline Scalar distance_to_rectangle(cref_t x, cref_t lb, cref_t ub) const {

    assert(spaces.size() == dims.size());
    assert(spaces.size());

    Scalar d = 0;
    int counter = 0;
    for (size_t i = 0; i < spaces.size(); i++) {

      auto caller = [&](const auto &obj) {
        return obj.distance_to_rectangle(x.segment(counter, dims[i]),
                                         lb.segment(counter, dims[i]),
                                         ub.segment(counter, dims[i]));
      };

      d += std::visit(caller, spaces[i]);
      counter += dims[i];
    }

    return d;
  }
};
} // namespace dynotree
