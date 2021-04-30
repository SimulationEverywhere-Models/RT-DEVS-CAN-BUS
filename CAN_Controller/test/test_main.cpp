#ifndef RT_ARM_MBED

#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

#include <cadmium.h>

#include <NDTime.hpp>
#include <cadmium/io/iestream.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

#include "../atomics/CAN_controller.hpp"
#include "../atomics/float_to_internal.hpp"


#include <cadmium/real_time/arm_mbed/io/analogInput.hpp>

using namespace std;
using namespace cadmium;


const char* PA_3 = "test.txt";
const char* PD_0 = "none";
const char* PD_1 = "none";
const char* PB_5 = "none";
const char* PB_6 = "none";




using hclock=chrono::high_resolution_clock;
using TIME =  NDTime;

//struct output_port : cadmium::out_port<internal_msg> {};

int main(int argc, char ** argv)
{


    auto start = hclock::now(); //to measure simulation execution time

    static std::ofstream out_data("./can_output.txt");
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
    /**************** CAN Controllers ***********************/
    /********************************************/
    AtomicModelPtr CAN_1 = cadmium::dynamic::translate::make_dynamic_atomic_model<CAN_controller, TIME>("CAN1",PD_0,PD_1,1);  
    AtomicModelPtr CAN_2 = cadmium::dynamic::translate::make_dynamic_atomic_model<CAN_controller, TIME>("CAN2",PB_5,PB_6,2);  
   

    // /********************************************/
    // /************ iternal msg converter **********/
    // /********************************************/


    AtomicModelPtr f_to_internal = cadmium::dynamic::translate::make_dynamic_atomic_model<float_to_internal, TIME>("f_to_internal",1100);
 
    AtomicModelPtr input = cadmium::dynamic::translate::make_dynamic_atomic_model<AnalogInput, TIME>("input",PA_3);
  
    /************************/
    /*******TOP MODEL********/
    /************************/


    cadmium::dynamic::modeling::Ports iports_TOP = {};
    cadmium::dynamic::modeling::Ports oports_TOP = {};
    cadmium::dynamic::modeling::Models submodels_TOP =  {input, f_to_internal, CAN_1, CAN_2};
    cadmium::dynamic::modeling::EICs eics_TOP = {};
    cadmium::dynamic::modeling::EOCs eocs_TOP = {};
    cadmium::dynamic::modeling::ICs ics_TOP = {
        cadmium::dynamic::translate::make_IC<analogInput_defs::out,floatToInternal_defs::DataIn>("input","f_to_internal"),
        cadmium::dynamic::translate::make_IC<floatToInternal_defs::DataOut, canController_defs::DataIn>("f_to_internal","CAN1"),
        cadmium::dynamic::translate::make_IC<canController_defs::Tx, canController_defs::Rx>("CAN1","CAN2")
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


    cadmium::dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(TIME("01:00:00:0000"));

    return 0;
}


#endif