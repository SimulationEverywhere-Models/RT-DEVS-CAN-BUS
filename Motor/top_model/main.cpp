
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium/modeling/coupling.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include <NDTime.hpp>
#include <cadmium/io/iestream.hpp>

#include "../atomics/CAN_controller.hpp"
#include "../atomics/Motor.hpp"

#ifdef RT_ARM_MBED
  #include "../mbed.h"
#else
  // When simulating the model it will use these files as IO in place of the pins specified.
  const char* PD_0;
  const char* PD_1;

#endif

using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;



int main(int argc, char ** argv) {

  #ifdef RT_ARM_MBED
      //Logging is done over cout in RT_ARM_MBED
      struct oss_sink_provider{
        static std::ostream& sink(){
          return cout;
        }
      };
  #else
    // all simulation timing and I/O streams are ommited when running embedded
    auto start = hclock::now(); //to measure simulation execution time

    static std::ofstream out_data("output/motor_out.txt");
    struct oss_sink_provider{
      static std::ostream& sink(){
        return out_data;
      }
    };
  #endif

  /*************** Loggers *******************/
  using info=cadmium::logger::logger<cadmium::logger::logger_info, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using debug=cadmium::logger::logger<cadmium::logger::logger_debug, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using state=cadmium::logger::logger<cadmium::logger::logger_state, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_messages=cadmium::logger::logger<cadmium::logger::logger_messages, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using routing=cadmium::logger::logger<cadmium::logger::logger_message_routing, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using global_time=cadmium::logger::logger<cadmium::logger::logger_global_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using local_time=cadmium::logger::logger<cadmium::logger::logger_local_time, cadmium::dynamic::logger::formatter<TIME>, oss_sink_provider>;
  using log_all=cadmium::logger::multilogger<info, debug, state, log_messages, routing, global_time, local_time>;

  using logger_top=cadmium::logger::multilogger<log_messages, global_time>;


  /*******************************************/

  using AtomicModelPtr=std::shared_ptr<cadmium::dynamic::modeling::model>;
  using CoupledModelPtr=std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>;


  /********************************************/
  /********** AnalogInput1 *******************/
  /********************************************/
 
AtomicModelPtr CAN_1 = cadmium::dynamic::translate::make_dynamic_atomic_model< CAN_controller, TIME>("CAN_1", PD_0, PD_1, 2);
AtomicModelPtr Motor_1 = cadmium::dynamic::translate::make_dynamic_atomic_model< Motor, TIME>("Motor_1");

 
  /************************/
  /*******TOP MODEL********/
  /************************/

  cadmium::dynamic::modeling::Ports iports_TOP = {};
  cadmium::dynamic::modeling::Ports oports_TOP = {};
  cadmium::dynamic::modeling::EICs eics_TOP = {};
  cadmium::dynamic::modeling::EOCs eocs_TOP = {
  };
  cadmium::dynamic::modeling::Models submodels_TOP =  {CAN_1, Motor_1};
  cadmium::dynamic::modeling::ICs ics_TOP = { 
   cadmium::dynamic::translate::make_IC<canController_defs::DataOut, Motor_defs::input>("CAN_1","Motor_1"),
   cadmium::dynamic::translate::make_IC<Motor_defs::output, canController_defs::DataIn>("Motor_1","CAN_1"),

  };
  CoupledModelPtr TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
    "TOP",
    submodels_TOP,
    iports_TOP,
    oports_TOP,
    eics_TOP,
    eocs_TOP,
    ics_TOP
  );

  ///****************////
  #ifdef RT_ARM_MBED

    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    //cadmium::dynamic::engine::runner<NDTime, cadmium::logger::not_logger> r(TOP, {0});
    r.run_until(NDTime("00:05:00:000"));
  #else
    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:10:00:000"));
    return 0;
  #endif
}
