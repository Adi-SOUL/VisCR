#include "includes/gl_widget.h"
#include <fstream>
#include <iostream>
#include <QtMath>
#include <QDebug>
#include <QOpenGLFramebufferObject>
#define _CRT_SECURE_NO_WARNINGS
constexpr auto PI = 3.14159265;

static QVector<float> get_with_norm() {
	QVector<float> res;

	for (int i = 0; i < my_vertices.size(); i += (3 * 5)) {
		QVector3D v1(my_vertices[i], my_vertices[i + 1], my_vertices[i + 2]);
		QVector3D v2(my_vertices[i + 5], my_vertices[i + 6], my_vertices[i + 7]);
		QVector3D v3(my_vertices[i + 10], my_vertices[i + 11], my_vertices[i + 12]);

		QVector3D edge1 = v2 - v1;
		QVector3D edge2 = v3 - v1;
		QVector3D norm = QVector3D::crossProduct(edge1, edge2).normalized();

		res << my_vertices[i] << my_vertices[i + 1] << my_vertices[i + 2] << my_vertices[i + 3] << my_vertices[i + 4] << norm[0] << norm[1] << norm[2] <<
			my_vertices[i + 5] << my_vertices[i + 6] << my_vertices[i + 7] << my_vertices[i + 8] << my_vertices[i + 9] << norm[0] << norm[1] << norm[2] <<
			my_vertices[i + 10] << my_vertices[i + 11] << my_vertices[i + 12] << my_vertices[i + 13] << my_vertices[i + 14] << norm[0] << norm[1] << norm[2];

	}
	qDebug() << res.size();
	return res;
}

static bool isZeroMatrix(const QMatrix4x4& matrix) {
	const float epsilon = 1e-6f;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (std::abs(matrix(row, col)) > epsilon)
				return false;
		}
	}
	return true;
}

Widget::Widget(QWidget* parent)
	: QOpenGLWidget(parent)
	, camera(this)
	, VBO(QOpenGLBuffer::VertexBuffer)
	, texture(QOpenGLTexture::Target2D)
	, texture1(QOpenGLTexture::Target2D)
	, texture2(QOpenGLTexture::Target2D)
	, texture_black(QOpenGLTexture::Target2D)
	, texture1_white(QOpenGLTexture::Target2D) {

	setFormat(QSurfaceFormat::defaultFormat());

	this->setWindowIcon(QIcon(":/logo.png"));

	connect(&timer, &QTimer::timeout, this, static_cast<void (Widget::*)()>(&Widget::update));
	timer.start();

	vertices = get_with_norm();
}

Widget::~Widget() {
	makeCurrent();
	texture.destroy();
	texture1.destroy();
	texture1_white.destroy();
	texture_black.destroy();
	doneCurrent();
}

void Widget::initializeGL() {
	fpsTimer.start();
	this->initializeOpenGLFunctions();
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/triangle.vert")) {    //添加并编译顶点着色器
		qDebug() << "ERROR:" << shaderProgram.log();
	}
	if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/triangle.frag")) {  //添加并编译片段着色器
		qDebug() << "ERROR:" << shaderProgram.log();
	}
	if (!shaderProgram.link()) {
		qDebug() << "ERROR:" << shaderProgram.log();
	}

	QOpenGLVertexArrayObject::Binder{ &VAO };



	texture.create();
	texture.setData(QImage(t).mirrored());
	texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	texture1.create();
	texture1.setData(QImage(t1).mirrored());
	texture1.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture1.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture1.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	texture2.create();
	texture2.setData(QImage(t2).mirrored());
	texture2.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture2.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture2.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	texture_black.create();
	texture_black.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture_black.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture_black.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	texture_black.setData(QImage(tb).mirrored());

	texture1_white.create();
	texture1_white.setData(QImage(tw).mirrored());
	texture1_white.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture1_white.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture1_white.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

	VBO.create();
	VBO.bind();
	VBO.allocate(vertices.data(), int(sizeof(float) * vertices.size()));

	shaderProgram.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, sizeof(GLfloat) * 8);
	shaderProgram.enableAttributeArray("aPos");
	shaderProgram.setAttributeBuffer("aTexCoord", GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 8);
	shaderProgram.enableAttributeArray("aTexCoord");
	shaderProgram.setAttributeBuffer("aNorm", GL_FLOAT, sizeof(GLfloat) * 5, 3, sizeof(GLfloat) * 8);
	shaderProgram.enableAttributeArray("aNorm");

	this->glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	camera.init();
	load_json_file();

	disk_thickness = type_to_size[QString("section_1")][2];
	base_thickness = type_to_size[QString("base")][2];
	tip_thickness = type_to_size[QString("tip")][2];
	// total_length = 324.f;
	continuum_length = total_length - type_to_size[QString("base")][2] - type_to_size[QString("tip")][2];
	section_length = -1.f;
	interval = -1.f;

	if (section_nums_rec[QString("section_2")] != -1) {
		section_length = continuum_length / 2;
		interval = continuum_length / (section_nums_rec[QString("section_2")] + section_nums_rec[QString("section_1")]);
		total_disk_num = section_nums_rec[QString("section_2")] + section_nums_rec[QString("section_1")];
		T_ts_all = {
			QVector3D(type_to_size[QString("section_2")][0] / 2 * .9, 0, 0) / 50.f,
			QVector3D(0, 0, type_to_size[QString("section_2")][1] / 2 * .9) / 50.f,
			QVector3D(-type_to_size[QString("section_2")][0] / 2 * .9, 0, 0) / 50.f,
			QVector3D(0, 0, -type_to_size[QString("section_2")][1] / 2 * .9) / 50.f
		};
	}
	else {
		section_length = continuum_length;
		interval = continuum_length / section_nums_rec[QString("section_2")];
		total_disk_num = section_nums_rec[QString("section_1")];
		T_ts_all = {
			QVector3D(type_to_size[QString("section_1")][0] / 2 * .9, 0, 0) / 50.f,
			QVector3D(0, 0, type_to_size[QString("section_1")][1] / 2 * .9) / 50.f,
			QVector3D(-type_to_size[QString("section_1")][0] / 2 * .9, 0, 0) / 50.f,
			QVector3D(0, 0, -type_to_size[QString("section_1")][1] / 2 * .9) / 50.f
		};
	}

	float dx = type_to_size[QString("section_1")][0] / 2 * .9 * sqrt(2) / 2;
	float dy = type_to_size[QString("section_1")][1] / 2 * .9 * sqrt(2) / 2;
	T_ts_1 = {
			QVector3D(dx, 0, dx) / 50.f,
			QVector3D(dy, 0, dy) / 50.f,
			QVector3D(-dx, 0, -dx) / 50.f,
			QVector3D(-dy, 0, -dy) / 50.f
	};

	first_interval = interval - disk_thickness + base_thickness;

	QMatrix4x4 matrix;
	matrix.translate(QVector3D(0.f, base_thickness, 0.f) / 50.f);
	init_T_sequence.push_back(matrix);

	float sensor_interval = continuum_length / (sensor_num - 1);
	for (int i = 1; i < sensor_num; i++) {
		// QMatrix4x4 temp_matrix;
		matrix.translate(QVector3D(0.f, sensor_interval, 0.f) / 50.f);
		init_T_sequence.push_back(matrix);
	}
	QMatrix4x4 zeroMatrix;
	zeroMatrix.fill(0.0f);
	zeroMatrix(0, 0) = 0.f;
	zeroMatrix(0, 1) = 20.f;
	zeroMatrix(0, 2) = 0.f;
	init_T_sequence.push_back(zeroMatrix);

	int N = total_disk_num + 1;  // 要绘制的 disk 数量（包括首尾）
	for (int j = 0; j < N; ++j) {
		// 映射到 [0, sensor_num - 1] 区间
		int index = static_cast<int>(std::round((sensor_num - 1) * (float(j) / (N - 1))));
		init_selectedIndices.append(index);
	}

	load_stl_files();
}

