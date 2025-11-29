#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "../renderer/mesh.h"

/* Load mesh from OBJ file */
Mesh* obj_loader_load(const char* filepath);

/* Parse OBJ from memory buffer */
Mesh* obj_loader_parse(const char* data, u32 data_size);

#endif /* OBJ_LOADER_H */
