/////////////////////////////////////////////////////////////////////////////
/// @file   acceleration.hpp
///
/// @date	12 oct. 2015 21:51:09
///
/// @author	Michael House
///
/// @brief  details on how acceleration and planning is handled
///
/// @details @todo move details to Design specification document later
///its possible to round off corners by replacing the corner and the area around with tiny
///segments of straight lines that form a curve. acceleration can then be easily implemented
///by slowing down speed for smaller segments. This method would be less accurate than one that
///does not replace corners with rounded off edges. This section describes an alternative method.
///
///Goal: implement acceleration while still retaining intended shape.
///API design:
///    Input data:
///        input will be a queue containing step number and direction to move on the X and Y axis.
///        ( input data is calculated in firmware by interpreting G-Code.)
///        An example of step and dir values to draw a triangle,
///        below is a list of tuples within tuples to hold this data
///        [((XStepNum,XDir),(YStepNum,YDir)),((XStepNum,XDir),(YStepNum,YDir)), ...]
///        The actual triangle data would be
///        [((3,1),(0,x)),((2,0),(3,1)),((0,0),(0,0))]
///        where x indicate "don't care which direction traveled"
///
///
///    FPGA motion control:
///        The design requires a xy movement to be synchronized. This means steps traveled on one axis
///        must be proportional to steps traveled on the other axis even while accelerating and deceleration.
///        number of steps and pulse width constants will be loaded into FPGA registers, the FPGA module would
///        normally generate the steps with a fixed pulse width clocked at a constant clock signal. To convert
///        this fixed pulse width output into something capable of acceleration, speed hold, and deceleration
///        an enable signal will be used to ignore some clock ticks.
///        For example: a clock may have a normal tick (! = 1 clock tick) interval of
///        [--------------------------------constant speed-------------------------------]
///        !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !  !
///        We want the clock to look like the following in order to implement acceleration/deceleration.
///        [-------acceleration---------][-----speed_hold-----][---------deceleration----------]
///
///        !       !      !     !    !   !  !  !  !  !  !  !  !  !   !    !     !      !       !
///        The above is what i call an accordion clock, it has 3 states: acceleration, speed_hold, and deceleration
///        Luckily, the FPGA module does not have to do all permutations of the 3 states.
///        The order will be one of these state orders:
///        where a = acceleration, h = speed_hold, d = deceleration
///        a     <- acceleration for during output of all steps
///        h     <- speed_hold for during output of all steps
///        d     <- deceleration for during output of all steps
///        a h d <- acceleration to speed hold to deceleration
///        a h   <- acceleration to speed hold
///        h d   <- speed hold to deceleration
///
///        we don't need to go from states like deceleration to acceleration, because that would mean there's a direction
///        change, in that case we'd get a new set of steps to output.
///
///        motion control input:
///            clock
///            enable
///            number of steps to accelerate
///            number of steps to hold_speed
///            number of steps to decelerate
///            axi1_total #of steps to travel
///            axi1_direction_axis
///            axi2_total #of steps to travel
///            axi2_direction_axis
///			   master_axi
///
///        motion control output:
///            step_signal
///            direction
///
///
///		  to be continued...
/// @par Copyright (c) 2015 All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef ACCELERATION_HPP_
#define ACCELERATION_HPP_





#endif /* ACCELERATION_HPP_ */
