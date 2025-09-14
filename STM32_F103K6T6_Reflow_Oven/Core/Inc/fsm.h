#pragma once
#include "main.h"

typedef struct {
    uint32_t duration_s; // seconds
    uint32_t target_temperature_c; // Degrees Celsius
    uint8_t exit_on_temp_reached; // If 1, exit on target temp, if 0 exit on duration
    uint8_t heater_enabled; // If 1, disable heater in this state
} FSM_State;

extern uint8_t fsm_state_index;

extern uint8_t current_state_overtemperatures; // Count of how many times the temperature has exceeded target in current state
extern uint8_t overtemperature_count_threshold; // How many overtemperatures until we switch to the next states

extern uint32_t temperature_hysteresis; // millidegrees Celsius

extern uint32_t previous_state_start_time_ms;

extern uint8_t heater_currently_on;

extern FSM_State states[];

void FSM_Run(uint32_t temperature);
void Control_Heater(uint32_t target_temperature, uint32_t current_temperature);
void Set_Heater_GPIO(int state);
void Set_FSM_State_Index(uint8_t index);
