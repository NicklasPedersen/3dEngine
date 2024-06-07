#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const char *sFilename)
{
	std::ifstream f(sFilename);
	if (!f.is_open())
		return;

	std::vector<Vector3> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			Vector3 v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}
		else if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			Vector3 v0 = verts[f[0] - 1];
			Vector3 v1 = verts[f[1] - 1];
			Vector3 v2 = verts[f[2] - 1];

			tris.push_back({ v0, v1, v2 });
		}
	}
}
