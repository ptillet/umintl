/* ===========================
 *
 * Copyright (c) 2013 Philippe Tillet - National Chiao Tung University
 *
 * FMinCL - Unconstrained Function Minimization on OpenCL
 *
 * License : MIT X11 - See the LICENSE file in the root folder
 * ===========================*/


#ifndef FMINCL_CHECK_GRAD_HPP
#define FMINCL_CHECK_GRAD_HPP

#include "tools/shared_ptr.hpp"
#include <iostream>

namespace fmincl{

    template<class BackendType, class FUN>
    typename BackendType::ScalarType check_grad(FUN const & fun, typename BackendType::VectorType const & x0, std::size_t N, typename BackendType::ScalarType h){
        typedef typename BackendType::ScalarType ScalarType;
        typedef typename BackendType::VectorType VectorType;
        VectorType x = BackendType::create_vector(N);
        BackendType::copy(N,x0,x);
        VectorType fgrad = BackendType::create_vector(N);
        VectorType numgrad = BackendType::create_vector(N);
        ScalarType res = 0;
        ScalarType vl, vr;
        fun(x,&vl,&fgrad);
        for(unsigned int i=0 ; i < N ; ++i){
            ScalarType vx = x[i];
            x[i] = vx-h; fun(x,&vl,NULL);
            x[i] = vx+h; fun(x,&vr,NULL);
            numgrad[i] = (vr-vl)/(2*h);
            x[i]=vx;
        }
        for(unsigned int i=0 ; i < N ; ++i){
            ScalarType denom = std::max(std::abs(numgrad[i]),std::abs(fgrad[i]));
            ScalarType diff = std::abs(numgrad[i]-fgrad[i]);
            if(denom>1)
                diff/=denom;
            res = std::max(res,diff);
        }
        return res;
    }


}
#endif