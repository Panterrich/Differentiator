# Differetiator 

This program can calculate the full first-order differential of a function of many variables. At the same time, a dump is generated in the form of LaTeX with an interesting story. 

### The principle of operation:

Using the syntax analysis implemented by recursive descent, we transform the original expression into a tree. Then the derivative is calculated recursively.

The following optimizations are used for calculations: 

* Convolution of constants

* Removing neutral elements

* Computation of the derivative at the translation stage

And then the values for the variables are substituted (which will need to inputted), then the TeX file is compiled.

### Example of the program operation: 

In file "equation.txt" input the expression. The output is a file ["images/equation.pdf"](images/equation.pdf).