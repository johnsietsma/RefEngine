#pragma once

#include "Vertex.h"

#include "FBXFile.h"

#include <assert.h>

struct VertexAttributes {

// Specialize this template to do custom vertex attribute setup for your struct.
template<typename T>
static void Setup() { assert(false); }


};


template<>
void VertexAttributes::Setup<Vertex_PositionColor>();

template<>
void VertexAttributes::Setup<Vertex_PositionTexCoord>();

template<>
void VertexAttributes::Setup<Vertex_PositionNormalTexCoord>();

template<>
void VertexAttributes::Setup<FBXVertex>();
