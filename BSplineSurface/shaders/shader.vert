#version 330 core
layout(location = 0) in vec3 a_Position; //点坐标
layout(location = 1) in vec3 a_Normal;   //点法向
//layout(location = 2) in vec3 a_Color;    //点颜色
out vec4 v_Color;   //输出颜色

uniform vec3 u_DiffuseLight;   //漫反射光
uniform vec3 u_LightDirection; //光线方向
uniform vec3 u_AmbientLight;   //环境光

uniform mat4 u_Model;
uniform mat4 u_Normal;  //model变换后，法向变换矩阵
uniform mat4 u_View;
uniform mat4 u_Projection;
void main()
{
    vec3 a_Color = vec3(1.0, 0.0, 0.0);
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0); 
	vec3 normal = normalize(vec3(u_Normal * vec4(a_Normal,0.0))); //计算变换后法向
	float nDotL = max(dot(u_LightDirection, normal),0);   //计算法向与光线方向内积
	vec3 diffuseColor = u_DiffuseLight * a_Color.rgb * nDotL;   //计算颜色

	vec3 ambient = u_AmbientLight * a_Color.rgb; 

	v_Color = vec4(diffuseColor + ambient, 1.0);

    
};
