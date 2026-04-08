#include "MeshSerializer.h"
#include <vector>
#include <string>

using namespace dingus;

#define READ_4BYTE(var) fread(&var,1,4,f)
#define READ_2BYTE(var) fread(&var,1,2,f)

static inline std::string gReadString( FILE* f )
{
	std::string str;
	while( true ) {
		int c = fgetc( f );
		if( c==0 || c==EOF )
			break;
		str += (char)c;
	}
	return str;
}

CMesh* CMeshSerializer::loadMeshFromFile( const char* fileName )
{
	// open file
	FILE* f = fopen( fileName, "rb" );
	if( !f ) return nullptr;

	//
	// read header

	// magic
	char magic[4];
	READ_4BYTE(magic);
	if( magic[0]!='D' || magic[1]!='M' || magic[2]!='S' || magic[3]!='H' ) {
		fclose( f );
		return nullptr;
	}
	// header
	int nverts, ntris, ngroups, vstride, istride, vformat;
	READ_4BYTE(nverts);
	READ_4BYTE(ntris);
	READ_4BYTE(ngroups);
	READ_4BYTE(vstride);
	READ_4BYTE(vformat);
	READ_4BYTE(istride);
	assert( nverts > 0 );
	assert( ntris > 0 );
	assert( ngroups > 0 );
	assert( vstride > 0 );
	assert( istride==2 || istride==4 );
	assert( vformat != 0 );

	// read data
	std::vector<uint8_t> vbData(vstride * nverts);
	fread(vbData.data(), vstride, nverts, f);

	std::vector<uint8_t> ibData(istride * ntris * 3);
	fread(ibData.data(), istride, ntris * 3, f);

	std::vector<CMesh::CGroup> groups;
	for( int i = 0; i < ngroups; ++i ) {
		int vstart, vcount, fstart, fcount;
		READ_4BYTE(vstart);
		READ_4BYTE(vcount);
		READ_4BYTE(fstart);
		READ_4BYTE(fcount);
		// HACK: TBD: for null groups
		if( ngroups==1 && vcount==0 && fcount==0 ) {
			vstart = 0;
			vcount = nverts;
			fstart = 0;
			fcount = ntris;
		}
		groups.emplace_back(CMesh::CGroup(vstart, vcount, fstart, fcount));
	}

	// close file
	fclose( f );
	// HACK: some of demo meshes have position + normal + UV but we don't need the UVs.
	// Discard them to simplify pipeline management.
	if (vstride == 32 && vformat == 4099)
	{
		for (int i = 0; i < nverts; ++i)
		{
			memmove(vbData.data() + i * 24, vbData.data() + i * 32, 24);
		}
		vstride = 24;
		vformat = 3;
	}

	// init mesh
	CVertexFormat format(vformat);
	assert(format.calcVertexSize() == vstride);
	CMesh* mesh = new CMesh(nverts, ntris * 3, format, istride, vbData.data(), ibData.data(), ngroups, groups.data());
	return mesh;
}
