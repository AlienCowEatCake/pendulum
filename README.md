# Виртуальная физическая лаборатория «Пружинный маятник в среде с сопротивлением»

**Цель проекта:** разработать программный продукт, представляющий собой виртуальную физическую лабораторию, моделирующую поведение пружинного маятника в среде с сопротивлением

**Язык программирования:** C++

**Фреймворк:** Qt4 / Qt5 / Qt6

**Лицензия:** [GNU GPL v3](http://www.gnu.org/copyleft/gpl.html)

## Описание установки

В программе моделируются свободные затухающие колебания пружинного маятника.

Существует возможность изменять следующие параметры модели:
 * массу груза
 * начальное смещение груза от положения равновесия
 * коэффициент жёсткости пружины
 * коэффициент сопротивления среды

Для удобства проведения эксперимента добавлены секундомер и возможность изменения скорости опыта, а так же линейка для измерения смещения груза от положения равновесия.

В программе могут быть выведены графики следующих зависимостей:
 * смещения от времени
 * скорости от времени
 * энергии от времени

## Модель

![image](src/Pendulum/resources/html/help/image001.png)

![r](src/Pendulum/resources/html/help/image002.png) — коэффициент сопротивления среды,

![k](src/Pendulum/resources/html/help/image004.png) — коэффициент жёсткости пружины,

![m](src/Pendulum/resources/html/help/image006.png) — масса груза,

![A_0](src/Pendulum/resources/html/help/image008.png) — начальная амплитуда,

![x](src/Pendulum/resources/html/help/image010.png) — смещение груза,

![t](src/Pendulum/resources/html/help/image012.png) — время,

![ω_0=sqrt(k/m)](src/Pendulum/resources/html/help/image014.png) — циклическая частота свободных незатухающих колебаний,

![β=r/(2*m)](src/Pendulum/resources/html/help/image016.png) — коэффициент затухания,

![ω=sqrt(ω_0^2-β^2)](src/Pendulum/resources/html/help/image018.png) — циклическая частота свободных затухающих колебаний,

![T=2*π/sqrt(ω_0^2-β^2)](src/Pendulum/resources/html/help/image020.png) — период колебаний,

![A_T=A_0*exp(-β*T)*cos(ω*T)](src/Pendulum/resources/html/help/image022.png) — амплитуда колебаний в момент времени, равный периоду,

![θ=ln(A_0/A_T)](src/Pendulum/resources/html/help/image024.png) — логарифмический декремент,

![E_0=k*A_0^2/2](src/Pendulum/resources/html/help/image026.png) — энергия в начале эксперимента,

![x=A_0*exp(-β*t)*cos(ω*t)](src/Pendulum/resources/html/help/image028.png) — смещение в текущий момент времени,

![v=Δx/Δt](src/Pendulum/resources/html/help/image030.png) — скорость в текущий момент времени,

![E=E_0*exp(-2*β*t)](src/Pendulum/resources/html/help/image032.png) — энергия в текущий момент времени.

