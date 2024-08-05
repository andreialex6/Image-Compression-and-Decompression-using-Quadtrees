Quadtree Image Compression and Decompression

This repository contains a C program that implements a quadtree-based image compression and decompression algorithm. The program reads a PPM image, constructs a quadtree based on a similarity factor, and either outputs the tree structure or reconstructs the image from the tree.

The program accepts the following command-line arguments:

    -c1 factor input.ppm output.txt: Computes and outputs the height, number of leaf nodes, and the size of the largest undivided block of the quadtree;
    -c2 factor input.ppm output.bin: Constructs a quadtree and writes it to a binary file;
    -d input.bin output.ppm: Reads a quadtree from a binary file and reconstructs the image.