bool Widget::load_json_file() {
	QFile file(json_file);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString jsonData = file.readAll();
	file.close();
	if (jsonData.isNull()) {
		exit(12);
	}
	QJsonParseError errorPtr;
	QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8(), &errorPtr);
	if (doc.isNull()) {
		exit(11);
	}
	QJsonObject root = doc.object();
	QJsonObject filesObj = root.value(QString("files")).toObject();
	for (const QString& key : filesObj.keys()) {
		QJsonObject obj = filesObj[key].toObject();
		// 获取文件路径

		QString filePath = obj.value(QString("file_path")).toString();
		// this->setWindowTitle("file_path");
		type_to_file_name[key] = filePath;

		// 获取尺寸
		QJsonObject sizeObj = obj.value("size").toObject();
		float x = sizeObj.value("x").toString().remove("f").toFloat();
		float y = sizeObj.value("y").toString().remove("f").toFloat();
		float z = sizeObj.value("z").toString().remove("f").toFloat();

		type_to_size[key] = QVector<float>{ x, y, z };
	}
	QJsonObject filesObj_2 = root.value(QString("parameters")).toObject();
	total_length = filesObj_2.value(QString("total_length")).toString().remove("f").toFloat();

	section_nums_rec[QString("section_1")] = filesObj_2.value(QString("disk_number_section_1")).toString().toInt();
	section_nums_rec[QString("section_2")] = filesObj_2.value(QString("disk_number_section_2")).toString().toInt();

	return true;
}

void Widget::resizeGL(int w, int h) {
	this->glViewport(0, 0, w, h);
}

void Widget::paintGL() {
	this->glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderProgram.bind();
	shaderProgram.setUniformValue("view", camera.getView());
	QMatrix4x4 projection;
	projection.perspective(45.0f, width() / (float)height(), 0.1f, 100.0f);
	shaderProgram.setUniformValue("projection", projection);

	QOpenGLVertexArrayObject::Binder{ &VAO };


	wtransforms = get_t();
	if (wtransforms.size() == 0) {
		start = false;
		wtransforms = init_T_sequence;
		sensor_num = 100;
		selectedIndices = init_selectedIndices;
	}
	else {
		start = true;
		sensor_num = wtransforms.size() - 1;
		selectedIndices.clear();
		// Step 1: 计算均匀分布的 sensor 索引
		int N = total_disk_num + 1;  // 要绘制的 disk 数量（包括首尾）
		for (int j = 0; j < N; ++j) {
			// 映射到 [0, sensor_num - 1] 区间
			int index = static_cast<int>(std::round((sensor_num - 1) * (float(j) / (N - 1))));
			selectedIndices.append(index);
		}
	}

	if (has_stl_file) {
		draw_stl_file();
	}

	draw_floor();
	frameCount++;
	if (fpsTimer.elapsed() >= 1000) { // 每秒统计一次
		currentFPS = frameCount * 1000.0f / fpsTimer.elapsed();
		this->setWindowTitle(QString::number(currentFPS));
		frameCount = 0;
		fpsTimer.restart();
	}
	// this->setWindowTitle(type_to_file_name[QString("base")] + "123");
}

void Widget::clear_positions() {
	positions.clear();
	sub_positions_1.clear();
}

void Widget::set_bg(QVector4D c) {
	background_color = c;
}

void Widget::set_disk_num(int num) {
	if (num > 2) {
		if (num < 10) {
			disk_num = num;
		}
	}
}

