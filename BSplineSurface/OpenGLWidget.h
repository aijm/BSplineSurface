#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <VirtualBall.h>
#include <BSplineSurface.h>
#include <QTime>

class QOpenGLShaderProgram;
class QExposeEvent;

class OpenGLWidget : public QOpenGLWidget,
	protected QOpenGLFunctions
{
	Q_OBJECT

		// OpenGL Events
public:
	OpenGLWidget(QWidget* parent=0);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	protected slots:
	void teardownGL();
	void on_showface();
	void on_showedge();
	void on_showcontrol();
	//void update();

protected:
	
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	
	QTime lastTime;

	bool showcontrol = true;
	bool showface = true;

	bool leftbuttonpress = false;
	float fov = 45.0f;
	VirtualBall vBall;  //ÐéÄâÇò£¬ÓÃÓÚÐý×ª
	// OpenGL State Information
	QOpenGLBuffer* m_vbo_vertices;
	QOpenGLBuffer* m_vbo_normals;
	QOpenGLBuffer* m_ebo_edge;
	QOpenGLBuffer* m_ebo_face;

	QOpenGLBuffer* m_cn_vbo_vertices;
	QOpenGLBuffer* m_cn_ebo_edge;

	QOpenGLVertexArrayObject m_vao_edge;
	QOpenGLVertexArrayObject m_vao_face;
	QOpenGLVertexArrayObject m_cn_vao_edge;


	QOpenGLShaderProgram *m_program;

	bspSurface bspsurf;


	vector<QVector3D> vertices;
	vector<QVector3D> normals;
	vector<unsigned short> edge_indices;
	vector<unsigned short> face_indices;

	vector<QVector3D> cn_vertices;
	vector<unsigned short> cn_edge_indices;
	// Shader Information
	int u_model;
	int u_view;
	int u_projection;
	int u_normal;
	QMatrix4x4 m_projection;
	QMatrix4x4 m_model;
	QMatrix4x4 m_view;
	QMatrix4x4 m_normalmatrix;

	

	// Private Helpers
	void printVersionInformation();
};

#endif // WINDOW_H
