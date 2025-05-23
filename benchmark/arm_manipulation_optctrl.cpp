///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2019-2023, LAAS-CNRS, University of Edinburgh
//                          Heriot-Watt University
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "crocoddyl/core/solvers/fddp.hpp"
#include "crocoddyl/core/utils/callbacks.hpp"
#include "crocoddyl/core/utils/timer.hpp"
#include "factory/arm.hpp"

int main(int argc, char* argv[]) {
  bool CALLBACKS = false;
  unsigned int N = 100;  // number of nodes
  unsigned int T = 5e3;  // number of trials
  unsigned int MAXITER = 1;
  if (argc > 1) {
    T = atoi(argv[1]);
  }

  // Building the running and terminal models
  std::shared_ptr<crocoddyl::ActionModelAbstract> runningModel, terminalModel;
  crocoddyl::benchmark::build_arm_action_models(runningModel, terminalModel);

  // Get the initial state
  std::shared_ptr<crocoddyl::StateMultibody> state =
      std::static_pointer_cast<crocoddyl::StateMultibody>(
          runningModel->get_state());
  std::cout << "NQ: " << state->get_nq() << std::endl;
  std::cout << "Number of nodes: " << N << std::endl;
  Eigen::VectorXd q0 =
      state->get_pinocchio()->referenceConfigurations["arm_up"];
  Eigen::VectorXd x0(state->get_nx());
  x0 << q0, Eigen::VectorXd::Random(state->get_nv());

  // For this optimal control problem, we define 100 knots (or running action
  // models) plus a terminal knot
  std::vector<std::shared_ptr<crocoddyl::ActionModelAbstract> > runningModels(
      N, runningModel);
  std::shared_ptr<crocoddyl::ShootingProblem> problem =
      std::make_shared<crocoddyl::ShootingProblem>(x0, runningModels,
                                                   terminalModel);
  std::vector<Eigen::VectorXd> xs(N + 1, x0);
  std::vector<Eigen::VectorXd> us(
      N, Eigen::VectorXd::Zero(runningModel->get_nu()));
  for (unsigned int i = 0; i < N; ++i) {
    const std::shared_ptr<crocoddyl::ActionModelAbstract>& model =
        problem->get_runningModels()[i];
    const std::shared_ptr<crocoddyl::ActionDataAbstract>& data =
        problem->get_runningDatas()[i];
    model->quasiStatic(data, us[i], x0);
  }

  // Formulating the optimal control problem
  crocoddyl::SolverFDDP solver(problem);
  if (CALLBACKS) {
    std::vector<std::shared_ptr<crocoddyl::CallbackAbstract> > cbs;
    cbs.push_back(std::make_shared<crocoddyl::CallbackVerbose>());
    solver.setCallbacks(cbs);
  }

  // Solving the optimal control problem
  Eigen::ArrayXd duration(T);
  for (unsigned int i = 0; i < T; ++i) {
    crocoddyl::Timer timer;
    solver.solve(xs, us, MAXITER, false, 0.1);
    duration[i] = timer.get_duration();
  }

  double avrg_duration = duration.sum() / T;
  double min_duration = duration.minCoeff();
  double max_duration = duration.maxCoeff();
  std::cout << "  FDDP.solve [ms]: " << avrg_duration << " (" << min_duration
            << "-" << max_duration << ")" << std::endl;

  // Running calc
  for (unsigned int i = 0; i < T; ++i) {
    crocoddyl::Timer timer;
    problem->calc(xs, us);
    duration[i] = timer.get_duration();
  }

  avrg_duration = duration.sum() / T;
  min_duration = duration.minCoeff();
  max_duration = duration.maxCoeff();
  std::cout << "  ShootingProblem.calc [ms]: " << avrg_duration << " ("
            << min_duration << "-" << max_duration << ")" << std::endl;

  // Running calcDiff
  for (unsigned int i = 0; i < T; ++i) {
    crocoddyl::Timer timer;
    problem->calcDiff(xs, us);
    duration[i] = timer.get_duration();
  }

  avrg_duration = duration.sum() / T;
  min_duration = duration.minCoeff();
  max_duration = duration.maxCoeff();
  std::cout << "  ShootingProblem.calcDiff [ms]: " << avrg_duration << " ("
            << min_duration << "-" << max_duration << ")" << std::endl;
}