void Widget::set_tendon_num(int num) {
	if (num == 3) tendon_num = 3;
	if (num == 4) tendon_num = 4;
}

void Widget::set_section_num(int num) {
	if (num == 1) section_num = 1;
	if (num == 2) section_num = 2;
	//if(num==3) section_num = 3;
}

void Widget::set_texture(QString s) {
	texture.destroy();
	texture.setData(QImage(s).mirrored());
	texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	t = s;
}

void Widget::set_texture_1(QString s) {
	texture1.destroy();
	texture1.setData(QImage(s).mirrored());
	texture1.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture1.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture1.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	t1 = s;
}

void Widget::set_texture_2(QString s) {
	texture2.destroy();
	texture2.setData(QImage(s).mirrored());
	texture2.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture2.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture2.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	t2 = s;
}

void Widget::set_texture_b(QString s) {
	texture_black.destroy();
	texture_black.setData(QImage(s).mirrored());
	texture_black.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture_black.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture_black.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	tb = s;
}

void Widget::set_texture_w(QString s) {
	texture1_white.destroy();
	texture1_white.create();
	texture1_white.setData(QImage(s).mirrored());
	texture1_white.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	texture1_white.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	texture1_white.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	tw = s;
}

void Widget::load_stl_files() {
	for (auto it = type_to_file_name.constBegin(); it != type_to_file_name.constEnd(); ++it) {
		std::pair<QString, QString> this_pair{ it.key(), it.value() };
		load_single_stl_file(this_pair);
	}
	start_point_of_arrow = 12;
	start_point_of_base = start_point_of_arrow + type_to_count[QString("arrow")] * 3;
	start_point_of_section_1 = start_point_of_base + type_to_count[QString("base")] * 3;
	start_point_of_section_2 = start_point_of_section_1 + type_to_count[QString("section_1")] * 3;
	start_point_of_tendon = start_point_of_section_2 + type_to_count[QString("section_2")] * 3;
	start_point_of_tip = start_point_of_tendon + type_to_count[QString("tendon")] * 3;
	start_point_of_lines = start_point_of_tip + type_to_count[QString("tip")] * 3;
	vertices_after_stl_loaded = vertices;
}

void Widget::load_single_stl_file(std::pair<QString, QString> this_pair) {
	std::string filename = std::get<1>(this_pair).toStdString();
	QString type = std::get<0>(this_pair);
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		throw std::runtime_error("无法打开文件");
	}

	file.ignore(80);
	file.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));

	std::vector<MyTriangle> triangles(triangleCount);
	for (uint32_t i = 0; i < triangleCount; ++i) {
		file.read(reinterpret_cast<char*>(&triangles[i].normal), sizeof(MyVec3));
		for (int j = 0; j < 3; ++j)
			file.read(reinterpret_cast<char*>(&triangles[i].vertices[j]), sizeof(MyVec3));
		uint16_t dummy;
		file.read(reinterpret_cast<char*>(&dummy), sizeof(uint16_t)); // attribute byte count
	}

	MyVec3 minPt = { std::numeric_limits<float>::infinity(),
					 std::numeric_limits<float>::infinity(),
					 std::numeric_limits<float>::infinity() };

	MyVec3 maxPt = { -std::numeric_limits<float>::infinity(),
					 -std::numeric_limits<float>::infinity(),
					 -std::numeric_limits<float>::infinity() };

	for (const auto& tri : triangles) {
		for (const MyVec3& v : tri.vertices) {
			minPt.x = (std::min)(minPt.x, v.x);
			minPt.y = (std::min)(minPt.y, v.y);
			minPt.z = (std::min)(minPt.z, v.z);

			maxPt.x = (std::max)(maxPt.x, v.x);
			maxPt.y = (std::max)(maxPt.y, v.y);
			maxPt.z = (std::max)(maxPt.z, v.z);
		}
	}
	stl_center = QVector3D(minPt.x + maxPt.x, minPt.z + maxPt.z, minPt.y + maxPt.y) * 0.5f;

	MyVec3 size{ maxPt.x - minPt.x, maxPt.y - minPt.y, maxPt.z - minPt.z };
	const float epsilon = 1e-6f;
	if (size.x < epsilon) size.x = epsilon;
	if (size.y < epsilon) size.y = epsilon;
	if (size.z < epsilon) size.z = epsilon;

	auto model_size = type_to_size[type];
	for (const auto& tri : triangles) {
		MyVec3 norm = tri.normal;
		for (const MyVec3& v : tri.vertices) {
			float nx = ((v.x - minPt.x) / size.x - .5f) * model_size[0] / 50.f;
			float nz = ((v.y - minPt.y) / size.y - .5f) * model_size[1] / 50.f;
			float ny = ((v.z - minPt.z) / size.z) * model_size[2] / 50.f;
			vertices << nx << ny << nz
				<< 1.f << 1.f
				<< norm.x << norm.y << norm.z;
			std::cout << nx << ny << nz << std::endl;
		}
	}

	VBO.bind();
	int _ = sizeof(float) * vertices.size();
	VBO.allocate(vertices.data(), _);

	type_to_count[type] = triangleCount;

	this->has_stl_file = true;
}

