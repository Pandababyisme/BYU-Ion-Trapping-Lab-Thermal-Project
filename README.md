# BYU-Ion-Trapping-Lab-Thermal-Project
Hi, I am Xiao Li. This is the project I worked on during summer of 2026, which is a thermal monitoring solution for an ion trapping quantum information processing lab. I will provide as detailed steps as I can here. Hopefully, it can be useful one day for you. :)

## 1. Preview
   In an ion trapping lab, different components and machines coperate as a whole to function the state-of-art quantum information process. While this happening, constronted by phsical phnominone, heat drfit will occur. And for our propuse we want to monitor the thermal information to get a better idea of what is going on.

## 2. Solution
   We proposed a solution of a system that consists one arduino, six maxchips, six pt100 sensors, and various supporting parts. In the following, we will step into more detailed description of different components, introductions of the way to make them work, assemble individual parts into a system and mislinious steps of supporting.

### 2.1)Arduino 
As the introduction, we will need one Arduino. The one we used is Arduino UNO R4 minima, personally I think it is a pretty powerful and realiable source for this project. In our solution, an arduino will role as the brain in system, which will suffit the tasks as: data collection, communicating with six maxchips via SPI protocal, transimitting data to our computer, etc. And the way we implement those tasks is through Arduino IDE. By uploading proper code to Arduino, it will perform the way as desired, and complete tasks. So we will include a Arduino IDE code file in this repository as an example of one solution. 

#### 2.11) Code design of the Arduino 


