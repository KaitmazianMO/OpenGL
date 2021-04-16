#ifndef GLSL_PROGRAM_INCLUDED
#define GLSL_PROGRAM_INCLUDED

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <stdexcept>
#include <glad/glad.h>
#include <cassert>
#include <sys/stat.h>
#include <algorithm>

#include <iostream>

namespace ShaderInfo
{
    struct ShaderFileExtension
    {
        const char *ext;
        GLenum type;
    };

    static ShaderFileExtension extensions[] = {
        { ".vs",   GL_VERTEX_SHADER },
        { ".vert", GL_VERTEX_SHADER },
        { ".gs",   GL_GEOMETRY_SHADER },
        { ".geom", GL_GEOMETRY_SHADER },
        { ".tcs",  GL_TESS_CONTROL_SHADER },
        { ".tes",  GL_TESS_EVALUATION_SHADER },
        { ".fs",   GL_FRAGMENT_SHADER },
        { ".frag", GL_FRAGMENT_SHADER },
        { ".cs",   GL_COMPUTE_SHADER }
    };
    static const size_t nShaderTypes = 6;

    GLenum translateFileExtension (const char *fileName);
}

class GLSLProgramException 
{
public:
    GLSLProgramException (const std::string &msg) 
        : m_what (msg) {}

    GLSLProgramException (const char *msg)
        : m_what(msg) {}
    
    GLSLProgramException (GLuint handle, GLenum handleType, 
          const char *headMessege = "");

    std::string what() const;

private:
    std::string m_what;
};

class GLSLProgram 
{
public:
    using uniformTable = std::map <std::string, int>;

    GLSLProgram();
   ~GLSLProgram();
                                 
    void   compileShader (const char *fileName);
    void   compileShader (const char *fileName, GLenum type);
    void   compileShader (const char *sourse, GLenum type,
           const char *fileName);

    void   link();
    void   validate();
    void   use() const;
    GLuint getHandle();
    bool   isLinked() const;
    void   bindAttribLocation   (GLuint location, const char *name);
    void   bindFragDataLocation (GLuint location, const char *name);

    void   setUniform (const char *name, float x, float y, float z);
    void   setUniform (const char *name, const glm::vec2 &v);
    void   setUniform (const char *name, const glm::vec3 &v);
    void   setUniform (const char *name, const glm::vec4 &v);
    void   setUniform (const char *name, const glm::mat4 &m);
    void   setUniform (const char *name, const glm::mat3 &m);
    void   setUniform (const char *name, float val);
    void   setUniform (const char *name, int val);
    void   setUniform (const char *name, bool val);
    void   setUniform (const char *name, GLuint val);
    void   findUniformLocations();
    void   printActiveUniforms();
    void   printActiveUniformBlocks();
    void   printActiveAttribs();
    const char *getTypeString (GLenum type);

private:
    GLuint       m_programHandle;
    bool         m_isLinked;
    GLuint       m_shaderHandels [ShaderInfo::nShaderTypes];
    uniformTable m_uniformLocations;

    int    getShaderIndex     (GLenum type) const;
    GLint  getUniformLocation (const char *name);
    bool   fileExists         (const std::string &fileName);
    
    GLSLProgram             (const GLSLProgram &other) = delete;
    GLSLProgram &operator = (const GLSLProgram &other) = delete;
};
                            
bool isShader  (GLenum type);
bool isProgram (GLenum type);

#endif // !GLSL_PROGRAM_INCLUDED