void Widget::draw_stl() {
	if (!has_stl_file) return;

	shaderProgram.setUniformValue("s", 1);
	QMatrix4x4 I;
	QMatrix4x4 scale_tendon; scale_tendon.scale(1.f);
	QMatrix4x4 scale_backbone; scale_backbone.scale(2.f, 1.f, 2.f);

	uint32_t tendon_count = type_to_count["tendon"];
	uint32_t half_tendon_count = tendon_count / 2;
	uint32_t section_1_count = type_to_count["section_1"];
	uint32_t section_2_count = type_to_count["section_2"];

	VAO.bind();
	instanceVBO.bind();

	// 写入所有 instance matrix
	instanceVBO.write(0, wtransforms.constData(), sizeof(QMatrix4x4) * (sensor_num - 1));

	// === 渲染骨架 ===
	texture2.bind(2);
	shaderProgram.setUniformValue("ourTexture", 2);
	glDrawArraysInstanced(GL_TRIANGLES, start_point_of_tendon, half_tendon_count * 3, sensor_num - 1);

	// === 渲染腱（4条）===
	texture1.bind(1);
	shaderProgram.setUniformValue("ourTexture", 1);

	QVector<QMatrix4x4> tendon_instances;
	for (int i = 0; i < wtransforms.size() - 1; ++i) {
		for (const auto& offset : T_ts_all) {
			QMatrix4x4 tendon_model = wtransforms[i];
			tendon_model.translate(offset);
			tendon_instances.push_back(tendon_model);
		}
	}
	instanceVBO.write(0, tendon_instances.data(), sizeof(QMatrix4x4) * tendon_instances.size());
	glDrawArraysInstanced(GL_TRIANGLES, start_point_of_tendon, half_tendon_count * 3, tendon_instances.size());

	// === 渲染中段 disks ===
	texture.bind(0);
	shaderProgram.setUniformValue("ourTexture", 0);

	QVector<QMatrix4x4> disk_instances;
	const bool has_section2 = section_nums_rec.contains("section_2") && section_nums_rec["section_2"] != -1;
	int mid_start = 1, mid_end = selectedIndices.size() - 1;
	for (int k = mid_start; k < mid_end; ++k) {
		int i = selectedIndices[k];
		disk_instances.push_back(wtransforms[i]);
	}
	instanceVBO.write(0, disk_instances.data(), sizeof(QMatrix4x4) * disk_instances.size());
	if (has_section2) {
		glDrawArraysInstanced(GL_TRIANGLES, start_point_of_section_1, section_1_count * 3, disk_instances.size() / 2);
		glDrawArraysInstanced(GL_TRIANGLES, start_point_of_section_2, section_2_count * 3, disk_instances.size() / 2);
	}
	else {
		glDrawArraysInstanced(GL_TRIANGLES, start_point_of_section_1, section_1_count * 3, disk_instances.size());
	}

	// === 渲染 Base 和 Tip ===
	shaderProgram.setUniformValue("model_scale", I);
	glDrawArrays(GL_TRIANGLES, start_point_of_base, type_to_count["base"] * 3);

	shaderProgram.setUniformValue("model_scale", wtransforms.last());
	glDrawArrays(GL_TRIANGLES, start_point_of_tip, type_to_count["tip"] * 3);

	// === 末端位置 top_position 更新 ===
	QMatrix4x4 tip_transform = wtransforms.last();
	tip_transform.translate(QVector3D(0.f, type_to_size["tip"][2], 0.f) / 50.f);
	top_position = QVector3D(tip_transform(0, 3), tip_transform(1, 3), tip_transform(2, 3));

	// === 末端力 ===
	if (!isZeroMatrix(wtransforms.last())) {
		QVector3D force = QVector3D(wtransforms.last()(0, 0), wtransforms.last()(0, 2), wtransforms.last()(0, 1)) / 50.f;
		draw_tip_force(top_position, top_position + force);
	}

	// === 绘制轨迹 ===
	if (start) {
		positions.append(tip_transform);
		QVector<float> latest_vertex_data;
		latest_vertex_data << tip_transform(0, 3) << tip_transform(1, 3) << tip_transform(2, 3)
			<< 1.f << 1.f << 0.f << 1.f << 0.f;
		vertices += latest_vertex_data;

		VBO.bind();
		VBO.write(vertices.size() * sizeof(float) - latest_vertex_data.size() * sizeof(float),
			latest_vertex_data.constData(),
			latest_vertex_data.size() * sizeof(float));
	}

	if (true) {
		shaderProgram.setUniformValue("model", I);
		shaderProgram.setUniformValue("scale", I);
		glLineWidth(.1f * line_width_factor);
		shaderProgram.setUniformValue("color", color_of_position_1);
		glDrawArrays(GL_LINE_STRIP, start_point_of_lines, positions.size());
	}

	if (show_base_frame) {
		draw_base_frame();
	}
}

