problem options:
type newtonian
maxiter 100
dampening 0.67
tolerance 0.01

fluid options:
density 0.994
viscosity 0.8891

rugosity 0.035
diameter 12.00

pipes
1 lenght 100 singularities 10 start 1 end 2
2 lenght 300 singularities 10 start 1 end 4
3 lenght 300 singularities 10 diameter 10 start 2 end 3
4 lenght 100 singularities 10 diameter 10 start 4 end 3
5 lenght 100 singularities 10 diameter 25 rugosity 0.5 start 2 end 4

nodes
1 12 external
2 15
3 22 external
4 10

specifications
1 pressure 1
3 flow -8

