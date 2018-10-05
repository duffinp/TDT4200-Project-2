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

	//All ranks except for the source rank delete the vertices, normals, and textures from the mesh.
	int mesh_size = meshs.size();
	int *vertex_sizes = new int[mesh_size];
	int *texture_sizes = new int[mesh_size];
	int *normal_sizes = new int[mesh_size];
	
	for (int i = 0; i < mesh_size; i++) {
		vertex_sizes[i] = meshs[i].vertices.size();
		texture_sizes[i] = meshs[i].textures.size();
		normal_sizes[i] = meshs[i].normals.size();
	}
	//int i = 0;
	if (world_rank > 0) {
		for (int i = 0; i < mesh_size; i++) {
			meshs[i].vertices.clear();
			meshs[i].textures.clear();
			normal_sizes[i] = meshs[i].normals.size();	meshs[i].normals.clear();
	//		std::cout << "Size of vertex vector at mesh " << i << " is " << vertex_sizes[i] << std::endl;
			meshs[i].vertices.resize(vertex_sizes[i], 0);
			meshs[i].textures.resize(texture_sizes[i], 0);
			meshs[i].normals.resize(normal_sizes[i]);
		}
	} 

	
	//Here we create the MPI data structures we need to pass them between processes.
	//MPI_Barrier(MPI_COMM_WORLD);

	MPI_Aint float4_offsets[] = {offsetof(float4, x), offsetof(float4, y), offsetof(float4, z), offsetof(float4, w)};
	MPI_Aint float3_offsets[] = {offsetof(float3, x), offsetof(float3, y), offsetof(float3, z)};

	int float4_lengths[] = {1, 1, 1, 1};
	int float3_lengths[] = {1, 1, 1};

	MPI_Datatype float4_types[] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
	MPI_Datatype float3_types[] = {MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};

	MPI_Datatype MPI_Float4, MPI_Float3;

	MPI_Type_create_struct(4, float4_lengths, float4_offsets, float4_types, &MPI_Float4);
	MPI_Type_create_struct(3, float3_lengths, float3_offsets, float3_types, &MPI_Float3);

	MPI_Type_commit(&MPI_Float4);
	MPI_Type_commit(&MPI_Float3);
	
	//std::cout << "The size of mesh is " << meshs.size() << std::endl;

	MPI_Barrier(MPI_COMM_WORLD);

	// Here we use a for loop to broadcast the vertices, textures and normals of each element of the mesh
	for (int i = 0; i < mesh_size; i++) {
		MPI_Bcast(&(meshs[i].vertices[0]), vertex_sizes[i], MPI_Float4, 0, MPI_COMM_WORLD);
		MPI_Bcast(&(meshs[i].textures[0]), texture_sizes[i], MPI_Float3, 0, MPI_COMM_WORLD);
		MPI_Bcast(&(meshs[i].normals[0]), normal_sizes[i], MPI_Float3, 0, MPI_COMM_WORLD);
	}
		std::cout << "Process " << world_rank << " has completed its broadcast." << std::endl;
		MPI_Barrier(MPI_COMM_WORLD);

	delete vertex_sizes;
	delete texture_sizes;
	delete normal_sizes;

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
