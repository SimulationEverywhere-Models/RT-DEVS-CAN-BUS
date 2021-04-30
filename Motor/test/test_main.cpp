
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


#include "../atomics/Motor.hpp"
#include "../atomics/CAN_controller.hpp"
#include "../../Display/atomics/can_input.hpp"
#include "../../data_structures/test_input.hpp"


  const char* PD_0;
  const char* PD_1;

struct reader_output : public iestream_input_defs<test_input>::out{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<test_input,T,reader_output> {
    public:
        InputReader_Message_t () = default;
        InputReader_Message_t (const char* file_path) : iestream_input<test_input,T,reader_output>(file_path) {}
};


using namespace std;

using hclock=chrono::high_resolution_clock;
using TIME = NDTime;



int main(int argc, char ** argv) {


    // all simulation timing and I/O streams are ommited when running embedded
    auto start = hclock::now(); //to measure simulation execution time

    static std::ofstream out_data("output/motor_out.txt");
    struct oss_sink_provider{
      static std::ostream& sink(){
        return out_data;
      }
    };
 



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
AtomicModelPtr input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME>("input_reader" , "input/test.txt");
AtomicModelPtr can_1 = cadmium::dynamic::translate::make_dynamic_atomic_model< CAN_controller, TIME>("can_1",PD_0,PD_1,2);
AtomicModelPtr Motor_1 = cadmium::dynamic::translate::make_dynamic_atomic_model< Motor, TIME>("Motor_1");
AtomicModelPtr test_input_1 = cadmium::dynamic::translate::make_dynamic_atomic_model< can_input, TIME>("test_1");



  /************************/
  /*******TOP MODEL********/
  /************************/

  cadmium::dynamic::modeling::Ports iports_m = {};
  cadmium::dynamic::modeling::Ports oports_m = {};
  cadmium::dynamic::modeling::EICs eics_m = {};
  cadmium::dynamic::modeling::EOCs eocs_m = {};
  cadmium::dynamic::modeling::Models submodels_m =  { input_reader, Motor_1, can_1, test_input_1};
  cadmium::dynamic::modeling::ICs ics_m = { 
    cadmium::dynamic::translate::make_IC<reader_output::out, can_test_defs::DataIn>("input_reader","test_1"),
    cadmium::dynamic::translate::make_IC<can_test_defs::DataOut, canController_defs::Rx>("test_1","can_1"),
    cadmium::dynamic::translate::make_IC<canController_defs::DataOut, Motor_defs::input>("can_1","Motor_1"),
    cadmium::dynamic::translate::make_IC<Motor_defs::output,canController_defs::DataIn>("Motor_1","can_1"),
  };
  CoupledModelPtr TOP = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
    "TOP",
    submodels_m,
    iports_m,
    oports_m,
    eics_m,
    eocs_m,
    ics_m
  );

  ///****************////


    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:00:10:000"));
    return 0;

}
