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

	//Compute number of vertices (which is also the same as the number of textures and normals)
	unsigned int mesh_size = meshs.size();
	int total_vertices = 0;
	if (world_rank == 0) {
		for (unsigned int i = 0; i < mesh_size; i++) {
			std::cout << "Mesh vertex size at " << i << " is " << meshs[i].vertices.size() << std::endl;
			total_vertices += meshs[i].vertices.size();
		}
		std::cout << "Total number of vertices is " << total_vertices << std::endl;
	}

	
	//These variables are for receiving tasks from the master
	std::vector<int> sub_tasks; // = new int(meshs.size());
	std::vector<int> master_tasks; // = new int(meshs.size());
	unsigned int counter = 0;
	unsigned int sub_size = 0;
	int new_task = 0;
	int assign_rank = 0;

	//This is where we start dividing the different objects to be rendered by different processes.
	if (world_rank == 0) {
		
		//This is an approximate number to help with load balancing
		int vertices_per_process = total_vertices / world_size;
		int vertices_cur_capacity = vertices_per_process;
		
		//The master rank assigns itself tasks first before sending them out to other ranks.
		while (assign_rank == 0) {
			for (unsigned int i = counter; i < meshs.size(); i++) {
				(sub_tasks).push_back(i);
				vertices_cur_capacity -= meshs[i].vertices.size();

				//If rank reaches its quota, then exit the for loop
				if (vertices_cur_capacity <= 0) {
					counter = i + 1;
					i = meshs.size();
				}
			}
			assign_rank += 1;
		}

		master_tasks = sub_tasks;
		while (assign_rank < world_size) {
			sub_tasks.clear();
			vertices_cur_capacity = vertices_per_process;

			//Until the quota is reached, keep sending new tasks to the assigned_rank
			for (unsigned int i = counter; i < meshs.size(); i++) {
				new_task = i;
				sub_tasks.push_back(i);
				vertices_cur_capacity -= meshs[i].vertices.size();
				MPI_Send(&new_task, 1, MPI_INT, assign_rank, i - counter + 1, MPI_COMM_WORLD);

				//If rank reaches its quota, then exit the for loop
				if (vertices_cur_capacity <= 0) {
					counter = i + 1;
					i = meshs.size();
				}
			}

			//This just tells each rank how many objects they are receiving.
			sub_size = sub_tasks.size();
			MPI_Send(&sub_size, 1, MPI_INT, assign_rank, 0, MPI_COMM_WORLD);

			assign_rank += 1;
		}
		sub_tasks = master_tasks;
		sub_size = sub_tasks.size();

	} else {
		//All other ranks must receive their instructions from the master.
		MPI_Recv(&sub_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (unsigned int i = 0; i < sub_size; i++) {
			MPI_Recv(&new_task, 1, MPI_INT, 0, i + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sub_tasks.push_back(new_task);
		}
		//std::cout << "Rank " << world_rank << " took on " << sub_size << " tasks." << std::endl;
		//std::cout << "Rank " << world_rank << " has its first job as " << sub_tasks[0] << std::endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);


	//Erase mesh objects that aren't assigned to your rank by shifting the assigned tasks to the front.
	for (unsigned int i = 0; i < sub_size; i++) {
		meshs[i] = meshs[i + sub_tasks[0]];
	}
	for (unsigned int j = 0; j < (mesh_size - sub_size); j++) {
		meshs.pop_back();
	}
		std::cout << "Rank " << world_rank << " took on " << meshs.size() << " tasks." << std::endl;


	std::vector<unsigned char> frameBuffer = rasterise(meshs, width, height, depth);

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
