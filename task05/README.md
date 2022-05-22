# Task05: Rasterization of Bezier Curve (Sturm's Method, Bisection Method)

![preview](preview.png)

**Deadline: May 26th (Thu) at 15:00pm**

----

## Before Doing Assignment

If you have not done the [task00](../task00), do it first to set up the C++ graphics development environment.

Follow [this document](../doc/submit.md) to submit the assignment, In a nutshell, before doing the assignment,  
- make sure you synchronized the `main ` branch of your local repository  to that of remote repository.
- make sure you created branch `task05` from `main` branch.
- make sure you are currently in the `task05` branch (use `git branch -a` command).

Now you are ready to go!

---

## Problem 1

1. Build the code using cmake
2. Run the code
3. Take a screenshot image (looks like image at the top)
4. Save the screenshot image overwriting `task05/problem1.png`

![problem1](problem1.png)


## Problem 2

Write some code (about 5 ~ 10  lines) around `line #185` in `shader.frag`  to define compute the minimum distance to the cubic Bezier curve using the Sturm's method. The minimum distance can be found by computing the root of the quintic polynominal (i.e., 5th order polynominal). The polynominal here is the square of the distance differentiated by the parameter. Read the instruction in the `shader.frag` for more information. 

Save the screenshot image overwriting `task05/problem2.png`

![problem2](problem2.png)


## After Doing the Assignment

After modify the code, push the code and submit a pull request. 
