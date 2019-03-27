// Winter 2019

#include "GeometryNode.hpp"

#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <sstream>


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::updateShaderUniforms(bool picking, ShaderProgram &shader, glm::mat4 & viewMatrix){
    shader.enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader.getUniformLocation("ModelView");
        mat4 modelView = viewMatrix * parent_trans * trans * trackball_mat;
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;
        
        if(picking){
            float r = float( m_nodeId       &0xff) / 255.0f;
            float g = float((m_nodeId >> 8) &0xff) / 255.0f;
            float b = float((m_nodeId >>16) &0xff) / 255.0f;
            
            location = shader.getUniformLocation("material.kd");
            glUniform3f( location, r, g, b );
            CHECK_GL_ERRORS;
        }else{
            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;
            
            
            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = material.kd;
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.ks");
            vec3 ks = material.ks;
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.shininess");
            glUniform1f(location, material.shininess);
            CHECK_GL_ERRORS;
        }
        
        
        
    }
    shader.disable();
}

void GeometryNode::geoRender(bool picking, ShaderProgram &shader, glm::mat4 &viewMat, BatchInfoMap &batchInfoMap){
    
    updateShaderUniforms(picking, shader, viewMat);
    BatchInfo batchInfo = batchInfoMap[meshId];
    shader.enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    shader.disable();
}
