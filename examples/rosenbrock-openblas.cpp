/* ===========================
 *
 * Copyright (c) 2013 Philippe Tillet - National Chiao Tung University
 *
 * FMinCL - Unconstrained Function Minimization on OpenCL
 *
 * License : MIT X11 - See the LICENSE file in the root folder
 * ===========================*/

#include <cstdlib>

#include "umintl/backends/cblas.hpp"
#include "umintl/minimize.hpp"

typedef double ScalarType;
typedef umintl::backend::cblas_types<ScalarType> BackendType;

class custom_stop : public umintl::stopping_criterion<BackendType>{
public:
    bool operator()(umintl::optimization_context<BackendType> & context){
        ScalarType * X = context.x(); //Obtain current iterate
        return std::abs(X[0] - 1)<1e-6;
    }
};

class rosenbrock{
public:
    rosenbrock(std::size_t N) : N_(N){ }
    void operator()(ScalarType* const & x, ScalarType* val, ScalarType** grad) const {
        ScalarType res=0;
        for(unsigned int i=0 ; i<N_-1;++i){
            res = res + 100*(pow(x[i+1] - x[i]*x[i],2)) + pow(1 - x[i],2);
        }
        if(val){
            *val=res;
        }
        if(grad){
            (*grad)[0] = -400*x[0]*(x[1] - pow(x[0],2)) - 2*(1 - x[0]);
            for(unsigned int i=1 ; i<N_-1 ; ++i){
                ScalarType xi = x[i];
                ScalarType xim1 = x[i-1];
                ScalarType xip1 = x[i+1];
                (*grad)[i] = 200*(xi - xim1*xim1) - 400*xi*(xip1-xi*xi) - 2*(1 - xi);
            }
            (*grad)[N_-1] = 200*(x[N_-1]-x[N_-2]*x[N_-2]);
        }
    }
private:
    std::size_t N_;

};

void print_vector(ScalarType * x, std::size_t N){
    std::cout << "["; for(std::size_t i = 0 ; i < N ; ++i) std::cout << x[i] << ((i==N-1)?']':',') << std::flush;
}

void print_solution(umintl::optimization_result const & result, ScalarType * S, std::size_t D)
{
    std::cout << "Optimization complete ! " << std::endl;
    std::cout << "Solution : " << std::endl;
    print_vector(S,D);
    std::cout << std::endl;
    std::cout << "Solution's value : " << result.f << std::endl;
    std::cout << "Found in " << result.iteration << " iterations / " << result.n_functions_eval << " functions eval" << " / " << result.n_gradient_eval << " gradient eval " << std::endl;
    std::cout << std::endl;
}

int main(){
    srand(0);

    unsigned int D = 100;

    std::cout << "====================" << std::endl;
    std::cout << "Minimization of the generalized Rosenbrock function" << std::endl;
    std::cout << "Dimension : " << D << std::endl;
    std::cout << "====================" << std::endl;

    ScalarType* X0 = new ScalarType[D];
    ScalarType* S = new ScalarType[D];

    for(std::size_t i = 0 ; i < D ; ++i) X0[i] = 0;

    std::cout << "Starting at : " << std::endl;
    print_vector(X0,D);
    std::cout << std::endl;

    umintl::minimizer<BackendType> minimizer;
    rosenbrock objective(D);
    minimizer.max_iter = 100000;
    minimizer.verbosity_level=0;
    umintl::optimization_result result;

    std::cout << std::endl;

//    std::cout << "--------------------" << std::endl;
//    std::cout << "Using steepest descent" << std::endl;
//    std::cout << "--------------------" << std::endl;
//    minimizer.direction = new umintl::steepest_descent<BackendType>();
//    result = minimizer(S,objective,X0,D);
//    print_solution(result,S,D);

//    std::cout << "--------------------" << std::endl;
//    std::cout << "Using CG [ beta = polak-ribiere , no restart ]" << std::endl;
//    std::cout << "--------------------" << std::endl;
//    minimizer.direction = new umintl::conjugate_gradient<BackendType>(new umintl::polak_ribiere<BackendType>(), new umintl::no_restart<BackendType>());
//    result = minimizer(S,objective,X0,D);
//    print_solution(result,S,D);

//    std::cout << "--------------------" << std::endl;
//    std::cout << "Using BFGS" << std::endl;
//    std::cout << "--------------------" << std::endl;
//    minimizer.direction = new umintl::quasi_newton<BackendType>(new umintl::bfgs<BackendType>());
//    result = minimizer(S,objective,X0,D);
//    print_solution(result,S,D);

//    std::cout << "--------------------" << std::endl;
//    std::cout << "Using L-BFGS [ memory = 5 ]" << std::endl;
//    std::cout << "--------------------" << std::endl;
//    minimizer.direction = new umintl::quasi_newton<BackendType>(new umintl::lbfgs<BackendType>(8));
//    result = minimizer(S,objective,X0,D);
//    print_solution(result,S,D);

    std::cout << "--------------------" << std::endl;
    std::cout << "Using Truncated Newton" << std::endl;
    std::cout << "--------------------" << std::endl;
    umintl::hessian_free::options<BackendType> opts(100);
    minimizer.verbosity_level=2;
    minimizer.max_iter=1000;
    minimizer.direction = new umintl::truncated_newton<BackendType>(opts);
    //minimizer.direction = new umintl::quasi_newton<BackendType>(new umintl::lbfgs<BackendType>(8));
    result = minimizer(S,objective,X0,D);
    //print_solution(result,S,D);

//    std::cout << "====================" << std::endl;
//    std::cout << "====================" << std::endl;
//    std::cout << std::endl;
//    std::cout << "--------------------" << std::endl;
//    std::cout << "L-BFGS" << std::endl;
//    std::cout << "Custom Stopping criterion:" << std::endl;
//    std::cout << "Stops when the first dimension is close enough to optimal" << std::endl;
//    std::cout << "--------------------" << std::endl;
//    minimizer.stopping_criterion = new custom_stop();
//    result = minimizer(S,objective,X0,D);
//    print_solution(result,S,D);



    return EXIT_SUCCESS;
}
