// #define __DEBUG

__constant size_t kBlockSize = 256;
__constant size_t kKeySize = 128;
__constant unsigned char localLinkFunction[64] = 
	{1, 0, 0, 1, 1, 0, 0, 0, 
	 1, 0, 0, 1, 0, 1, 1, 1, 
	 1, 1, 0, 0, 0, 0, 1, 0, 
	 1, 1, 0, 0, 1, 1, 0, 1, 
	 1, 0, 0, 1, 1, 0, 0, 0, 
	 1, 0, 0, 1, 0, 1, 1, 1, 
	 0, 0, 1, 1, 1, 1, 0, 1, 
	 0, 0, 1, 1, 0, 0, 1, 0};


__kernel void ecb_kernel(__global unsigned char * plainText,
                         __global unsigned char * cipherText,
                         __global unsigned char * key,
                         __global unsigned char * constantValue, // TODO: move it to local constants
                         __global unsigned int * adjacencyList,
#ifdef __DEBUG
                         __global unsigned int *adjacencyListCopy,
#endif
                         unsigned int vertexCount,
                         unsigned int vertexEdgeCount,
                         unsigned int blockSize,
                         unsigned int keySize,
                         unsigned int constantSize
                         )
{
	size_t globalId = get_global_id(0);
	size_t blockId = get_group_id(0);           // 0:dataSize / localSize
	size_t localId = get_local_id(0);
	size_t localSize = get_local_size(0);       // 256

	__local unsigned char keySchedule[64 * 4];

	__local unsigned char left[128], right[128];     // TODO: allocate these arrays outside of kernel
	__local unsigned char cellValues[230];
	__local unsigned char cellValuesNew[230];
	__local unsigned char temp[128];     // TODO: try to use private memory

	__local unsigned int alCopy[1380];

	if (localId < 230) {
		for (int i = 0; i < 6; ++i)
			alCopy[localId + 230 * i] = adjacencyList[localId + 230 * i];
	}

	if (localId < 128) {
		keySchedule[localId] = key[localId];
	}
	if (localId >= 128 && localId < 192) {
		keySchedule[localId] = key[64 - (localId - 128)];
	}
	if (localId >= 192 && localId < 256) {
		keySchedule[localId] = key[128 - (localId - 192)];
	}

	size_t offset = 0;
	if (localId < 230) {
		offset = localId * vertexEdgeCount;
	}

	// load initial plaintext
	if (localId < 128) {
		left[localId] = plainText[localId + blockId * kBlockSize];
	} else {
		right[localId - 128] = plainText[localId + blockId * kBlockSize];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	unsigned long arg = 0;
	for (size_t roundNumber = 0; roundNumber < 4; ++roundNumber) {
		if (localId < 128) {
			cellValues[localId] = temp[localId] = right[localId];
		}

		if (localId >= 128 && localId < 192) {
			cellValues[localId] = keySchedule[roundNumber * (localId - 128)];
		}

		if (localId >= 192 && localId < 230) {
			cellValues[localId] = constantValue[localId - 192];
		}

		// compute a few iterations of CA
		barrier(CLK_LOCAL_MEM_FENCE);
		for (size_t it = 0; it < 8; ++it) {
			arg = 0;
			if (localId < vertexCount) {
				for (int i = 0; i < vertexEdgeCount; ++i) {
				// if (localId < vertexEdgeCount) {
					if (it % 2) {
						// odd iteration: cellValuesNew -> arg
						// arg |= cellValuesNew[alCopy[localId + offset]] << localId;
						arg |= cellValuesNew[alCopy[i + offset]]<<i ;
					} else {
						// even iteration cellValues -> arg
						// arg |= cellValues[alCopy[localId + offset]] << localId;
						arg |= cellValues[alCopy[i + offset]]<<i;
					}
				}
				// compute local link function
				if (it % 2) {
					// odd iteration: arg -> cellValues
					cellValues[localId] = localLinkFunction[arg];
				} else {
					// even iteration: arg -> cellValuesNew
					cellValuesNew[localId] = localLinkFunction[arg];
				}
			}
			barrier(CLK_LOCAL_MEM_FENCE);
		}
		// L = R(-1)
		// R = L(-1) (+) cells[0:63]
		if (localId < 128) {
			right[localId] = left[localId] ^ cellValues[localId];
			left[localId] = temp[localId];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	// now combine L||R and write to ciphertext
	if (localId < 128) {
		cipherText[localId + kBlockSize * blockId] = left[localId];
	} else {
		cipherText[localId + kBlockSize * blockId] = right[localId - 128];
	}
}