void Widget::draw_stl_file() {
	if (!has_stl_file) {
		return;
	}
	else {
		shaderProgram.setUniformValue("s", 1);

		QMatrix4x4 I;
		QMatrix4x4 scale_tendon; scale_tendon.scale(1.5f);
		QMatrix4x4 scale_backbone; scale_backbone.scale(2.f, 1.5f, 2.f);

		uint32_t tendon_count = type_to_count["tendon"];
		uint32_t helf_tendon_count = tendon_count / 2;
		uint32_t quarter_tendon_count = tendon_count / 4;
		uint32_t section_1_count = type_to_count["section_1"];
		uint32_t section_2_count = type_to_count["section_2"];
		uint32_t arrow_count = type_to_count["arrow"];

		texture2.bind(2);  // ❗仅绑定一次
		texture1.bind(1);  // ❗只在需要时绑定
		// === 渲染骨架 + 腱 ===
		for (int i = 0; i < sensor_num - 1; ++i) {
			shaderProgram.setUniformValue("ourTexture", 2);
			auto model_scale = wtransforms[i] * scale_backbone;
			shaderProgram.setUniformValue("model_scale", model_scale);
			this->glDrawArrays(GL_TRIANGLES, start_point_of_tendon, helf_tendon_count * 3);


			shaderProgram.setUniformValue("ourTexture", 1);

			for (int j = 0; j < 4; ++j) {
				QMatrix4x4 T = QMatrix4x4(); T.translate(T_ts_all[j]);
				shaderProgram.setUniformValue("model_scale", wtransforms[i] * T * scale_tendon);
				this->glDrawArrays(GL_TRIANGLES, start_point_of_tendon, helf_tendon_count * 3);
			}

			if (section_nums_rec["section_2"] != -1 && i < sensor_num / 2) {
				for (int j = 0; j < 4; ++j) {
					QMatrix4x4 T = QMatrix4x4(); T.translate(T_ts_1[j]);
					shaderProgram.setUniformValue("model_scale", wtransforms[i] * T * scale_tendon);
					this->glDrawArrays(GL_TRIANGLES, start_point_of_tendon, helf_tendon_count * 3);
				}
			}
		}

		// === 渲染中段 Disk ===
		texture.bind(0);  // ❗绑定前置到一次
		shaderProgram.setUniformValue("ourTexture", 0);

		const bool has_section2 = section_nums_rec.contains("section_2") && section_nums_rec["section_2"] != -1;
		int mid_start = 1, mid_end = selectedIndices.size() - 1;

		for (int k = mid_start; k < mid_end; ++k) {
			int i = selectedIndices[k];
			shaderProgram.setUniformValue("model_scale", wtransforms[i]);
			//shaderProgram.setUniformValue("model", wtransforms[i]);
			//shaderProgram.setUniformValue("scale", I);

			if (has_section2) {
				if (k < total_disk_num / 2)
					this->glDrawArrays(GL_TRIANGLES, start_point_of_section_1, section_1_count * 3);
				else
					this->glDrawArrays(GL_TRIANGLES, start_point_of_section_2, section_2_count * 3);
			}
			else {
				this->glDrawArrays(GL_TRIANGLES, start_point_of_section_1, section_1_count * 3);
			}
		}

		// === 渲染 Base ===
		shaderProgram.setUniformValue("model_scale", I);
		// shaderProgram.setUniformValue("scale", I);
		this->glDrawArrays(GL_TRIANGLES, start_point_of_base, type_to_count["base"] * 3);

		// === 渲染 Tip ===
		shaderProgram.setUniformValue("model_scale", wtransforms[sensor_num - 1]);
		this->glDrawArrays(GL_TRIANGLES, start_point_of_tip, type_to_count["tip"] * 3);

		// === 更新 top_position ===
		QMatrix4x4 tip_transform;
		tip_transform.translate(QVector3D(0.f, type_to_size["tip"][2], 0.f) / 50.f);
		tip_transform = wtransforms[sensor_num - 1] * tip_transform;
		top_position = QVector3D(tip_transform(0, 3), tip_transform(1, 3), tip_transform(2, 3));


		// force
		if (!isZeroMatrix(wtransforms[sensor_num])) {
			auto force = QVector3D(wtransforms[sensor_num](0, 0), wtransforms[sensor_num](0, 1), wtransforms[sensor_num](0, 2)) / 50.f;
			// auto force = wtransforms[sensor_num] / 50.f;
			draw_tip_force(top_position, top_position + force);
		}

		// positions
		if (start) {
			positions.append(tip_transform);

			vertices = vertices_after_stl_loaded;
			for (auto s : positions) {
				vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
			}

			VBO.bind();
			int _ = sizeof(float) * vertices.size();
			VBO.allocate(vertices.data(), _);
		}
		if (true) {
			shaderProgram.setUniformValue("model_scale", I);
			glLineWidth(.1 * line_width_factor);
			shaderProgram.setUniformValue("color", color_of_position_1);
			GLsizei size_of_positions = positions.size();
			this->glDrawArrays(GL_LINE_STRIP, start_point_of_lines, size_of_positions);
		}

	}

	if (show_base_frame) {
		draw_base_frame();
	}

}

