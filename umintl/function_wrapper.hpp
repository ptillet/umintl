/* ===========================
  Copyright (c) 2013 Philippe Tillet
  UMinTL - Unconstrained Minimization Template Library

  License : MIT X11 - See the LICENSE file in the root folder
 * ===========================*/

#ifndef UMINTL_FUNCTION_WRAPPER_HPP
#define UMINTL_FUNCTION_WRAPPER_HPP

#include "tools/shared_ptr.hpp"
#include "tools/is_call_possible.hpp"
#include "tools/exception.hpp"
#include "umintl/forwards.h"
#include <iostream>



namespace umintl{

    namespace detail{
        
        template<int N>
        struct int2type{ };

        template<class BackendType>
        class function_wrapper{
            typedef typename BackendType::ScalarType ScalarType;
            typedef typename BackendType::VectorType VectorType;
        public:
            function_wrapper(){ }
            virtual unsigned int n_value_computations() const = 0;
            virtual unsigned int n_gradient_computations() const  = 0;
            virtual unsigned int n_hessian_vector_product_computations() const  = 0;
            virtual void operator()(VectorType const & x, ScalarType & value, value_tag const &) = 0;
            virtual void operator()(VectorType const & x, VectorType & gradient, gradient_tag const &) = 0;
            virtual void operator()(VectorType const & x, ScalarType & value, VectorType & grad, value_gradient_tag const &) = 0;
            virtual void operator()(VectorType const & x, VectorType const & v, VectorType & Hv, hessian_vector_product_tag const &) = 0;
            virtual ~function_wrapper(){ }
        };


        template<class BackendType, class Fun>
        class function_wrapper_impl : public function_wrapper<BackendType>{
        private:
            typedef typename BackendType::VectorType VectorType;
            typedef typename BackendType::ScalarType ScalarType;
        private:

            //Compute function's value alone
            void operator()(VectorType const &, ScalarType &, value_tag const &, int2type<false>){
                throw exceptions::incompatible_parameters(
                            "\n"
                            "No function supplied to compute the function's value alone!"
                            "Please provide an overload of :\n"
                            "void operator()(VectorType const &, ScalarType &, umintl::value_tag)\n."
                            "Alternatively, if you are computing the function's value along with the gradient,"
                            "check that minimizer.tweaks.function_gradient_evaluation is set to:"
                            "PACKED_FUNCTION_GRADIENT_EVALUATION."
                            );
            }
            void operator()(VectorType const & x, ScalarType & value, value_tag const & tag, int2type<true>){
                fun_(x,value,tag);
                n_value_computations_++;
            }

            //Compute function's gradient alone
            void operator()(VectorType const &, VectorType &, gradient_tag const &, int2type<false>){
                throw exceptions::incompatible_parameters(
                            "\n"
                            "No function supplied to compute the function's gradient alone!"
                            "Please provide an overload of :\n"
                            "void operator()(VectorType const & X, VectorType & gradient, umintl::gradient_tag)\n."
                            "Alternatively, if you are computing the function's gradient along with the value,"
                            "check that minimizer.tweaks.function_gradient_evaluation is set to:"
                            "PACKED_FUNCTION_GRADIENT_EVALUATION."
                            );
            }
            void operator()(VectorType const & x, VectorType & gradient, gradient_tag const & tag, int2type<true>){
                fun_(x,gradient,tag);
                n_gradient_computations_++;
            }

            //Compute both function's value and gradient
            void operator()(VectorType const &, ScalarType&, VectorType &, value_gradient_tag const &, int2type<false>){
                throw exceptions::incompatible_parameters(
                            "\n"
                            "No function supplied to compute both the function's value and gradient!"
                            "Please provide an overload of :\n"
                            "void operator()(VectorType const & X, ScalarType& value, VectorType & gradient, umintl::value_gradient_tag)\n."
                            "Alternatively, if you are computing the function's gradient and value separately,"
                            "check that minimizer.tweaks.function_gradient_evaluation is set to:"
                            "SEPARATE_FUNCTION_GRADIENT_EVALUATION."
                            );
            }
            void operator()(VectorType const & x, ScalarType& value, VectorType & gradient, value_gradient_tag const & tag, int2type<true>){
                fun_(x,value,gradient,tag);
                n_value_computations_++;
                n_gradient_computations_++;
            }

            //Compute hessian-vector product
            void operator()(VectorType const &, VectorType const &, VectorType&, hessian_vector_product_tag const &, int2type<false>){
                throw exceptions::incompatible_parameters(
                            "\n"
                            "No function supplied to compute the hessian-vector product!"
                            "Please provide an overload of :\n"
                            "void operator()(VectorType const & X, VectorType& v, VectorType & Hv, umintl::hessian_vector_product_tag)\n."
                            "If you wish to use right/centered-differentiation of the function's gradient, please set the appropriate options."
                            );
            }
            void operator()(VectorType const & x, VectorType const & v, VectorType& Hv, hessian_vector_product_tag const & tag, int2type<true>){
                fun_(x,v,Hv,tag);
                n_hessian_vector_product_computations_++;
            }

        public:
            function_wrapper_impl(Fun & fun) : fun_(fun){
                n_value_computations_ = 0;
                n_gradient_computations_ = 0;
                n_hessian_vector_product_computations_ = 0;
            }

            unsigned int n_value_computations() const{
                return n_value_computations_;
            }

            unsigned int n_gradient_computations() const {
                return n_gradient_computations_;
            }

            unsigned int n_hessian_vector_product_computations() const {
                return n_hessian_vector_product_computations_;
            }

            void operator()(VectorType const & x, ScalarType & value, value_tag const & tag){
                (*this)(x,value,tag,int2type<is_call_possible<Fun,void(VectorType const &, ScalarType&, value_tag)>::value>());
            }

            void operator()(VectorType const & x, VectorType & gradient, gradient_tag const & tag){
                (*this)(x,gradient,tag,int2type<is_call_possible<Fun,void(VectorType const &, VectorType&, gradient_tag)>::value>());
            }

            void operator()(VectorType const & x, ScalarType & value, VectorType & gradient, value_gradient_tag const & tag){
                (*this)(x,value,gradient,tag,int2type<is_call_possible<Fun,void(VectorType const &, ScalarType&, VectorType&, value_gradient_tag)>::value>());
            }

            void operator()(VectorType const & x, VectorType const & v, VectorType & Hv, hessian_vector_product_tag const & tag){
                (*this)(x,v,Hv,tag,int2type<is_call_possible<Fun,void(VectorType const &, VectorType&, VectorType&, hessian_vector_product_tag)>::value>());
            }
        private:
            Fun & fun_;

            unsigned int n_value_computations_;
            unsigned int n_gradient_computations_;
            unsigned int n_hessian_vector_product_computations_;
        };

    }

}
#endif
