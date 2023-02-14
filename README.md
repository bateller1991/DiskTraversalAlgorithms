# DiskTraversalAlgorithms

This is a C program that simulates four different disk traversal algorithms - SSTF, FIFO, SCAN, and CSCAN. The program takes user input for the disk sequence and traverses it according to the selected algorithm. The disk size is limited to a range of 1-832.


# Algorithms

SSTF (Shortest Seek Time First)
This algorithm selects the disk request with the shortest distance from the current head position.

FIFO (First In, First Out)
This algorithm services requests in the order they are received.

SCAN (Elevator Algorithm)
This algorithm services requests in one direction until the end of the disk is reached, then changes direction and services requests in the other direction.

CSCAN (Circular SCAN)
This algorithm is similar to SCAN, but instead of reversing direction at the end of the disk, it jumps to the beginning of the disk and continues in the original direction.

# Report
At the end of the program, a report will be printed to the screen that includes the total number of head movements and the average seek time for the selected algorithm.

# License
This program is licensed under the MIT License. See the LICENSE file for more information.

# Credits
This program was created for an Operating Systems class at California State University, Northridge. It was developed by Brandon Teller.
