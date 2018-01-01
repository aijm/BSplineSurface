#include "OpenGLWidget.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QExposeEvent>
#include <iostream>

using namespace std;
// Front Verticies
#define VERTEX_FTR QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f )
#define VERTEX_FTL QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f )
#define VERTEX_FBL QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f )
#define VERTEX_FBR QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 0.0f )

// Back Verticies
#define VERTEX_BTR QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) 
#define VERTEX_BTL QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) 
#define VERTEX_BBL QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) 
#define VERTEX_BBR QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f )

// Create a colored cube
static const QVector3D sg_vertexes[] = {
	// Face 1 (Front)
	VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
	VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
	// Face 2 (Back)
	VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
	VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
	// Face 3 (Top)
	VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
	VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
	// Face 4 (Bottom)
	VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
	VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
	// Face 5 (Left)
	VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
	VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
	// Face 6 (Right)
	VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
	VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR

/*******************************************************************************
* OpenGL Events
******************************************************************************/

OpenGLWidget::OpenGLWidget(QWidget* parent)
	:QOpenGLWidget(parent)
{
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3); //version 3.3
	this->setFormat(format);
	m_model.translate(0.0f, 0.0f, -5.0f);
}

void OpenGLWidget::initializeGL()
{
	// Initialize OpenGL Backend
	initializeOpenGLFunctions();
	connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
	//当缓冲区交换后
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update())); 
	printVersionInformation();

	// Set global information
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
		m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
		m_program->link();
		m_program->bind();

		// Cache Uniform Locations
		u_model = m_program->uniformLocation("model");
		u_view = m_program->uniformLocation("view");
		u_projection = m_program->uniformLocation("projection");

		// Create Buffer (Do not release until VAO is created)
		m_vertex.create();
		m_vertex.bind();
		m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

		// Create Vertex Array Object
		m_object.create();
		m_object.bind();
		m_program->enableAttributeArray(0);
		m_program->enableAttributeArray(1);
		m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 2*sizeof(QVector3D)); 
		m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(QVector3D), 3, 2 * sizeof(QVector3D));

		// Release (unbind) all
		m_object.release();
		m_vertex.release();
		m_program->release();
	}
}

void OpenGLWidget::resizeGL(int width, int height)
{
	//glViewport(0, 0, width, height);
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}

void OpenGLWidget::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT);


	// Render using our shader
	m_program->bind();
	m_program->setUniformValue(u_model, m_model);
	m_program->setUniformValue(u_view, m_view);
	m_program->setUniformValue(u_projection, m_projection);
	{
		m_object.bind();
		//cout << sizeof(sg_vertexes) << endl;
		//cout << sizeof(sg_vertexes[0]) << endl;
		glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
		m_object.release();
	}
	m_program->release();
}

void OpenGLWidget::teardownGL()
{
	// Actually destroy our OpenGL information
	m_object.destroy();
	m_vertex.destroy();
	delete m_program;
}

void OpenGLWidget::update()
{
	// Update instance information
	m_model.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

	// Schedule a redraw
	QOpenGLWidget::update();
}

//void OpenGLWidget::exposeEvent(QExposeEvent *ev)
//{
//	if (ev->region() != m_cachedRegion)
//	{
//		m_cachedRegion = ev->region();
//		QOpenGLWidget::exposeEvent(ev);
//	}
//	else
//	{
//		ev->ignore();
//	}
//
//}

/*******************************************************************************
* Private Helpers
******************************************************************************/

void OpenGLWidget::printVersionInformation()
{
	QString glType;
	QString glVersion;
	QString glProfile;

	// Get Version Information
	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
	switch (format().profile())
	{
		CASE(NoProfile);
		CASE(CoreProfile);
		CASE(CompatibilityProfile);
	}
#undef CASE

	// qPrintable() will print our QString w/o quotes around it.
	qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}