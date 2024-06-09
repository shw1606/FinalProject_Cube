#pragma once
#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif
struct instruction {
	int x, y, dir;
	instruction() {
		x = 0;
		y = 0;
		dir = 0;
	}
	instruction(int x, int y, int dir)
	{
		this->x = x;
		this->y = y;
		this->dir = dir;
	}
	instruction(instruction& ins)
	{
		this->x = ins.x;
		this->y = ins.y;
		this->dir = ins.dir;
	}
};
class MagicCube
{
private:
	struct unit {
		int x, y, z;
		float* vertice;
		glm::mat4 model;
		unit() {
			model = glm::mat4(1.0f);
		}
		void rotate(glm::mat4& trans)
		{
			for (int i = 0; i < 36; i++)
			{
				glm::vec4 temp(vertice[i * 8], vertice[i * 8 + 1], vertice[i * 8 + 2],1.0f);
				temp = trans * temp;
				vertice[i * 8] = temp.x;
				vertice[i * 8+1] = temp.y;
				vertice[i * 8+2] = temp.z;
			}
		}
	};
	
public:
	bool cubeMoving=false;
	instruction lastIns;
	int count = 0;
	int front;
	unit units[27];
	float vertices[288 * 27] = {};
	int next()
	{
		return 0;
	}
	void rotate(instruction& ins)
	{
		if ((!cubeMoving)&&(ins.dir == 8))
			return;
		if (!cubeMoving)
		{
			cubeMoving = true;
			lastIns = instruction(ins);
			ins.dir = 8;
		}
		++count;
		glm::mat4 trans, trans_final;
		int offset, offset_cmp;
		switch (lastIns.dir)
		{
		case 0:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			offset = 0;
			offset_cmp = lastIns.x;
			break;
		case 1:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			offset = 0;
			offset_cmp = lastIns.x;
			break;
		case 2:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			offset = 1;
			offset_cmp = lastIns.y;
			break;
		case 3:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			offset = 1;
			offset_cmp = lastIns.y;
			break;
		case 4:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			break;
		case 5:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 6:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			break;
		case 7:
			trans = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			trans_final = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		}
		if (lastIns.dir < 4)
		{
			for (int i = 0; i < 27; i++)
				if (*((int*)&(units[i]) + offset) == offset_cmp)
				{
					units[i].rotate(trans);
					if (count == 90)
					{
						glm::vec4 temp(units[i].x - 1, units[i].y - 1, 1 - units[i].z, 1.0f);
						temp = trans_final * temp;
						units[i].x = (int)(round(temp.x)) + 1;
						units[i].y = (int)(round(temp.y)) + 1;
						units[i].z = 1 - (int)(round(temp.z));
						temp = temp * 1.0f;
					}
				}
		}
		else
		{
			for (int i = 0; i < 27; i++)
			{
				units[i].rotate(trans);
				if (count == 90)
				{
					glm::vec4 temp(units[i].x - 1, units[i].y - 1, 1 - units[i].z, 1.0f);
					temp = trans_final * temp;
					units[i].x = (int)(round(temp.x)) + 1;
					units[i].y = (int)(round(temp.y)) + 1;
					units[i].z = 1 - (int)(round(temp.z));
					temp = temp * 1.0f;
				}
			}
		}

		if (count == 90)
		{
			count = 0;
			cubeMoving = false;
		}
	}
	MagicCube()
	{
		float core[288]= {
		//x y z texCoordx texCoordy normalx normaly normalz
			-0.495f,  0.495f, -0.495f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //0 윗면
			0.495f,  0.495f, -0.495f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
			0.495f,  0.495f,  0.495f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
			0.495f,  0.495f,  0.495f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
			-0.495f,  0.495f,  0.495f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
			-0.495f,  0.495f, -0.495f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 

			-0.495f, -0.495f,  0.495f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, //1 앞면
			0.495f, -0.495f,  0.495f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 
			0.495f,  0.495f,  0.495f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 
			0.495f,  0.495f,  0.495f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 
			-0.495f,  0.495f,  0.495f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 
			-0.495f, -0.495f,  0.495f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 

			0.495f,  0.495f,  0.495f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, //2 오른쪽 면
			0.495f,  0.495f, -0.495f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 
			0.495f, -0.495f, -0.495f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 
			0.495f, -0.495f, -0.495f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 
			0.495f, -0.495f,  0.495f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
			0.495f,  0.495f,  0.495f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 

			-0.495f, -0.495f, -0.495f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //3 뒷면
			0.495f, -0.495f, -0.495f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
			0.495f,  0.495f, -0.495f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
			0.495f,  0.495f, -0.495f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
			-0.495f,  0.495f, -0.495f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
			-0.495f, -0.495f, -0.495f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 

			-0.495f,  0.495f,  0.495f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, //4 왼쪽 면
			-0.495f,  0.495f, -0.495f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  
			-0.495f, -0.495f, -0.495f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,  
			-0.495f, -0.495f, -0.495f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,  
			-0.495f, -0.495f,  0.495f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  
			-0.495f,  0.495f,  0.495f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 

			-0.495f, -0.495f, -0.495f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, //5 아랫면
			0.495f, -0.495f, -0.495f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 
			0.495f, -0.495f,  0.495f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
			0.495f, -0.495f,  0.495f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
			-0.495f, -0.495f,  0.495f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
			-0.495f, -0.495f, -0.495f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 
		};
		/*
		     ______________
		    /             /
           /		     /      0
		  /             /       3
		 /_____________/  4          2 
		 |            |
		 |            |       1
		 y0           |         5
		 | z0         |
		 |___x0_______|
		*/
		for (int x = 0; x < 3; x++)
			for (int y = 0; y < 3; y++)
				for (int z = 0; z < 3; z++)
				{
					int index = (x * 9 + y * 3 + z) * 288;
					units[index / 288].x = x;
					units[index / 288].y = y;
					units[index / 288].z = z;
					
					for (int v0 = 0; v0 < 288; v0++)
					{
						int gem = v0 / 48;
						if (v0 % 8 != 4)
							vertices[index + v0] = core[v0];
						if (v0 % 8 == 0)
							vertices[index + v0] += x - 1;
						else if(v0 % 8 == 1)
							vertices[index + v0] += y - 1;
						else if (v0 % 8 == 2)
							vertices[index + v0] += 1 - z;
						else if (v0 % 8 == 3)
						{
							vertices[index + v0+1] = core[v0+1];
							switch (gem) {
							case 0:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + x / 3.0)/4.0;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + z / 3.0)/4.0;
								break;
							case 1:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + x / 3.0)/4.0;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + y / 3.0)/4.0 + 0.25;
								break;
							case 2:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + y / 3.0)/4.0;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + z / 3.0)/4.0 + 0.5;
								break;
							case 3:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + x / 3.0)/4.0;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + y / 3.0)/4.0 + 0.75;
								break;
							case 4:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + y / 3.0)/4.0 + 0.25;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + z / 3.0)/4.0;
								break;
							case 5:
								vertices[index + v0] = (vertices[index + v0] / 3.0 + x / 3.0)/4.0+0.25;
								vertices[index + v0 + 1] = (vertices[index + v0 + 1] / 3.0 + z / 3.0)/4.0+0.25;
								break;

							}
						}
					}
				}
		for (int i = 0; i < 27; i++)
			units[i].vertice = vertices + i * 288;
	}
};