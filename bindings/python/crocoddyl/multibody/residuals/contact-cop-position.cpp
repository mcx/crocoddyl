///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2020-2025, University of Duisburg-Essen, University of
// Edinburgh,
//                          Heriot-Watt University
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "crocoddyl/multibody/residuals/contact-cop-position.hpp"

#include "python/crocoddyl/multibody/multibody.hpp"
#include "python/crocoddyl/utils/copyable.hpp"
#include "python/crocoddyl/utils/deprecate.hpp"

namespace crocoddyl {
namespace python {

void exposeResidualContactCoPPosition() {
  bp::register_ptr_to_python<
      std::shared_ptr<ResidualModelContactCoPPosition> >();

  bp::class_<ResidualModelContactCoPPosition,
             bp::bases<ResidualModelAbstract> >(
      "ResidualModelContactCoPPosition",
      bp::init<std::shared_ptr<StateMultibody>, pinocchio::FrameIndex,
               CoPSupport, std::size_t, bp::optional<bool> >(
          bp::args("self", "state", "id", "cref", "nu", "fwddyn"),
          "Initialize the contact CoP position residual model.\n\n"
          ":param state: state of the multibody system\n"
          ":param id: reference frame id\n"
          ":param cref: support region of the CoP\n"
          ":param nu: dimension of control vector\n"
          ":param fwddyn: indicate if we have a forward dynamics problem "
          "(True) or inverse dynamics problem (False) "
          "(default True)"))
      .def(bp::init<std::shared_ptr<StateMultibody>, pinocchio::FrameIndex,
                    CoPSupport>(
          bp::args("self", "state", "id", "cref"),
          "Initialize the contact CoP position residual model.\n\n"
          "The default nu is obtained from state.nv. Note that this "
          "constructor can be used for forward-dynamics\n"
          "cases only.\n"
          ":param state: state of the multibody system\n"
          ":param id: reference frame id\n"
          ":param cref: support region of the CoP"))
      .def<void (ResidualModelContactCoPPosition::*)(
          const std::shared_ptr<ResidualDataAbstract>&,
          const Eigen::Ref<const Eigen::VectorXd>&,
          const Eigen::Ref<const Eigen::VectorXd>&)>(
          "calc", &ResidualModelContactCoPPosition::calc,
          bp::args("self", "data", "x", "u"),
          "Compute the contact CoP position residual.\n\n"
          ":param data: residual data\n"
          ":param x: state point (dim. state.nx)\n"
          ":param u: control input (dim. nu)")
      .def<void (ResidualModelContactCoPPosition::*)(
          const std::shared_ptr<ResidualDataAbstract>&,
          const Eigen::Ref<const Eigen::VectorXd>&,
          const Eigen::Ref<const Eigen::VectorXd>&)>(
          "calcDiff", &ResidualModelContactCoPPosition::calcDiff,
          bp::args("self", "data", "x", "u"),
          "Compute the derivatives of the contact CoP position residual.\n\n"
          "It assumes that calc has been run first.\n"
          ":param data: action data\n"
          ":param x: state point (dim. state.nx)\n"
          ":param u: control input (dim. nu)\n")
      .def("createData", &ResidualModelContactCoPPosition::createData,
           bp::with_custodian_and_ward_postcall<0, 2>(),
           bp::args("self", "data"),
           "Create the contact CoP position residual data.\n\n"
           "Each residual model has its own data that needs to be allocated. "
           "This function\n"
           "returns the allocated data for the CoP position residual.\n"
           ":param data: shared data\n"
           ":return residual data.")
      .add_property(
          "id", bp::make_function(&ResidualModelContactCoPPosition::get_id),
          bp::make_function(
              &ResidualModelContactCoPPosition::set_id,
              deprecated<>(
                  "Deprecated. Do not use set_id, instead create a new model")),
          "reference frame id")
      .add_property("reference",
                    bp::make_function(
                        &ResidualModelContactCoPPosition::get_reference,
                        bp::return_value_policy<bp::copy_const_reference>()),
                    &ResidualModelContactCoPPosition::set_reference,
                    "reference support region of the CoP")
      .def(CopyableVisitor<ResidualModelContactCoPPosition>());

  bp::register_ptr_to_python<
      std::shared_ptr<ResidualDataContactCoPPosition> >();

  bp::class_<ResidualDataContactCoPPosition, bp::bases<ResidualDataAbstract> >(
      "ResidualDataContactCoPPosition",
      "Data for contact CoP position residual.\n\n",
      bp::init<ResidualModelContactCoPPosition*, DataCollectorAbstract*>(
          bp::args("self", "model", "data"),
          "Create contact CoP position residual data.\n\n"
          ":param model: contact CoP position residual model\n"
          ":param data: shared data")[bp::with_custodian_and_ward<
          1, 2, bp::with_custodian_and_ward<1, 3> >()])
      .add_property("pinocchio",
                    bp::make_getter(&ResidualDataContactCoPPosition::pinocchio,
                                    bp::return_internal_reference<>()),
                    "pinocchio data")
      .add_property(
          "contact",
          bp::make_getter(&ResidualDataContactCoPPosition::contact,
                          bp::return_value_policy<bp::return_by_value>()),
          bp::make_setter(&ResidualDataContactCoPPosition::contact),
          "contact data associated with the current residual")
      .def(CopyableVisitor<ResidualDataContactCoPPosition>());
}

}  // namespace python
}  // namespace crocoddyl
