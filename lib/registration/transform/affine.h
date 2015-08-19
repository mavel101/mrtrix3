/*
   Copyright 2012 Brain Research Institute, Melbourne, Australia

   Written by David Raffelt, 24/02/2012

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef __registration_transform_affine_h__
#define __registration_transform_affine_h__

#include "registration/transform/base.h"
#include "math/gradient_descent.h"
#include "types.h"

using namespace MR::Math;

namespace MR
{
  namespace Registration
  {
    namespace Transform
    {

      /** \addtogroup Transforms
      @{ */

      /*! A 3D affine transformation class for registration.
       *
       * This class supports the ability to define the centre of rotation.
       * This should be set prior to commencing registration based on the centre of the target image.
       * The translation also should be initialised as moving image centre minus the target image centre.
       *
       */
      class Affine : public Base  {
        public:

          typedef typename Base::ParameterType ParameterType;
          typedef Math::LinearUpdate UpdateType;

          Affine () : Base (12) {
            for (size_t i = 0; i < 9; ++i)
              this->optimiser_weights[i] = 0.003;
            for (size_t i = 9; i < 12; ++i)
              this->optimiser_weights[i] = 1.0;
            DEBUG("non-symmetric affine transformation");
          }


          void get_jacobian_wrt_params (const Eigen::Vector3& p, Eigen::MatrixXd& jacobian) const {
            jacobian.resize (3,12);
            jacobian.setZero();
            Eigen::Vector3 v;
            v[0] = p[0] - this->centre[0];
            v[1] = p[1] - this->centre[1];
            v[2] = p[2] - this->centre[2];
            size_t blockOffset = 0;
            for (size_t block = 0; block < 3; ++block) {
              for (size_t dim = 0; dim < 3; ++dim)
                jacobian (block, blockOffset + dim) = v[dim];
              blockOffset += 3;
            }
            for (size_t dim = 0; dim < 3; ++dim) {
              jacobian (dim, blockOffset + dim) = 1.0;
            }
          }


          void set_parameter_vector (const Eigen::Matrix<default_type, Eigen::Dynamic, 1>& param_vector) {
            size_t index = 0;
            for (size_t row = 0; row < 3; ++row) {
              for (size_t col = 0; col < 3; ++col)
                this->matrix (row, col) = param_vector[index++];
            }
            for (size_t dim = 0; dim < 3; ++dim)
              this->translation [dim] = param_vector[index++];
            this->compute_offset();
          }

          void get_parameter_vector (Eigen::Matrix<default_type, Eigen::Dynamic, 1>& param_vector) const {
            param_vector.resize (12);
            size_t index = 0;
            for (size_t row = 0; row < 3; ++row) {
              for (size_t col = 0; col < 3; ++col)
                param_vector[index++] = this->matrix (row, col);
            }
            for (size_t dim = 0; dim < 3; ++dim)
              param_vector[index++] = this->translation[dim];
          }

          UpdateType* get_gradient_descent_updator () {
            return &gradient_descent_updator;
          }



        protected:
          UpdateType gradient_descent_updator;
      };
      //! @}
    }
  }
}

#endif
