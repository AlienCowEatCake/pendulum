# Virtual Physical Laboratory “Mass Spring Damper System”

**Project Mission:** to develop a virtual physical laboratory for simulation a free damped oscillations of a spring pendulum

**Programming Language:** C++

**Framework:** Qt4 / Qt5

**License:** [GNU GPL v3](http://www.gnu.org/copyleft/gpl.html)

## System Description

This software simulates free damped oscillations of a spring pendulum.

The software allows you to adjust the following parameters of the model:
 * Mass
 * Initial displacement
 * Spring constant
 * Damping coefficient

The stopwatch, the experiment speed slider and the ruler to measure the displacement were added to improve usability.

The software allows you to plot graphs of the following functions:
 * Displacement as a function of time
 * Speed as a function of time
 * Energy as a function of time

## Model

![image](src/Pendulum/resources/html/help/image001.png)

![r](src/Pendulum/resources/html/help/image002.png) — damping coefficient,

![k](src/Pendulum/resources/html/help/image004.png) — spring constant,

![m](src/Pendulum/resources/html/help/image006.png) — mass,

![A_0](src/Pendulum/resources/html/help/image008.png) — initial displacement,

![x](src/Pendulum/resources/html/help/image010.png) — displacement,

![t](src/Pendulum/resources/html/help/image012.png) — time,

![ω_0=sqrt(k/m)](src/Pendulum/resources/html/help/image014.png) — ringing frequency of free undamped oscillations,

![β=r/(2*m)](src/Pendulum/resources/html/help/image016.png) — attenuation rate,

![ω=sqrt(ω_0^2-β^2)](src/Pendulum/resources/html/help/image018.png) — ringing frequency of free damped oscillations,

![T=2*π/sqrt(ω_0^2-β^2)](src/Pendulum/resources/html/help/image020.png) — period of oscillation,

![A_T=A_0*exp(-β*T)*cos(ω*T)](src/Pendulum/resources/html/help/image022.png) — oscillation amplitude at the time equal to the period,

![θ=ln(A_0/A_T)](src/Pendulum/resources/html/help/image024.png) — logarithmic decrement,

![E_0=k*A_0^2/2](src/Pendulum/resources/html/help/image026.png) — energy at the start of the experiment,

![x=A_0*exp(-β*t)*cos(ω*t)](src/Pendulum/resources/html/help/image028.png) — current displacement,

![v=Δx/Δt](src/Pendulum/resources/html/help/image030.png) — current speed,

![E=E_0*exp(-2*β*t)](src/Pendulum/resources/html/help/image032.png) — current energy.

