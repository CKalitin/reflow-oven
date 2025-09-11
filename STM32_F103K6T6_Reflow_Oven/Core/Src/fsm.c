#include "fsm.h"

uint8_t fsm_state_index = 0;

uint32_t temperature_hysteresis = 1000; // millidegrees Celsius

uint32_t previous_state_start_time_ms = 0;

uint8_t heater_currently_on = 0;

FSM_State states[] = {
    // duration (s), target temp (C), exit on temp reached, heater enabled
    {5, 0, 0, 0},
    {5, 100, 0, 1},
    {0, 100, 1, 1},
    {20, 100, 0, 1},
    {0, 150, 1, 1},
    {20, 150, 0, 1},
    {0, 200, 1, 1},
    {20, 200, 0, 1},
    {0, 240, 1, 1},
    {10, 240, 0, 1},
    {999999, 0, 0, 0}
};

void FSM_Run(uint32_t temperature) {
    if (states[fsm_state_index].exit_on_temp_reached) {
        if (temperature >= states[fsm_state_index].target_temperature_c * 1000) { // Convert C to mC by *1000
            Set_FSM_State_Index(fsm_state_index + 1);
        }
    } else {
        int elapsed_time_ms = HAL_GetTick() - previous_state_start_time_ms;
        if (elapsed_time_ms >= states[fsm_state_index].duration_s * 1000) {
            Set_FSM_State_Index(fsm_state_index + 1);
        }
    }

    Set_Heater_GPIO(GPIO_PIN_RESET); // Disable heater by default
    if (states[fsm_state_index].heater_enabled) {
        Control_Heater(states[fsm_state_index].target_temperature_c, temperature);
    }
}

void Control_Heater(uint32_t target_temperature, uint32_t current_temperature) {
    Set_Heater_GPIO(GPIO_PIN_RESET); // Turn heater OFF
    if (current_temperature < target_temperature * 1000 - temperature_hysteresis) { // Convert mC to C by *1000
        Set_Heater_GPIO(GPIO_PIN_SET); // Turn heater ON if temp is low enough
    }
}

void Set_Heater_GPIO(int state) {
    HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, state);
    heater_currently_on = state;
}

void Set_FSM_State_Index(uint8_t index) {
    fsm_state_index = index;
    previous_state_start_time_ms = HAL_GetTick();
}