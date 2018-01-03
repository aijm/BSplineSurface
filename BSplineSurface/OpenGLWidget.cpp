#include "OpenGLWidget.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <iostream>


using namespace std;


/*******************************************************************************
* OpenGL Events
******************************************************************************/

OpenGLWidget::OpenGLWidget(QWidget* parent)
	:QOpenGLWidget(parent)
{
	//设置OpenGL版本
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3); //version 3.3
	this->setFormat(format);
	//m_model.translate(0.0f, 0.0f, -5.0f);
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
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
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

		QVector3D lightDirection = QVector3D(0.0f,0.0f,-1.0f);
		lightDirection.normalize();
		m_program->setUniformValue("u_DiffuseLight", QVector3D(0.8f, 0.8f, 0.8f)); //漫反射
		m_program->setUniformValue("u_LightDirection", lightDirection); //光照方向
		

		
			

		//通过随机数生成随机控制网格
		vector<vector<QVector3D>> controlpoint(6);
		
		for (int i = 0; i < 6; ++i)
		{
			QTime time;

			while (QTime::currentTime() == lastTime)
			{

			}
			time = QTime::currentTime();
			lastTime = time;
			qsrand(time.msec() + time.second() * 1000);
			controlpoint[i].resize(6);
			for (int j = 0; j < 6; ++j)
			{
							
				float z = 1.0*(qrand() % 20 - 10) / 20.0;
				controlpoint[i][j] = QVector3D(-1.0 + i*2.0 / 5, -1.0 + j*2.0 / 5, z);
			}
		}

		
		//节点向量，插值端点
		vector<float> knots_u = { 0.0f, 0.0f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.0f, 1.0f };
		vector<float> knots_v = { 0.0f, 0.0f, 0.0f, 0.0f, 0.33f, 0.66f, 1.0f, 1.0f, 1.0f, 1.0f };

		bspsurf = bspSurface(controlpoint, knots_u, knots_v);


		
		//vbo_vertices,vbo_normals,ebo_face,ebo_edge //通过三角面片绘制
		{
			bspsurf.getbuffer_object(vertices, normals, edge_indices, face_indices, 0.01f);
			
			m_vbo_vertices = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			m_vbo_normals = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			m_ebo_face = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
			m_ebo_edge = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

			// Create Buffer (Do not release until VAO is created)

			m_vbo_vertices->create();
			m_vbo_vertices->bind();
			m_vbo_vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
			m_vbo_vertices->allocate(&(vertices[0]), vertices.size() * sizeof(QVector3D));

			m_vbo_normals->create();
			m_vbo_normals->bind();
			m_vbo_normals->setUsagePattern(QOpenGLBuffer::StaticDraw);
			m_vbo_normals->allocate(&(normals[0]), normals.size() * sizeof(QVector3D));

			m_ebo_face->create();
			m_ebo_face->bind();
			m_ebo_face->setUsagePattern(QOpenGLBuffer::StaticDraw);
			m_ebo_face->allocate(&(face_indices[0]), face_indices.size() * sizeof(unsigned short));

			m_ebo_edge->create();
			m_ebo_edge->bind();
			m_ebo_edge->setUsagePattern(QOpenGLBuffer::StaticDraw);
			m_ebo_edge->allocate(&(edge_indices[0]), edge_indices.size() * sizeof(unsigned short));


		}
		//vao_face
		{
			m_vao_face.create();
			m_vao_face.bind();

			m_vbo_vertices->bind();
			m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
			m_program->enableAttributeArray(0);

			m_vbo_normals->bind();
			m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3, sizeof(QVector3D));
			m_program->enableAttributeArray(1);

			m_ebo_face->bind();

			m_vao_face.release();
			m_ebo_face->release();
		}
			
		//vao_edge
		{
			m_vao_edge.create();
			m_vao_edge.bind();

			m_vbo_vertices->bind();
			m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
			m_program->enableAttributeArray(0);

			m_vbo_normals->bind();
			m_program->setAttributeBuffer(1, GL_FLOAT, 0, 3, sizeof(QVector3D));
			m_program->enableAttributeArray(1);

			m_ebo_edge->bind();

			m_vao_edge.release();
			m_ebo_edge->release();
		}
		
		//cn_vao_edge，控制网格
		{
			m_cn_vao_edge.create();
			m_cn_vao_edge.bind();

			m_cn_vbo_vertices = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
			
			m_cn_ebo_edge = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);


			bspsurf.getcontrolpoint(cn_vertices, cn_edge_indices);

			// Create Buffer (Do not release until VAO is created)

			m_cn_vbo_vertices->create();
			m_cn_vbo_vertices->bind();
			m_cn_vbo_vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
			m_cn_vbo_vertices->allocate(&(cn_vertices[0]), cn_vertices.size() * sizeof(QVector3D));

			m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));
			m_program->enableAttributeArray(0);

			m_cn_ebo_edge->create();
			m_cn_ebo_edge->bind();
			m_cn_ebo_edge->setUsagePattern(QOpenGLBuffer::StaticDraw);
			
			m_cn_ebo_edge->allocate(&(cn_edge_indices[0]), cn_edge_indices.size() * sizeof(unsigned short));

			m_cn_vao_edge.release();
			m_cn_ebo_edge->release();
		}


		// Release (unbind) al
		m_program->release();
	}
}

void OpenGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	vBall.setBounds(width, height);
	
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

	//模型变换后，法向变换对应的矩阵为模型矩阵 逆的转置
	m_normalmatrix = m_model.inverted();
	m_normalmatrix = m_normalmatrix.transposed();

	// Render using our shader
	m_program->bind();
	m_program->setUniformValue(u_model, m_model);
	m_program->setUniformValue(u_view, m_view);
	m_program->setUniformValue(u_projection, m_projection);
	m_program->setUniformValue(u_normal, m_normalmatrix);
	{

		if (showface)
		{
			//绘制B样条曲面
			m_vao_face.bind();
			m_program->setUniformValue("u_Color", QVector3D(1.0, 0.0, 0.0)); //曲面表面颜色
			m_program->setUniformValue("u_AmbientLight", QVector3D(0.3f, 0.3f, 0.3f)); //环境光

			glDrawElements(GL_TRIANGLES, face_indices.size(), GL_UNSIGNED_SHORT, 0);

			m_vao_face.release();
		}
		else
		{
			//绘制B样条，网格线形式

			m_vao_edge.bind();
			m_program->setUniformValue("u_Color", QVector3D(1.0, 0.0, 0.0)); //曲面表面颜色
			m_program->setUniformValue("u_AmbientLight", QVector3D(1.0f, 1.0f, 1.0f)); //环境光

			glDrawElements(GL_LINES, edge_indices.size(), GL_UNSIGNED_SHORT, 0);

			m_vao_edge.release();
		}

		if (showcontrol)
		{
			//绘制控制网格
			m_cn_vao_edge.bind();
			m_program->setUniformValue("u_Color", QVector3D(0.8, 0.8, 0.8)); //曲面表面颜色
			m_program->setUniformValue("u_AmbientLight", QVector3D(1.0f, 1.0f, 1.0f)); //环境光
			glDrawElements(GL_LINES, cn_edge_indices.size(), GL_UNSIGNED_SHORT, 0);
			m_cn_vao_edge.release();
		}
		
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

void OpenGLWidget::on_showface()
{
	showface = true;
	update();
}

void OpenGLWidget::on_showedge()
{
	showface = false;
	update();
}

void OpenGLWidget::on_showcontrol()
{
	showcontrol = !showcontrol;
	update();
}

void OpenGLWidget::teardownGL()
{
	// Actually destroy our OpenGL information


	m_vbo_vertices->destroy();
	m_vbo_normals->destroy();
	m_ebo_edge->destroy();
	m_ebo_face->destroy();
	m_cn_vbo_vertices->destroy();
	m_cn_ebo_edge->destroy();
	m_vao_edge.destroy();
	m_vao_face.destroy();
	m_cn_vao_edge.destroy();
	delete m_program;
}




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