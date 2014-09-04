#ifndef NSEGO_HPP_
#define NSEGO_HPP_

#include <algorithm>
#include "bo_multi.hpp"


namespace limbo {


  template <
    class Params
    , class A2 = boost::parameter::void_
    , class A3 = boost::parameter::void_
    , class A4 = boost::parameter::void_
    , class A5 = boost::parameter::void_
    , class A6 = boost::parameter::void_
    , class A7 = boost::parameter::void_
    >
  class NsEgo : public BoMulti<Params, A2, A3, A4, A5, A6, A7> {
   public:
    typedef std::tuple<Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd> pareto_point_t;

    template<typename EvalFunction>
    void optimize(const EvalFunction& feval, bool reset = true) {
      this->_init(feval, reset);

      while (this->_samples.size() == 0 || this->_pursue()) {
	std::cout<<"updating pareto model..."; std::cout.flush();
        this->template update_pareto_model<EvalFunction::dim>();
	std::cout<<"ok"<<std::endl;
        auto pareto = this->pareto_model();
        // this is hack to test wether we need a bound
        pareto.erase(std::remove_if(pareto.begin(), pareto.end(),
        [](const pareto_point_t& x) {
          for (int i = 0; i < std::get<1>(x).size(); ++i)
            if (std::get<1>(x)(i) > 1)
              return true;
          return false;
        }), pareto.end());
        auto best = std::max_element(pareto.begin(), pareto.end(),
        [](const pareto_point_t& x1, const pareto_point_t& x2) {
          return std::get<2>(x1).sum() < std::get<2>(x2).sum();
        });
        Eigen::VectorXd best_v = std::get<0>(*best);
        //  best_v = std::get<0>(pareto[(int)misc::rand<double>(0, pareto.size())]);
        this->add_new_sample(best_v, feval(best_v));
        this->_iteration++;
        std::cout << this->_iteration << " | " << best_v.transpose()
                  << "-> " << this->_observations.back().transpose()
                  << " (expected:" << this->_models[0].mu(best_v) << " "
                  << this->_models[1].mu(best_v) << ")"
                  << " sigma:" << this->_models[0].sigma(best_v)
                  << " " << this->_models[1].sigma(best_v)
                  << std::endl;
        _update_stats();
      }

    }

   protected:
    void _update_stats() {
      std::cout << "stats" << std::endl;
      boost::fusion::for_each(this->_stat, RefreshStat_f<NsEgo>(*this));
    }

  };

}

#endif
