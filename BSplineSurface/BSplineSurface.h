#ifndef BSPLINESURFACE_H

#define BSPLINESURFACE_H




#include <vector>
#include <iostream>
#include <QVector3D>
using namespace std;

class bspSurface
{
public:
	bspSurface(){}
	bspSurface(const vector<vector<QVector3D>>& cnPoint, const vector<float>& knots_u, vector<float>& knots_v)
	{
		m_cnPoint = cnPoint;
		m_knots_u = knots_u;
		m_knots_v = knots_v;

		m_nu = m_cnPoint.size() - 1;
		m_nv = m_cnPoint[0].size() - 1;
		m_ku = m_knots_u.size()-1 - m_nu;
		m_kv = m_knots_v.size()-1 - m_nv;
	}

	bspSurface(const bspSurface& surface)
	{
		m_cnPoint = surface.m_cnPoint;
		m_knots_u = surface.m_knots_u;
		m_knots_v = surface.m_knots_v;

		m_nu = surface.m_nu;
		m_nv = surface.m_nv;
		m_ku = surface.m_ku;
		m_kv = surface.m_kv;
		
	}

	bspSurface& operator=(const bspSurface& surface)
	{
		m_cnPoint = surface.m_cnPoint;
		m_knots_u = surface.m_knots_u;
		m_knots_v = surface.m_knots_v;

		m_nu = surface.m_nu;
		m_nv = surface.m_nv;
		m_ku = surface.m_ku;
		m_kv = surface.m_kv;
		return *this;
	}

	//根据参数u,v计算曲面上点的坐标
	QVector3D calPos(const float& u, const float& v)
	{
		vector<QVector3D> v_constant(m_nu + 1);
		for (int i = 0; i < v_constant.size(); ++i)
		{
			v_constant[i] = calPos(m_cnPoint[i], m_knots_v, v);
		}
		return calPos(v_constant, m_knots_u, u);

	}

	QVector3D calPos(const vector<QVector3D>& controlpoint, const vector<float>& knots, const float& t)
	{
		int n = controlpoint.size() - 1;
		int k = knots.size() - controlpoint.size();
		int L = 0;
		//计算t所处的区间[t_L,t_(L+1)]，t只在[ knots[k-1], knots[n+1] ]中有效
		if (t >= knots[n + 1])
		{
			L = n;
		}
		else if (t <= knots[k - 1])
		{
			L = k - 1;
		}
		else
		{
			for (int i = k - 1; i <= n + 1; ++i)
			{
				if (t >= knots[i] && t<knots[i + 1])
				{
					L = i;
					break;
				}
			}
		}
		
		if (L >= n + 1) L = n;
		


		vector<QVector3D> temp(k);
		for (int i = 0; i<k; ++i)
		{
			temp[i] = controlpoint[i + L - k + 1];
		}

		//de-BoorCox算法
		for (int r = 1; r <= k - 1; ++r)
		{
			for (int i = 0; i <= k - r - 1; ++i)
			{
				int index = L - k + 1 + r;
				double factor = 0;
				if (knots[index + i + k - r] != knots[index + i])
				{
					factor = (t - knots[index + i]) / (knots[index + i + k - r] - knots[index + i]);
				}
				temp[i] = factor*temp[i + 1] + (1 - factor)*temp[i];

			}
		}
		return temp[0];
	}

