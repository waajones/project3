# project3
OpenMP

Should work as expected. For help on OpenMP directives (how to use them, and when to use them), I used IBMs manual on parallelization. I will add the link to that in here. From there you can click on the drop down menues and see the syntax and rules for each. More specifically, I used the manual for omp collapse and omp critical. I used omb collapse for making sure nested loops are all read by omp as one big loop. and used critical to make sure only 1 thread is allowed to access a cell at a time, making sure that 2 different threads are not updating a varible at the same time, to 2 different values.

https://www.ibm.com/docs/en/xffbg/121.141?topic=fortran-parallelization-directives
