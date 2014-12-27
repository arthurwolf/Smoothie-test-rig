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
    // Set the orange pin ON, indicating testing is on-going
    this->orange_led_pin.set(true);
    
    // Reset the status pins
    this->red_led_pin.set(false);
    this->green_led_pin.set(false);

    // Turn the target Smoothieboard ON
    this->logic_switch_pin.set(true);





}
