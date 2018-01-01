#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <VirtualBall.h>
#include <BSplineSurface.h>

class QOpenGLShaderProgram;

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
	//void update();

protected:
	//void exposeEvent(QExposeEvent *ev);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
private:

	bool leftbuttonpress = false;
	float fov = 45.0f;
	VirtualBall vBall;
	// OpenGL State Information
	QOpenGLBuffer* m_vbo_vertices;
	QOpenGLBuffer* m_vbo_normals;
	QOpenGLBuffer* m_ebo_edge;
	QOpenGLBuffer* m_ebo_face;
	QOpenGLVertexArrayObject m_vao_edge;
	QOpenGLVertexArrayObject m_vao_face;
	QOpenGLShaderProgram *m_program;
	

	QOpenGLBuffer* m_cube_v;
	QOpenGLBuffer* m_cube_n;
	QOpenGLBuffer* ebo_cube;
	QOpenGLVertexArrayObject vao_cube;

	bspSurface bspsurf;


	vector<QVector3D> vertices;
	vector<QVector3D> normals;
	vector<unsigned short> edge_indices;
	vector<unsigned short> face_indices;

	// Shader Information
	int u_model;
	int u_view;
	int u_projection;
	int u_normal;
	QMatrix4x4 m_projection;
	QMatrix4x4 m_model;
	QMatrix4x4 m_view;
	QMatrix4x4 m_normalmatrix;

	// Fix for Windows
	QRegion m_cachedRegion;

	// Private Helpers
	void printVersionInformation();
};

#endif // WINDOW_H
