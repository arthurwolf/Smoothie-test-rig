/*
    This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
    Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#include "libs/Module.h"
#include "libs/Kernel.h"
#include "Tester.h"
#include "libs/nuts_bolts.h"
#include "Config.h"
#include "StreamOutputPool.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "libs/Pin.h"
#include "mbed.h"

#define tester_module_enable_checksum        CHECKSUM("tester_module_enable")
#define tester_checksum                      CHECKSUM("tester")
#define green_led_pin_checksum               CHECKSUM("green_led_pin")
#define orange_led_pin_checksum              CHECKSUM("orange_led_pin")
#define red_led_pin_checksum                 CHECKSUM("red_led_pin")
#define green_button_pin_checksum            CHECKSUM("green_button_pin")
#define orange_button_pin_checksum           CHECKSUM("orange_button_pin")
#define red_button_pin_checksum              CHECKSUM("red_button_pin")
#define logic_switch_pin_checksum            CHECKSUM("logic_switch_pin")

Tester::Tester(){
}

void Tester::on_module_loaded() {
    if( !THEKERNEL->config->value( tester_module_enable_checksum )->by_default(true)->as_bool() ){
        // as not needed free up resource
        delete this;
        return;
    }
   
    // LED pins for the buttons 
    this->green_led_pin.from_string(  THEKERNEL->config->value(tester_checksum, green_led_pin_checksum  )->by_default("1.18!")->as_string())->as_output();
    this->orange_led_pin.from_string( THEKERNEL->config->value(tester_checksum, orange_led_pin_checksum )->by_default("1.19!")->as_string())->as_output();
    this->red_led_pin.from_string(    THEKERNEL->config->value(tester_checksum, red_led_pin_checksum    )->by_default("1.20!")->as_string())->as_output();

    // Set LED pins to off
    this->green_led_pin.set(false);
    this->orange_led_pin.set(false);
    this->red_led_pin.set(false);

    // Button pins
    this->green_button_pin.from_string(  THEKERNEL->config->value(tester_checksum, green_button_pin_checksum  )->by_default("1.22")->as_string())->as_input();
    this->orange_button_pin.from_string( THEKERNEL->config->value(tester_checksum, orange_button_pin_checksum )->by_default("1.23")->as_string())->as_input();
    this->red_button_pin.from_string(    THEKERNEL->config->value(tester_checksum, red_button_pin_checksum    )->by_default("1.24")->as_string())->as_input();

    // Power supply switch pins
    this->logic_switch_pin.from_string(  THEKERNEL->config->value(tester_checksum, logic_switch_pin_checksum  )->by_default("0.4")->as_string())->as_output();
    this->logic_switch_pin.set(false);

    // Button states
    this->green_button_state = true;

    // Register for events
    this->register_for_event(ON_MAIN_LOOP);

    // Make the downstream ( to the tested Smoothieboard ) Com object
    this->com = new Com();

}

void Tester::on_main_loop(void *argument){
    // Green button switching
    bool new_green_button_state = this->green_button_pin.get();
    if( this->green_button_state != new_green_button_state && new_green_button_state ){
        this->test_process(); 
    }
    this->green_button_state = new_green_button_state;

}

void Tester::test_process(){
    // A new test is starting
    this->log("info : New test starting\n");
    
    // Set the orange pin ON, indicating testing is on-going
    this->orange_led_pin.set(true);
    
    // Reset the status pins
    this->red_led_pin.set(false);
    this->green_led_pin.set(false);

    // Flush the serial port
    this->com->flush();

    // Turn the target Smoothieboard ON
    this->logic_switch_pin.set(true);
    
    // We wait for the welcome messages from the target Smoothieboard
    this->log("info : Waiting for welcome messages\n");

    string first_line, second_line, third_line, extra_line;
    bool got_first_line = this->com->get_line(&first_line, 5000);
    bool got_second_line = this->com->get_line(&second_line, 5000);
    bool got_third_line = this->com->get_line(&third_line, 5000);
    bool got_extra_line = this->com->get_line(&extra_line, 2000);

    if( got_first_line ){
        this->received(first_line);
    }else{ 
        this->log("error: Did not receive the first welcome line\n");
    }
    if( got_second_line ){ 
        this->received(second_line);
    }else{ 
        this->log("error: Did not receive the second welcome line\n");
    } 
    if( got_third_line ){
        this->received(third_line);
    }else{ 
        this->log("error: Did not receive the third welcome line\n");
    }
    if( got_extra_line ){
        this->received(third_line);
        this->log("error: Received one line too many\n");
    }else{
        this->log("info : Did not get any extra lines, end of welcome messages\n");
    }

    // At this point we received the welcome message so the board is working correctly




}

void Tester::received(string received){
    this->log("< rcv : '%s'\n", received.c_str());
}

void Tester::log(const char * format, ...){
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsprintf (buffer,format, args);
    THEKERNEL->streams->printf("%s", buffer);
    va_end (args);
}



