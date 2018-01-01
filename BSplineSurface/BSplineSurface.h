#include <vector>
#include <QVector3D>
using namespace std;

class bspSurface
{
	bspSurface(const vector<vector<QVector3D>>& cnPoint, const vector<float>& knots_u, vector<float>& knots_v)
	{
		m_cnPoint = cnPoint;
		m_knots_u = knots_u;
		m_knots_v = knots_v;

		m_nu = m_cnPoint.size() - 1;
		m_nv = m_cnPoint[0].size() - 1;
		m_ku = m_knots_u.size() - m_nu;
		m_kv = m_knots_v.size() - m_nv;
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
	}

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
		int k = knots.size() - knots.size();
		int L = 0;
		//计算t所处的区间[t_L,t_(L+1)]
		for (int i = k - 1; i <= n + 1; ++i)
		{
			if (t >= knots[i] && t<knots[i + 1])
			{
				L = i;
				break;
			}
		}
		if (L == n + 1) L = n;


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

	//根据精度生成vbo,vao,ebo


private:
	int m_nu; // u向 0-nu
	int m_nv; // v向 0-nv
	int m_ku; //u向阶
	int m_kv; //v向阶
	vector<vector<QVector3D>> m_cnPoint; //控制网格点坐标 (nu+1)x(nv+1)
	vector<float> m_knots_u;//u向节点向量 u_0,...,u_(nu+ku)
	vector<float> m_knots_v;//v向节点向量 v_0,...,v_(nv+kv)
};