	//根据精度生成vbo,vao,ebo，用于绘制
	void getbuffer_object(
		vector<QVector3D>& vertices, vector<QVector3D>& normals,
		vector<unsigned short>& edge_indices, vector<unsigned short>& face_indices,float step=0.01f)
	{

		int m = static_cast<int>((m_knots_u[m_nu + 1] - m_knots_u[m_ku - 1]) / step);
		int n = static_cast<int>((m_knots_v[m_nv + 1] - m_knots_v[m_kv - 1]) / step);

		for (int i = 0; i <= m; ++i)
		{
			for (int j = 0; j <= n; ++j)
			{
				float u = 0, v = 0;
				if (i == m)
				{
					u = m_knots_u[m_nu + 1];
					v = m_knots_v[m_kv - 1] + j*step;
					
				}
				else if (j == n)
				{
					u = m_knots_u[m_ku - 1] + i*step;
					v = m_knots_v[m_nv + 1];
					
				}
				else
				{
					u = m_knots_u[m_ku - 1] + i*step;
					v = m_knots_v[m_kv - 1] + j*step;
				}
				
				QVector3D temp = calPos(u, v);
				vertices.push_back(temp);
			}
		}
		normals.resize(vertices.size());
		for (int i = 0; i < m; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				QVector3D vector1 = vertices[i*(n + 1) + j + 1] - vertices[i*(n + 1) + j];
				QVector3D vector2 = vertices[i*(n + 1) + j + (n + 1)] - vertices[i*(n + 1) + j];
				QVector3D temp_normal = QVector3D::crossProduct(vector2, vector1);
				temp_normal.normalize();
				normals[i*(n + 1) + j] = temp_normal;

			}
		}
		for (int i = 0; i <= m; ++i)
		{
			normals[i*(n + 1) + n] = normals[i*(n + 1) + n - 1];
		}
		for (int j = 0; j <= n; ++j)
		{
			normals[m*(n + 1) + j] = normals[(m - 1)*(n + 1) + j];
		}
		
		for (unsigned short i = 0; i < m; ++i)
		{
			for (unsigned short j = 0; j < n; ++j)
			{
				face_indices.push_back(i*(n + 1) + j);
				face_indices.push_back(i*(n + 1) + j + 1);
				face_indices.push_back(i*(n + 1) + j + (n + 1));
				
				face_indices.push_back(i*(n + 1) + j + (n + 1));
                face_indices.push_back(i*(n + 1) + j + 1);
				face_indices.push_back(i*(n + 1) + j + (n + 1) + 1);

				edge_indices.push_back(i*(n + 1) + j);
				edge_indices.push_back(i*(n + 1) + j + 1);
				edge_indices.push_back(i*(n + 1) + j);
				edge_indices.push_back(i*(n + 1) + j + (n + 1));
			
			}
		}

		for (unsigned short i = 0; i < m; ++i)
		{
			edge_indices.push_back(i*(n + 1) + n);
			edge_indices.push_back((i + 1)*(n + 1) + n);
		}
		for (unsigned short j = 0; j < n; ++j)
		{
			edge_indices.push_back(m*(n + 1) + j);
			edge_indices.push_back(m*(n + 1) + j + 1);
		}
	}
	
	//生成控制顶点的顶点及边上点的索引，用于绘制
	void getcontrolpoint(vector<QVector3D>& vertices, vector<unsigned short>& edge_indices)
	{
		for (int i = 0; i <= m_nu; ++i)
		{
			for (int j = 0; j <= m_nv; ++j)
			{
				vertices.push_back(m_cnPoint[i][j]);
			}
		}
		for (unsigned short i = 0; i < m_nu; ++i)
		{
			for (unsigned short j = 0; j < m_nv; ++j)
			{

				edge_indices.push_back(i*(m_nv + 1) + j);
				edge_indices.push_back(i*(m_nv + 1) + j + 1);
				edge_indices.push_back(i*(m_nv + 1) + j);
				edge_indices.push_back(i*(m_nv + 1) + j + (m_nv + 1));

			}
		}

		for (unsigned short i = 0; i < m_nu; ++i)
		{
			edge_indices.push_back(i*(m_nv + 1) + m_nv);
			edge_indices.push_back((i + 1)*(m_nv + 1) + m_nv);
		}
		for (unsigned short j = 0; j < m_nv; ++j)
		{
			edge_indices.push_back(m_nu*(m_nv + 1) + j);
			edge_indices.push_back(m_nu*(m_nv + 1) + j + 1);
		}
	}


private:
	int m_nu; // u向 0-nu
	int m_nv; // v向 0-nv
	int m_ku; //u向阶
	int m_kv; //v向阶
	vector<vector<QVector3D>> m_cnPoint; //控制网格点坐标 (nu+1)x(nv+1)
	vector<float> m_knots_u;//u向节点向量 u_0,...,u_(nu+ku)
	vector<float> m_knots_v;//v向节点向量 v_0,...,v_(nv+kv)
};


#endif // !BSPLINESURFACE_H