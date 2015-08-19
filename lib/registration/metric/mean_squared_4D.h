/*
   Copyright 2012 Brain Research Institute, Melbourne, Australia

   Written by David Raffelt, 10/07/13

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

#ifndef __registration_metric_mean_squared_4D_h__
#define __registration_metric_mean_squared_4D_h__

#include "registration/metric/base.h"

namespace MR
{
  namespace Registration
  {
    namespace Metric
    {
      class MeanSquared4D : public Base {
        public:

          template <class Params>
            default_type operator() (Params& params,
                                     const Eigen::Vector3 target_point,
                                     const Eigen::Vector3 moving_point,
                                     Eigen::Matrix<default_type, Eigen::Dynamic, 1>& gradient) {

              assert (params.template_image.ndim() == 4);

              params.template_image.index(3) = 0;
              if (isnan (default_type (params.template_image.value())))
                return 0.0;

              params.transformation.get_jacobian_wrt_params (target_point, this->jacobian);

              default_type total_diff = 0.0;

              // TODO get gradient from image on the fly
              gradient_interp->scanner (moving_point);

              // TODO interpolate 4th dimension in one go
              for (params.template_image.index(3) = 0; params.template_image.index(3) < params.template_image.size(3); ++params.template_image.index(3)) {
                (*gradient_interp).index(4) = params.template_image.index(3);
                (*params.moving_image_interp).index(3) = params.template_image.index(3);

                moving_grad = gradient_interp->row(3).template cast<default_type>();

                default_type diff = params.moving_image_interp->value() - params.template_image.value();
                total_diff += (diff * diff);

                for (size_t par = 0; par < gradient.size(); par++) {
                  default_type sum = 0.0;
                  for ( size_t dim = 0; dim < 3; dim++)
                    sum += 2.0 * diff * this->jacobian (dim, par) * moving_grad[dim];
                  gradient[par] += sum;
                }

              }
              return total_diff;
          }

      };
    }
  }
}
#endif
