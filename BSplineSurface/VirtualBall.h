/**************************
算法：    虚拟球旋转算法
作者：    aijiaming
时间：    2017/12/3
联系方式：1907059378@qq.com 
**************************/
#ifndef VIRTUALBALL_H
#define VIRTUALBALL_H
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
const float PI = 3.1415926;
class VirtualBall
{
public:
	VirtualBall()
	{
		//初始旋转设为单位的
		LastRotate = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0),0.0f);
		currentRotate = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0), 0.0f);
	}

	VirtualBall(float width,float height)
		:_width(width),_height(height)
	{
		//初始旋转设为单位的
		LastRotate = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0), 0.0f);
		currentRotate = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0), 0.0f);
	}
	~VirtualBall(){}
	
	void setBounds(float newwidth, float newheight)
	{
		_width = newwidth;
		_height = newheight;
	}
	//将四元数转化为对应的4x4旋转矩阵
	QMatrix4x4 RotateMatrix()
	{
		return quatToMat(currentRotate);
	}
	//将屏幕坐标映射到虚拟球上
	QVector3D mapToSphere(double x, double y)
	{
		//首先将点[0,width],[0,height]映射到[-1,1],[1,-1]上
		x = x*2.0 / _width - 1;
		y = y*(-2.0) / _height + 1;
		double d = x*x + y*y;
		
		if (d <= 1)
		{
			return QVector3D(x, y, sqrt(1 - d));
		}
		else
		{
			return QVector3D(x / sqrt(d), y / sqrt(d), 0);
		}
	}
	//点击时，屏幕坐标映射到球面坐标
	void click(double x,double y)
	{
		_lastpos = mapToSphere(x, y);
	}
	//拖动后，结束点的屏幕坐标映射到球面坐标，并
	//通过前后两个坐标计算旋转轴，旋转角度，生成对应四元数
	void dragTo(double x, double y)
	{
		_newpos = mapToSphere(x, y);

		QVector3D axis = QVector3D::crossProduct(_lastpos, _newpos); //叉积为旋转轴
		axis.normalize();
		float angle = asin((QVector3D::crossProduct(_lastpos, _newpos)).length()); //旋转角度
		angle = angle / PI * 180;
		//Quaternion参数中的角度不是弧度
		QQuaternion temp = QQuaternion::fromAxisAndAngle( axis,2*angle); //生成四元数，其中为了旋转360度，角度为2倍
		currentRotate = temp*LastRotate; //保存到currentRotate中
	}
	void push()
	{
		LastRotate = currentRotate; //一次拖动完成，将currentRotate保存
	}

	inline QMatrix4x4 quatToMat(QQuaternion q)
	{
		//based on algorithm on wikipedia
		// http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
		float w = q.scalar();
		float x = q.x();
		float y = q.y();
		float z = q.z();

		float n = q.lengthSquared();
		float s = n == 0 ? 0 : 2 / n;
		float wx = s * w * x, wy = s * w * y, wz = s * w * z;
		float xx = s * x * x, xy = s * x * y, xz = s * x * z;
		float yy = s * y * y, yz = s * y * z, zz = s * z * z;

		float m[16] = { 1 - (yy + zz),         xy + wz ,         xz - wy ,0,
			xy - wz ,    1 - (xx + zz),         yz + wx ,0,
			xz + wy ,         yz - wx ,    1 - (xx + yy),0,
			0 ,               0 ,               0 ,1 };
		QMatrix4x4 result = QMatrix4x4(m, 4, 4);
		result.optimize();
		return result;
	}

private:
	QVector3D _lastpos; //点击屏幕时，映射到球面的坐标
	QVector3D _newpos;  //拖动后释放时，最后点映射到球面的坐标
	float _width;   
	float _height;
	QQuaternion currentRotate;  //当前旋转 四元数
	QQuaternion LastRotate;  //前一个旋转 四元数
	
	
};

#endif //VIRTUALBALL_H