#include "OpenGLWidget.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <iostream>
#include <QTime>



using namespace std;


GLfloat vertices1[] = {
	1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, // v0-v1-v2-v3 front
		1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, // v0-v3-v4-v5 right
		1.0, 1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, // v0-v5-v6-v1 up
		-1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, // v1-v6-v7-v2 left
		-1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, // v7-v4-v3-v2 down
		1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0  // v4-v7-v6-v5 back
};



// Normal
GLfloat normals1[] = {
	0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,  // v0-v1-v2-v3 front
		1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,  // v0-v3-v4-v5 right
		0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,  // v0-v5-v6-v1 up
		-1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0,  // v1-v6-v7-v2 left
		0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0,  // v7-v4-v3-v2 down
		0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0   // v4-v7-v6-v5 back
};

GLuint indices1[] = {
	0, 1, 2, 0, 2, 3,    // front
		4, 5, 6, 4, 6, 7,    // right
		8, 9, 10, 8, 10, 11,    // up
		12, 13, 14, 12, 14, 15,    // left
		16, 17, 18, 16, 18, 19,    // down
		20, 21, 22, 20, 22, 23     // back
};

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
	//connect(this, SIGNAL(frameSwapped()), this, SLOT(update())); 
	printVersionInformation();

	// Set global information
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader.vert");
		m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
		m_program->link();
		m_program->bind();

		// Cache Uniform Locations
		u_model = m_program->uniformLocation("u_Model");
		u_view = m_program->uniformLocation("u_View");
		u_projection = m_program->uniformLocation("u_Projection");
		u_normal = m_program->uniformLocation("u_Normal");

		QVector3D lightDirection = QVector3D(0.5f, 3.0f, 4.0f);
		lightDirection.normalize();
		m_program->setUniformValue("u_DiffuseLight", QVector3D(0.5f, 0.5f, 0.5f));
		m_program->setUniformValue("u_LightDirection", lightDirection);
		m_program->setUniformValue("u_AmbientLight", QVector3D(0.2f, 0.2f, 0.2f));

		
			


		vector<vector<QVector3D>> controlpoint(6);
		
		for (int i = 0; i < 6; ++i)
		{
			controlpoint[i].resize(6);
			for (int j = 0; j < 6; ++j)
			{
				QTime time;
				time = QTime::currentTime();
				qsrand(time.msec() + time.second() * 1000);
				float z = 1.0*(qrand() % 20 - 10) / 10.0;
				cout << z << endl;
				controlpoint[i][j] = QVector3D(-1.0 + i*2.0 / 5, -1.0 + j*2.0 / 5, z);
			}
		}

		vector<float> knots_u = { -0.02f, -0.01f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.01f, 1.02f };
		vector<float> knots_v = { -0.03f, -0.02f, -0.01f, 0.0f, 0.33f, 0.66f, 1.0f, 1.01f, 1.02f, 1.03f };
		
		bspsurf = bspSurface(controlpoint, knots_u, knots_v);

		bspsurf.getbuffer_object(vertices, normals, edge_indices, face_indices,0.1f);

		/*cout << "vertices size: " << vertices.size() << endl;
		cout << "normals size: " << normals.size() << endl;
		cout << "edge_indices size: " << edge_indices.size() << endl;
		cout << "face_indices size: " << face_indices.size() << endl;*/

		for (int i = 0; i < face_indices.size(); ++i)
		{
			cout << face_indices[i] << endl;
		}

		m_vao_face.create();
		m_vao_face.bind();
		
		m_vbo_vertices = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		m_vbo_normals = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		m_ebo_face = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		m_ebo_edge = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);


		// Create Buffer (Do not release until VAO is created)
		
		m_vbo_vertices->create();
		m_vbo_vertices->bind();
		m_vbo_vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_vbo_vertices->allocate(&(vertices[0]), vertices.size()*sizeof(QVector3D));

		m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
		m_program->enableAttributeArray(0);
		

		m_vbo_normals->create();
		m_vbo_normals->bind();
		m_vbo_normals->setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_vbo_normals->allocate(&(normals[0]), normals.size() * sizeof(QVector3D));

		m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3, sizeof(QVector3D));
		m_program->enableAttributeArray(1);
		

		m_ebo_face->create();
		m_ebo_face->bind();
		m_ebo_face->setUsagePattern(QOpenGLBuffer::StaticDraw);
		cout << sizeof(unsigned short) << endl;
		cout << sizeof(face_indices) << endl;
		cout << sizeof(int) << endl;
		m_ebo_face->allocate(&(face_indices[0]), face_indices.size() * sizeof(unsigned short));

		m_vao_face.release();
		m_ebo_face->release();

		/*m_cube_v=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		m_cube_n=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		ebo_cube = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		
		vao_cube.create();
		vao_cube.bind();

		m_cube_v->create();
		m_cube_v->bind();
		m_cube_v->allocate(vertices1, sizeof(vertices1));
		m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));
		m_program->enableAttributeArray(0);

		m_cube_n->create();
		m_cube_n->bind();
		m_cube_n->allocate(normals1, sizeof(normals1));
		m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));
		m_program->enableAttributeArray(1);

		ebo_cube->create();
		ebo_cube->bind();
		ebo_cube->allocate(indices1, sizeof(indices1));
		
		vao_cube.release();
		ebo_cube->release();*/



		// Release (unbind) al
		m_program->release();
	}
}

void OpenGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	vBall.setBounds(width, height);
	//m_projection.setToIdentity();
	//m_projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}

void OpenGLWidget::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT);

	m_model.setToIdentity();
	m_view.setToIdentity();
	m_projection.setToIdentity();
	m_normalmatrix.setToIdentity();


	m_model = vBall.RotateMatrix();
	m_view.lookAt(QVector3D(1.0, 1.0, 3.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
	m_projection.perspective(fov, this->width() / (float)(this->height()), 1.0f, 1000.0f);

	m_normalmatrix = m_model.inverted();
	m_normalmatrix = m_normalmatrix.transposed();



	// Render using our shader
	m_program->bind();
	m_program->setUniformValue(u_model, m_model);
	m_program->setUniformValue(u_view, m_view);
	m_program->setUniformValue(u_projection, m_projection);
	m_program->setUniformValue(u_normal, m_normalmatrix);
	{
		m_vao_face.bind();
		//vao_cube.bind();
		//glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
		//glDrawArrays(GL_TRIANGLES, 0, sg_vertexes.size() / 2);
	
		glDrawElements(GL_TRIANGLES, face_indices.size(), GL_UNSIGNED_SHORT, 0);
		//glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0);
		m_vao_face.release();
		//vao_cube.release();
	}
	m_program->release();
}
void OpenGLWidget::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		
		vBall.click(e->pos().x(), e->pos().y());
		leftbuttonpress = true;
	}
}
void OpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (leftbuttonpress)
	{
		vBall.dragTo(e->pos().x(), e->pos().y());
		update();
	}
	
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		vBall.push();
		leftbuttonpress = false;
	}

}

void OpenGLWidget::wheelEvent(QWheelEvent *e)
{
	int offset = e->angleDelta().y() / 15;

	if (fov >= 1.0f&&fov <= 89.0f)
	{
		fov += offset;
	}
	else if (fov <= 1.0f)
	{
		fov = 1.0f;
	}
	else
	{
		fov = 89.0f;
	}
	update();
}

void OpenGLWidget::teardownGL()
{
	// Actually destroy our OpenGL information


	/*m_vbo_vertices->destroy();
	m_vbo_normals->destroy();
	m_ebo_edge->destroy();
	m_ebo_face->destroy();
	m_vao_edge.destroy();
	m_vao_face.destroy();*/
	delete m_program;
}

//void OpenGLWidget::update()
//{
//	// Update instance information
//	m_model.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));
//
//	// Schedule a redraw
//	QOpenGLWidget::update();
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