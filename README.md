# Applied Computer Graphics 4860-1084

[![Build_Win](https://github.com/ACG-2022S/acg/actions/workflows/windows.yml/badge.svg)](https://github.com/ACG-2022S/acg/actions/workflows/windows.yml)
[![Build_Linux](https://github.com/ACG-2022S/acg/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ACG-2022S/acg/actions/workflows/ubuntu.yml)

Lecture at graduate school of information science and technology in the university of Tokyo, spring semester, 2022

ITC-LMS (for Slack and GitHub Classroom invitaitons): 

- https://itc-lms.ecc.u-tokyo.ac.jp/lms/course?idnumber=20224860-10840F01


## Instructor

Dr. Nobuyuki Umetani 
- email: umetani@ci.i.u-tokyo.ac.jp
- url: http://www.nobuyuki-umetani.com/
- lab's web page: https://cgenglab.github.io/labpage/en/

## Time

Monday 3rd period, 13:00pm - 14:30pm


## Course Description

Computer graphics is a technology to computationally represent objects' geometry, appearance and movement. This course is an introduction to the techniques generally seen in computer graphics. The aim of the course is to get familiar with applied mathematics such as linear algebra, vector analysis, partial differential equations, numerical analysis and optimization through the topics in computer graphics. There are C++ programming assignments to acquire research-oriented graphics programming skills such as OpenGL, Eigen matrix library, Git and cmake. 

Topics:
- affine transformation & homography
- character animation (forward & inverse kinematics)
- visualization (rasterization / ray casting)
- optimization ( continuous optimization / dynamic programming )
- parametric curves & surfaces
- variational mesh deformation
- grid-based fluid simulation


## Lecture Schedule

| Day | Topic | Assignment | Slide |
|:----|:---|:---|:---|
|(1)<br>Apr. 11| **Introduction**<br>graphics pipeline |  | [[3]](http://nobuyuki-umetani.com/acg2022s/graphics_pipeline.pdf) |
|(2)<br>Apr. 18| **Coordinate transfrormation**<br>barycentric transformation | [task00](task00) | [[4] ](http://nobuyuki-umetani.com/acg2022s/barycentric_coordinate.pdf), [[5]](http://nobuyuki-umetani.com/acg2022s/transformation.pdf) |
|(3)<br>Apr. 25| Character deformation Ⅰ | [task02](task02) | - |
|(4)<br>May 2| Ray Casting | task03 | - |
|(5)<br>May 9| Rasterization | task04 | - |
|(6)<br>May 16| Optimization Ⅰ | task05 | - |
|(7)<br>May 23| Optimization Ⅱ | task06 | - |
|(8)<br>June 6|  Character deformation Ⅱ | task07 | - |
|(9)<br>June 13| Variational mesh deformation | task08 | - |
|(10)<br>June 20| Parametric curves / surfaces | task09 | - |
|(11)<br>June 27| Guest lecture by Dr. Ryoichi Ando | - | - |
|(12)<br>July 4| Grid-based Fluid Ⅰ | task10 | - |
|(13)<br>July 11| Grid-based Fluid Ⅱ | - | - |




## Grading

- 20% lecture attendance
  - Attendance is counted based on writing a secret keyword on LMS. The keyword is announced for each lecture.  
- 80% small assignments
  - see below

## Assignemnts

There are many small programming assignments. To do the assignments, you need to create your own copy of this repository through **GitHub Classroom**.  These assignements needs to be submitted using **pull request** functionality of the GitHub. Look at the following document. 

[How to Submit the Assignments](doc/submit.md)

### Policy

- Do the assignment by yourself. Don't share the assignments with others.
- Don't post the answers of the assignment on Slack 
- Late submission of an assignment is subject to grade deduction
- Score each assignemnt will not be open soon (instructer needs to adjust weight of the score later)

### Tasks

- [task00](task00): build C++ Program with CMake
- [task01](task01): Affine transformation
- [task02](task02): articulated rigid bones
- [task03](task03): ray casting 
- [task04](task04): rastering
- [task05](task05): navigation mesh
- [task06](task06): laplacian mesh deformation
- [task07](task07): inverse kinematics
- [task08](task08): as-rigid-as possible mesh deformation
- [task09](task09): subdivision surface
- [task11](task11): FTDT simulation


## Slides

- [[1] C++ programming](http://nobuyuki-umetani.com/acg2022s/cpp.pdf)
- [[2] Git+GitHub](http://nobuyuki-umetani.com/acg2022s/git.pdf)
- [[3] Graphics pipeline](http://nobuyuki-umetani.com/acg2022s/graphics_pipeline.pdf)
- [[4] Barycentric coordinate](http://nobuyuki-umetani.com/acg2022s/barycentric_coordinate.pdf)
- [[5] Coordinate transformation](http://nobuyuki-umetani.com/acg2022s/transformation.pdf)

## Reading Material
- [Introduction to Computer Graphics by Cem Yuksel](https://www.youtube.com/watch?v=vLSphLtKQ0o&list=PLplnkTzzqsZTfYh4UbhLGpI5kGd5oW_Hh)
- [Scratchpixel 2.0](https://www.scratchapixel.com/)
- [Awesome Computer Graphics (GitHub)](https://github.com/luisnts/awesome-computer-graphics)
- [Skinning: Real-time Shape Deformation SIGGRAPH 2014 Course](https://skinning.org/)
- [Physics-based Animation2022S (Another course by the instructor) ](https://github.com/nobuyuki83/Physics-based_Animation_2021S)

