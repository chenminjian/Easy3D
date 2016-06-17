#include "ObjLoader.h"

GameObject* ObjLoader::loadOBJ(const wstring path)
{
	std::vector<unsigned int> vertexIndices,uvIndices;	//���������ͨ������֪����ֵ����Ϊģ���ļ�Ϊ�˹��ö����ʹ������

	std::vector<vec3> temp_vertices;	//�洢���ж�������
	std::vector<vec2> temp_uvs;			//�洢���ж����Ӧ����������

	std::vector<vec3> out_vertices;		//�洢���������ζ�Ӧ�������꣬������3*�����θ���
	std::vector<vec2> out_uvs;			//�洢���������εĶ����Ӧ����������

	FILE* file = _wfopen(path.c_str(),_T("r"));
	if (file == NULL)
	{
		printf("obj loaded unsuccessfully");
		getchar();
		return nullptr;
	}

	while (1)
	{
		char lineHeader[128];

		int res = fscanf(file,"%s",lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader,"v") == 0)	//��������			v 1.000000 -1.000000 -1.000000
		{
			vec3 vertex;
			fscanf(file,"%f %f %f\n",&vertex.x,&vertex.y,&vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader,"vt") == 0)	//������������		vt 0.750412 0.748571
		{
			vec2 uv;
			fscanf(file,"%f %f\n",&uv.u,&uv.v);

			uv.v = -uv.v;	//DDS �Ƿ���. TGA��BMP������ 

			temp_uvs.push_back(uv);

		}
		else if (strcmp(lineHeader,"vn") == 0)	//���㷨��				vn 0.000000 1.000000 -0.000000
		{
			//��ʱ������
		}
		else if (strcmp(lineHeader,"f") == 0)	//����������			f 5/1/1 1/2/1 4/3/1
		{
			unsigned int vertexIndex[3],uvIndex[3],normalIndex[3];		//��������ε�����
			int matches = fscanf(file,"%d/%d/%d %d/%d/%d %d/%d/%d\n",&vertexIndex[0],&uvIndex[0],&normalIndex[0],
				&vertexIndex[1],&uvIndex[1],&normalIndex[1],&vertexIndex[2],&uvIndex[2],&normalIndex[2]);
			if (matches !=9)
			{
				printf("file can't be read by simple parser");
				return nullptr;
			}
			vertexIndices.push_back(vertexIndex[0]);		//�������������ε�����
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
		}
		else			//����
		{
			char stupidBuffer[1000];
			fgets(stupidBuffer,1000,file);
		}
	}

	//ͨ������ȷ������Ķ�������
	for (unsigned int i = 0; i<vertexIndices.size(); i++)	//3*�����θ���
	{
		//��ȡ����ֵ
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];

		//��ȡ��������
		vec3 vertex = temp_vertices[vertexIndex-1];		//OBJ�ļ�������1��ʼ
		vec2 uv = temp_uvs[uvIndex-1];

		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
	}


	//������GameObject
	GameObject* go = new GameObject(out_vertices,out_uvs);
	return go;
}