void Widget::draw_single_section() {
	shaderProgram.setUniformValue("s", 1);
	size_t num = wtransforms.size();
	size_t step = (num - disk_num) / (disk_num - 1) + 1;
	size_t now = 1;

	texture.bind(0);
	shaderProgram.setUniformValue("ourTexture", 0);

	QMatrix4x4 model;
	QMatrix4x4 scale;
	scale.scale(QVector3D((10 / 200.f) * r, (10 / 200.f), (10 / 200.f) * r));
	QMatrix4x4 T;
	T.translate(QVector3D(0.f, -1.f / 2 + 1 / 20.f, 0.f));
	model = T * wtransforms[0];
	shaderProgram.setUniformValue("model", model);
	shaderProgram.setUniformValue("scale", scale);
	this->glDrawArrays(GL_TRIANGLES, 42, 756);
	QVector<QVector3D> T_ts;
	if (tendon_num == 4) {
		T_ts = {
			QVector3D((10 / 200.f) * r * .9, 0, 0),
			QVector3D(0, 0, (10 / 200.f) * r * .9),
			QVector3D(-(10 / 200.f) * r * .9, 0, 0),
			QVector3D(0, 0, -(10 / 200.f) * r * .9)
		};
	}
	else {
		if (tendon_num == 3) {
			T_ts = {
				QVector3D((10 / 200.) * r * .9, 0, 0),
				QVector3D(-(10 / 200.) * r * .9 * .5, 0, (10 / 200.) * r * .9 * sqrt(3.) / 2.),
				QVector3D(-(10 / 200.) * r * .9 * .5, 0, -(10 / 200.) * r * .9 * sqrt(3.) / 2.)
			};
		}
	}

	for (unsigned int i = 1; i < num - 1; i++) {
		if (now == step + 1) {
			texture.bind(0);
			shaderProgram.setUniformValue("ourTexture", 0);
			now = 0;
			QMatrix4x4 model_disk;
			model_disk = T * wtransforms[i];
			shaderProgram.setUniformValue("model", model_disk);
			shaderProgram.setUniformValue("scale", scale);
			this->glDrawArrays(GL_TRIANGLES, 42, 756);
		}
		else {
			texture2.bind(2);
			shaderProgram.setUniformValue("ourTexture", 2);
			QMatrix4x4 model_c;
			QMatrix4x4 scale_c;
			scale_c.scale(QVector3D((10 / 200.f) * r_center, (10 / 100.f), (10 / 200.f) * r_center));
			model_c = T * wtransforms[i];
			shaderProgram.setUniformValue("model", model_c);
			shaderProgram.setUniformValue("scale", scale_c);
			this->glDrawArrays(GL_TRIANGLES, 42, 756);

			for (int j = 0; j < tendon_num; j++) {
				texture1.bind(1);
				shaderProgram.setUniformValue("ourTexture", 1);
				QMatrix4x4 model_t;
				QMatrix4x4 scale_t;
				scale_t.scale(QVector3D((10 / 200.f) * r_tendon, (10 / 100.f), (10 / 200.f) * r_tendon));
				QMatrix4x4 T_t;
				T_t.translate(T_ts[j]);
				model_t = T * wtransforms[i] * T_t;
				shaderProgram.setUniformValue("model", model_t);
				shaderProgram.setUniformValue("scale", scale_t);
				this->glDrawArrays(GL_TRIANGLES, 42, 756);
			}
		}
		now += 1;
	}

	texture.bind(0);
	shaderProgram.setUniformValue("ourTexture", 0);
	QMatrix4x4 model_end;
	model_end = T * wtransforms[num - 1];
	shaderProgram.setUniformValue("model", model_end);
	shaderProgram.setUniformValue("scale", scale);
	this->glDrawArrays(GL_TRIANGLES, 42, 756);


	shaderProgram.setUniformValue("s", 0);
	if (show_frame_1) {
		glLineWidth(100. * frame_width_factor);
		QMatrix4x4 scale;
		scale.scale(.5);
		shaderProgram.setUniformValue("scale", scale);
		shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 798, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 800, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 802, 2);
	}

	if (start) {
		QMatrix4x4 pos;
		pos = model_end;
		positions.append(pos);
		vertices = get_with_norm();
		for (auto s : positions) {
			vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
		}
		VBO.bind();
		int _ = sizeof(float) * vertices.size();
		VBO.allocate(vertices.data(), _);
	}


	if (show_position_frame && show_positions_1) {
		for (int i = 0; i < positions.size(); i += step_of_show_position_frame) {
			glLineWidth(.1 * frame_width_factor);
			QMatrix4x4 scale;
			scale.scale(.5);
			shaderProgram.setUniformValue("scale", scale);
			shaderProgram.setUniformValue("model", positions[i]);
			shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 798, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 800, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 802, 2);
		}
	}

	if (show_positions_1) {
		QMatrix4x4 I;
		I.scale(1.);
		shaderProgram.setUniformValue("model", I);
		shaderProgram.setUniformValue("scale", I);
		glLineWidth(.1 * line_width_factor);
		//qDebug() << line_width_factor;
		shaderProgram.setUniformValue("color", color_of_position_1);
		// 804
		GLsizei size_of_positions = positions.size();
		this->glDrawArrays(GL_LINE_STRIP, 804, size_of_positions);
	}

}

void Widget::draw_base_frame() {
	shaderProgram.setUniformValue("s", 0);
	uint32_t arrow_count = type_to_count[QString("arrow")];

	// Z
	QMatrix4x4 T_1;
	QMatrix4x4 scale_of_base_frame;
	scale_of_base_frame.scale(5.f);
	shaderProgram.setUniformValue("model_scale", T_1 * scale_of_base_frame);
	// shaderProgram.setUniformValue("scale", scale);
	shaderProgram.setUniformValue("color", QVector4D(0.f, 0.f, 255.f, 255.f));
	this->glDrawArrays(GL_TRIANGLES, start_point_of_arrow, arrow_count * 3);

	//X
	T_1.rotate(-90, 0, 0, 1);
	shaderProgram.setUniformValue("model_scale", T_1 * scale_of_base_frame);
	// shaderProgram.setUniformValue("scale", scale);
	shaderProgram.setUniformValue("color", QVector4D(255.f, 0.f, 0.f, 255.f));
	this->glDrawArrays(GL_TRIANGLES, start_point_of_arrow, arrow_count * 3);

	// Y
	QMatrix4x4 T_2;
	T_2.rotate(90, 1, 0, 0);
	shaderProgram.setUniformValue("model_scale", T_2 * scale_of_base_frame);
	// shaderProgram.setUniformValue("scale", scale);
	shaderProgram.setUniformValue("color", QVector4D(0.f, 255.f, 0.f, 255.f));
	this->glDrawArrays(GL_TRIANGLES, start_point_of_arrow, arrow_count * 3);
}

