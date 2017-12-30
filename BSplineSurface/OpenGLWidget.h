#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include "transform3d.h"

class QExposeEvent;
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
	void update();

protected:
	//void exposeEvent(QExposeEvent *ev);

private:
	// OpenGL State Information
	QOpenGLBuffer m_vertex;
	QOpenGLVertexArrayObject m_object;
	QOpenGLShaderProgram *m_program;

	// Shader Information
	int u_modelToWorld;
	int u_worldToView;
	QMatrix4x4 m_projection;
	Transform3D m_transform;

	// Fix for Windows
	QRegion m_cachedRegion;

	// Private Helpers
	void printVersionInformation();
};

#endif // WINDOW_H
