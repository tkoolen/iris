#ifndef _IRIS_H
#define _IRIS_H

#include <Eigen/Core>
#include <vector>

const double ELLIPSOID_C_EPSILON = 1e-4;

struct IRISOptions {
  bool require_containment = false; // if true: if the IRIS polytope no longer contains the seed point
  bool error_on_infeas_start = false;
  double termination_threshold = 2e-2;
};

class Polytope {
public:
  Polytope(int dim=0);
  Polytope(Eigen::MatrixXd A, Eigen::VectorXd b);
  void setA(const Eigen::MatrixXd &A);
  const Eigen::MatrixXd& getA() const;
  void setB(const Eigen::VectorXd &b);
  const Eigen::VectorXd& getB() const;
  int getDimension() const;
  int getNumberOfConstraints() const;
  void appendConstraints(const Polytope &other);

private:
  Eigen::MatrixXd A_;
  Eigen::VectorXd b_;
};



class Ellipsoid {
public:
  Ellipsoid(int dim=0);
  Ellipsoid(Eigen::MatrixXd C, Eigen::VectorXd d);
  const Eigen::MatrixXd& getC() const;
  const Eigen::VectorXd& getD() const;
  void setC(const Eigen::MatrixXd &C_);
  void setCEntry(Eigen::DenseIndex row, Eigen::DenseIndex col, double value);
  void setD(const Eigen::VectorXd &d_);
  void setDEntry(Eigen::DenseIndex idx, double value);
  int getDimension() const;
  static Ellipsoid fromNSphere(Eigen::VectorXd &center, double radius=ELLIPSOID_C_EPSILON);

private:
  Eigen::MatrixXd C_;
  Eigen::VectorXd d_;
};

class IRISRegion {
public:
  Polytope polytope;
  Ellipsoid ellipsoid;

  IRISRegion(int dim=0):
    polytope(dim),
    ellipsoid(dim)
    {}
};

struct IRISDebugData {
  std::vector<Ellipsoid> ellipsoid_history;
  std::vector<Polytope> polytope_history;
  std::vector<Eigen::MatrixXd> obstacles;
  Eigen::VectorXd ellipsoid_times;
  Eigen::VectorXd polytope_times;
  double total_time;
  int iters;
};

class IRISProblem {
private:
  std::vector<Eigen::MatrixXd> obstacle_pts; // each obstacle is a matrix of size (_dim, pts_per_obstacle)
  Polytope bounds;
  int dim;
  Ellipsoid seed;

public:
  IRISProblem(int dim):
    bounds(dim),
    dim(dim),
    seed(dim) {}

  void setSeedPoint(Eigen::VectorXd point);
  void setSeedEllipsoid(Ellipsoid ellipsoid);
  int getDimension() const;
  Ellipsoid getSeed() const;
  void setBounds(Polytope new_bounds);
  void addObstacle(Eigen::MatrixXd new_obstacle_vertices);
  std::vector<Eigen::MatrixXd> getObstacles() const;
  Polytope getBounds() const;
};


void inflate_region(const IRISProblem &problem, const IRISOptions &options, IRISRegion *result, IRISDebugData *debug=NULL);

void separating_hyperplanes(const std::vector<Eigen::MatrixXd> obstacle_pts, const Ellipsoid &ellipsoid, Polytope &polytope, bool &infeasible_start);


#endif