void Widget::draw_tip_force(QVector3D from, QVector3D to) {
	QVector3D defaultDir(0, 1, 0);
	QVector3D targetDir = to - from;
	targetDir.normalize();
	QVector3D rotationAxis = QVector3D::crossProduct(defaultDir, targetDir);
	float angle = std::acos(QVector3D::dotProduct(defaultDir, targetDir));

	// 如果方向几乎一致/相反，可能需要特殊处理
	QMatrix4x4 rotation;
	if (rotationAxis.length() < 1e-6) {
		if (QVector3D::dotProduct(defaultDir, targetDir) < 0) {
			// 方向相反：绕任意垂直轴旋转 180 度
			rotation.rotate(180, 1, 0, 0);  // 或任何与 defaultDir 垂直的轴
		}
		// 否则不旋转
	}
	else {
		rotationAxis.normalize();
		rotation.rotate(qRadiansToDegrees(angle), rotationAxis);
	}
	QMatrix4x4 translation;
	translation.translate(from);
	shaderProgram.setUniformValue("s", 0);
	uint32_t arrow_count = type_to_count[QString("arrow")];

	QMatrix4x4 scale;
	scale.scale(5.f);
	shaderProgram.setUniformValue("model_scale", translation * rotation * scale);
	// shaderProgram.setUniformValue("scale", scale);
	shaderProgram.setUniformValue("color", QVector4D(120.f, 0.f, 120.f, 255.f));
	this->glDrawArrays(GL_TRIANGLES, start_point_of_arrow, arrow_count * 3);
}

void Widget::set_show_positions_1(bool show) {
	show_positions_1 = show;
}

void Widget::set_show_positions_2(bool show) {
	show_positions_2 = show;
}

void Widget::set_show_positions_frame(bool show) {
	show_position_frame = show;
}

void Widget::set_step_of_show_position_frame(int step) {
	step_of_show_position_frame = step;
}

void Widget::set_show_base_frame(bool show) {
	show_base_frame = show;
}

void Widget::set_position_of_base_frame(QVector3D position) {
	position_of_base_frame = position;
}

void Widget::set_line_width(float width) {
	line_width_factor = width;
}

void Widget::set_frame_width(float width) {
	frame_width_factor = width;
}

void Widget::set_color_1(QVector4D color) {
	color_of_position_1 = color;
}

void Widget::set_color_2(QVector4D color) {
	color_of_position_2 = color;
}

void Widget::set_show_frame_1(bool res) {
	show_frame_1 = res;
}

void Widget::set_show_frame_2(bool res) {
	show_frame_2 = res;
}

