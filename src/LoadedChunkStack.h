#ifndef LOADEDCHUNKSTACK_H
#define LOADEDCHUNKSTACK_H

#include "ChunkStack.h"
#include "VulkanWrapper.h"
#include "Quad.h"
#include "ObjArray.h"
#include "AABB.h"

#include <atomic>

class LoadedChunkStack {
public:
	LoadedChunkStack(VulkanWrapper &vulkanWrapper, ObjArray *objArray);

	~LoadedChunkStack();

	/**
	 * @brief Contains all loaded chunks.
	 */
	ChunkStack chunkStack;

	ChunkStack leftStack;
	ChunkStack rightStack;
	ChunkStack frontStack;
	ChunkStack backStack;
	ChunkStack frontLeftStack;
	ChunkStack frontRightStack;
	ChunkStack backLeftStack;
	ChunkStack backRightStack;

	Chunk emptyChunk;

	AABB aabb;

	bool chunkStackReady = false;
	bool chunkRemoved = false;
	bool willBeRemoved = false;
	volatile std::atomic<int> lifeCounter = 10;

	/**
	 * @brief Array of the vertex buffers of the chunks.
	 */
	std::vector<VkBuffer> chunkVertexBuffer;

	/**
	 * @brief Array of the vertex buffer memorys of the chunks.
	 */
	std::vector<VkDeviceMemory> chunkVertexBufferMemory;

	/**
	 * @brief Array of the index buffers of the chunks.
	 */
	std::vector<VkBuffer> chunkIndexBuffer;

	/**
	 * @brief Array of the index buffer memorys of the chunks.
	 */
	std::vector<VkDeviceMemory> chunkIndexBufferMemory;

	/**
	 * @brief Array of the indices counts of the chunks.
	 */
	std::vector<uint32_t> chunkIndexCount;

	std::vector<VkBuffer> objVertexBuffer;
	std::vector<VkDeviceMemory> objVertexBufferMemory;
	std::vector<VkBuffer> objIndexBuffer;
	std::vector<VkDeviceMemory> objIndexBufferMemory;
	std::vector<uint32_t> objIndexCount;


	/**
	 * @brief Generates vulkan objects and data for all chunks in loaded chunks.
	 */
	void generateVulkanChunks();

	void deleteVulkanChunks();

	void generateBoxData(std::vector<BoxData> &boxData, std::vector<BoxData> &emissiveBoxData);

private:
	/**
	 * @brief Updates all necessary vectors to add space in the y dimension for a new chunk on top of the already existing ones.
	 *
	 * @param x X coordinate of the chunk.
	 * @param z Z coordinate of the chunk.
	 */
	void updateHeight();

	/**
	 * @brief Handle of the vulkan wrapper to get acces to the vulkan buffer generation.
	 */
	VulkanWrapper *vulkanWrapper;

	/**
	 * @brief Generates for a given chunk the needed vulkan objects and data to be able to render the cunk.
	 *
	 * @param x X coordinate of the chunk.
	 * @param z Z coordinate of the chunk.
	 * @param y Y coordinate of the chunk.
	 */
	void generateVulkanChunk(int const y);

	void deleteVulkanChunk(int const y);

	/**
	 * @brief Checks if a given cube inside the referenced chunk can be culled, because the cube is not visible
	 *
	 * @param u U coordinate of the cube.
	 * @param w W coordinate of the cube.
	 * @param v V coordinate of the cube.
	 * @param chunk Chunk in which the cube is located.
	 * @return true If the cube can be culled, because it is not visible.
	 * @return false If the cube can not be culled because it is visible.
	 */
	bool cullCube(int const u, int const w, int const v, int const y);

	uint8_t calculateAmbientOcclusionValue(int const u, int const w, int const v, int const y, int const vertexID);

	uint8_t occlusion(CubeType const side1, CubeType const side2, CubeType const corner);

	bool flippedTriangles(uint8_t const ambientOcclusionValue0, uint8_t const ambientOcclusionValue1, uint8_t const ambientOcclusiontValue2, uint8_t const ambientOcclusionValue3);

	void calculateLightLevels();

	int8_t getLightLevel(int const u, int const w, int const v, int const y, int const side);
	
	bool cullSide(int const u, int const w, int const v, int const y, int const side);

	void greedyMesh1D(std::vector<Quad> &cubeSideQuads, int const side, bool const isX);

	bool fitsTogether(Quad const &main, Quad const &addition, int const side, bool const isX);

	void greedyMesh2D(std::vector<Quad> &cubeSideQuads, int const side);

	ObjArray *objArray;
};

#endif // !LOADEDCHUNKSTACK_H