/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TESTER_MODULE_H
#define TESTER_MODULE_H

#include "libs/Module.h"
#include "Pin.h"
#include "Com.h"

class Tester : public Module{
    public:
        Tester();
        virtual ~Tester() {};
        void on_module_loaded();
        void on_main_loop(void* argument);
        void test_process();
        void received(string received);
        void log(const char * format, ...);

    private:
        Pin green_led_pin;
        Pin orange_led_pin;
        Pin red_led_pin;
        Pin green_button_pin;
        Pin orange_button_pin;
        Pin red_button_pin;

        Pin logic_switch_pin;

        bool green_button_state;

        Com* com;

};

#endif