void Widget::draw_two_sections() {
	shaderProgram.setUniformValue("s", 1);
	size_t num = wtransforms.size();
	size_t step = (num - disk_num * 2 + 1) / (disk_num * 2 - 2) + 1;
	size_t now = 1;
	QMatrix4x4 model_section_1;

	texture.bind(0);
	shaderProgram.setUniformValue("ourTexture", 0);
	QMatrix4x4 model;
	QMatrix4x4 scale;
	scale.scale(QVector3D((10 / 200.f) * r, (10 / 200.f), (10 / 200.f) * r));
	QMatrix4x4 T;
	T.translate(QVector3D(0, -1.f / 2 + 1 / 20.f, 0));
	model = T * wtransforms[0];
	shaderProgram.setUniformValue("model", model);
	shaderProgram.setUniformValue("scale", scale);
	this->glDrawArrays(GL_TRIANGLES, 42, 756);

	QMatrix4x4 rot90;

	QVector<QVector3D> T_ts;
	if (tendon_num == 4) {
		T_ts = {
			QVector3D((10 / 200.) * r * .9, 0, 0),
			QVector3D(0, 0, (10 / 200.) * r * .9),
			QVector3D(-(10 / 200.) * r * .9, 0, 0),
			QVector3D(0, 0, -(10 / 200.) * r * .9)
		};
		rot90.rotate(45, QVector3D(0, 1, 0));
	}
	else {
		if (tendon_num == 3) {
			T_ts = {
				QVector3D((10 / 200.) * r * .9, 0, 0),
				QVector3D(-(10 / 200.) * r * .9 * .5, 0, (10 / 200.f) * r * .9 * sqrt(3.) / 2.),
				QVector3D(-(10 / 200.) * r * .9 * .5, 0, -(10 / 200.f) * r * .9 * sqrt(3.) / 2.)
			};
		}
		rot90.rotate(60, QVector3D(0, 1, 0));
	}

	int d = 1;
	for (unsigned int i = 1; i < num - 1; i++) {
		if (now == step + 1) {
			now = 0;
			texture.bind(0);
			shaderProgram.setUniformValue("ourTexture", 0);
			QMatrix4x4 model_disk;
			model_disk = T * wtransforms[i];
			shaderProgram.setUniformValue("model", model_disk);
			shaderProgram.setUniformValue("scale", scale);
			this->glDrawArrays(GL_TRIANGLES, 42, 756);
			d += 1;
			if (d == disk_num) {
				model_section_1 = model_disk;
			}
		}
		else {
			texture2.bind(2);
			shaderProgram.setUniformValue("ourTexture", 2);
			QMatrix4x4 model_c;
			QMatrix4x4 scale_c;
			scale_c.scale(QVector3D((10 / 200.f) * r_center, (10 / 100.f), (10 / 200.f) * r_center));
			model_c = T * wtransforms[i];
			shaderProgram.setUniformValue("model", model_c);
			shaderProgram.setUniformValue("scale", scale_c);
			this->glDrawArrays(GL_TRIANGLES, 42, 756);

			if (d < disk_num) {
				for (int j = 0; j < tendon_num; j++) {
					texture1.bind(1);
					shaderProgram.setUniformValue("ourTexture", 1);
					QMatrix4x4 model_t;
					QMatrix4x4 scale_t;
					scale_t.scale(QVector3D((10 / 200.f) * r_tendon, (10 / 100.f), (10 / 200.f) * r_tendon));
					QMatrix4x4 T_t;
					T_t.translate(T_ts[j]);
					model_t = T * wtransforms[i] * rot90 * T_t;
					shaderProgram.setUniformValue("model", model_t);
					shaderProgram.setUniformValue("scale", scale_t);
					this->glDrawArrays(GL_TRIANGLES, 42, 756);
				}
			}
			for (int j = 0; j < tendon_num; j++) {
				texture1.bind(1);
				shaderProgram.setUniformValue("ourTexture", 1);
				QMatrix4x4 model_t;
				QMatrix4x4 scale_t;
				scale_t.scale(QVector3D((10 / 200.f) * r_tendon, (10 / 100.f), (10 / 200.f) * r_tendon));
				QMatrix4x4 T_t;
				T_t.translate(T_ts[j]);
				model_t = T * wtransforms[i] * T_t;
				shaderProgram.setUniformValue("model", model_t);
				shaderProgram.setUniformValue("scale", scale_t);
				this->glDrawArrays(GL_TRIANGLES, 42, 756);
			}
		}
		now += 1;
	}

	texture.bind(0);
	shaderProgram.setUniformValue("ourTexture", 0);
	QMatrix4x4 model_end;
	model_end = T * wtransforms[num - 1];
	shaderProgram.setUniformValue("model", model_end);
	shaderProgram.setUniformValue("scale", scale);
	this->glDrawArrays(GL_TRIANGLES, 42, 756);


	shaderProgram.setUniformValue("s", 0);
	if (show_frame_2) {
		glLineWidth(100. * frame_width_factor);
		QMatrix4x4 scale;
		scale.scale(.5);
		shaderProgram.setUniformValue("scale", scale);
		shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 798, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 800, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 802, 2);
	}

	if (show_frame_1) {
		glLineWidth(100. * frame_width_factor);
		QMatrix4x4 scale;
		scale.scale(.5);
		shaderProgram.setUniformValue("model", model_section_1);
		shaderProgram.setUniformValue("scale", scale);
		shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 798, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 800, 2);
		shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
		this->glDrawArrays(GL_LINE_STRIP, 802, 2);
	}

	if (start) {
		QMatrix4x4 pos;
		pos = model_end;
		positions.append(pos);
		vertices = my_vertices;
		for (auto s : positions) {
			vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
		}

		sub_positions_1.append(model_section_1);
		for (auto s : sub_positions_1) {
			vertices << s(0, 3) << s(1, 3) << s(2, 3) << 1.f << 1.f << 0.f << 1.f << 0.f;
		}
		VBO.bind();
		int _ = sizeof(float) * vertices.size();
		VBO.allocate(vertices.data(), _);
	}


	if (show_position_frame && show_positions_2) {
		for (int i = 0; i < positions.size(); i += step_of_show_position_frame) {
			glLineWidth(.1 * frame_width_factor);
			QMatrix4x4 scale;
			scale.scale(.5);
			shaderProgram.setUniformValue("scale", scale);
			shaderProgram.setUniformValue("model", positions[i]);
			shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 798, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 800, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 802, 2);
		}
	}

	if (show_position_frame && show_positions_1) {
		for (int i = 0; i < positions.size(); i += step_of_show_position_frame) {
			glLineWidth(.1 * frame_width_factor);
			QMatrix4x4 scale;
			scale.scale(.5);
			shaderProgram.setUniformValue("scale", scale);
			shaderProgram.setUniformValue("model", sub_positions_1[i]);
			shaderProgram.setUniformValue("color", QVector4D(255., 0., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 798, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 255., 0., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 800, 2);
			shaderProgram.setUniformValue("color", QVector4D(0., 0., 255., 255.));
			this->glDrawArrays(GL_LINE_STRIP, 802, 2);
		}
	}

	GLint size_of_positions_2 = positions.size();
	GLint size_of_positions_1 = sub_positions_1.size();
	if (show_positions_2) {
		QMatrix4x4 I;
		I.scale(1.);
		shaderProgram.setUniformValue("model", I);
		shaderProgram.setUniformValue("scale", I);
		glLineWidth(.1 * line_width_factor);
		shaderProgram.setUniformValue("color", color_of_position_2);
		// 804
		this->glDrawArrays(GL_LINE_STRIP, 804, size_of_positions_2);
	}

	if (show_positions_1) {
		QMatrix4x4 I;
		I.scale(1.);
		shaderProgram.setUniformValue("model", I);
		shaderProgram.setUniformValue("scale", I);
		glLineWidth(.1 * line_width_factor);
		//qDebug() << line_width_factor;
		shaderProgram.setUniformValue("color", color_of_position_1);
		// 804
		this->glDrawArrays(GL_LINE_STRIP, 804 + size_of_positions_2, size_of_positions_1);
	}


}

void Widget::draw_floor() {
	shaderProgram.setUniformValue("s", 1);
	int all = 16;
	int last_texture_id = -1;

	auto bind_texture = [&](int tex_unit, QOpenGLTexture& tex, int tex_id) {
		if (last_texture_id != tex_id) {
			tex.bind(tex_unit);
			shaderProgram.setUniformValue("ourTexture", tex_unit);
			last_texture_id = tex_id;
		}
		};

	QMatrix4x4 scale;
	scale.scale(1.);
	// shaderProgram.setUniformValue("scale", scale);

	for (int i = 0; i < all; ++i) {
		float x = 8. - i;
		for (int j = 0; j <= all; ++j) {
			float z = 8. - j;
			int checker = (i + j) % 2;
			bind_texture(checker == 0 ? 2 : 3, checker == 0 ? texture_black : texture1_white, checker);

			QMatrix4x4 model;
			model.translate(QVector3D(x, 0, z));
			shaderProgram.setUniformValue("model_scale", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

bool Widget::event(QEvent* e) {
	camera.handle(e);
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent* event = static_cast<QKeyEvent*>(e);
		if (event->key() == Qt::Key_Escape) {
			//doing nothing is better
		}
	}
	return QWidget::event(e);   //调用父类的事件分发函数
}
