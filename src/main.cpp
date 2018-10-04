#include <mpi.h>
#include <chrono>
#include <time.h>
#include <iostream>
#include <cstring>
#include "utilities/OBJLoader.hpp"
#include "utilities/lodepng.h"
#include "rasteriser.hpp"

int main(int argc, char **argv) {
 
	auto start = clock();

	// Initialize MPI framework
	MPI_Init(NULL, NULL);

    	// Get the number of processes
    	int world_size;
    	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    	// Get the rank of the process
    	int world_rank;
    	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//printf("Rank %d out of %d processors\n", world_rank, world_size);
	std::string input("../input/sphere.obj");
	std::string output("../output/sphere.png");
	unsigned int width = 1920;
	unsigned int height = 1080;
	unsigned int depth = 3;


	float rotationAngle = 0;
	if (world_rank == 0) {
		// Master rank computes all of the rotation angles and iteratively sends them out to each process.
		for (int i = 1; i < world_size; i++) {
			rotationAngle = 180.0*((float) i/ (float) (world_size - 1));
			MPI_Send(&rotationAngle, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}
		rotationAngle = 0;
	} else if (world_rank > 0) {
		// Each process receives its rotation angle from the master rank.
		MPI_Recv(&rotationAngle, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	for (int i = 1; i < argc; i++) {
		if (i < argc -1) {
			if (std::strcmp("-i", argv[i]) == 0) {
				input = argv[i+1];
			} else if (std::strcmp("-o", argv[i]) == 0) {
				output = argv[i+1];
			} else if (std::strcmp("-w", argv[i]) == 0) {
				width = (unsigned int) std::stoul(argv[i+1]);
			} else if (std::strcmp("-h", argv[i]) == 0) {
				height = (unsigned int) std::stoul(argv[i+1]);
			} else if (std::strcmp("-d", argv[i]) == 0) {
				depth = (int) std::stoul(argv[i+1]);
			}
		}
	}
	std::cout << "Loading '" << input << "' file... " << std::endl;

	std::vector<Mesh> meshs = loadWavefront(input, false);

	std::vector<unsigned char> frameBuffer = rasterise(meshs, width, height, depth, rotationAngle);

	int idx = output.length() - 4;
	output.insert(idx, std::to_string(world_rank));

	std::cout << "Writing image to '" << output << "'..." << std::endl;

	unsigned error = lodepng::encode(output, frameBuffer, width, height);

	if(error)
	{
		std::cout << "An error occurred while writing the image file: " << error << ": " << lodepng_error_text(error) << std::endl;
	}

	MPI_Finalize();

	auto end = clock();

	float timeTaken = float (end - start)/CLOCKS_PER_SEC;
	std::cout << "To run process " << world_rank << ", it took " << timeTaken << " seconds." << std::endl;

	return 0;
}
