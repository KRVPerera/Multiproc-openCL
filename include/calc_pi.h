//
// Created by ruksh on 16/03/2024.
//

#ifndef MULTIPROCOPENCL_CALC_PI_H
#define MULTIPROCOPENCL_CALC_PI_H

double calc_pi(int num_steps);
double calc_pi_mt(int num_steps);
double calc_pi_mt_false_sharing(int num_steps);

#endif //MULTIPROCOPENCL_CALC_PI_H
