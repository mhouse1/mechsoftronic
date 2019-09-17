'''
Created on May 12, 2015

@author: MHouse1

@details: pseudo code for FPGA pulse generator acceleration and deceleration profile
            note: x and y axis must use same starting speed and speed change rate to ensure accurate plotting
                  VHDL will need to implement a version of this using state machine
'''
def generate_pulse_with_acceleration(steps = 200,pulse_high = 400000,pulse_low = 3000,starting_speed = 1000000,speed_change_rate = 2000):
    pulse_period = []

    actual_speed =starting_speed
    steps_taken_to_reach_nominal_speed = 0
    
    done_accelerating = False
    steps_allowed_to_accel = steps/2
    leftovers = 0
    for step_number in range(steps):
        high_count = 0
        low_count = 0
        printed = 0
        
        for high_pulse in range(actual_speed):
            high_count += 1
        #accelerate until desired speed by calculating what actual_speed should be
        if ((actual_speed - speed_change_rate) >= pulse_high)and (not done_accelerating) and (steps_allowed_to_accel > step_number):
            actual_speed = actual_speed - speed_change_rate
            steps_taken_to_reach_nominal_speed += 1
            print 'steps to accelerate = ',steps_taken_to_reach_nominal_speed
        else:
            if not done_accelerating:
                print 'done accelerating'
                leftovers = actual_speed - pulse_high
                if leftovers > 0:
                    steps_taken_to_reach_nominal_speed += 1
                actual_speed = pulse_high
                
                done_accelerating = True
            
        for low_pulse in range(actual_speed):
            low_count += 1
        #decelerate until starting speed by calculating what actual_speed should be
        steps_remaining = steps - step_number 
        if (steps_remaining < (steps_taken_to_reach_nominal_speed  + 2)):
            if leftovers > 0 :
                actual_speed = actual_speed + leftovers
                leftovers = 0
            else:
                actual_speed = actual_speed + speed_change_rate
    
               
        pulse_period.append((high_count,low_count))
    return pulse_period
if __name__ == '__main__':
    for period in generate_pulse_with_acceleration(steps = 30,pulse_high = 8,pulse_low = 5,starting_speed = 16,speed_change_rate = 7):
        print period[0]