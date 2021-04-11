#ifndef GLSL_PROGRAM_INCLUDED
#define GLSL_PROGRAM_INCLUDED

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <stdexcept>
#include <glad/glad.h>
#include <cassert>
#include <sys/stat.h>

#include <iostream>

class GLSLProgramException : public std::runtime_error 
{
public:
    GLSLProgramException (const std::string &msg) 
        : std::runtime_error(msg) {}

    GLSLProgramException (const char *msg)
        : std::runtime_error(msg) {}
};

class GLSLProgram 
{
public:
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
    static const size_t nShaderTypes = 6;
    GLuint handle_;
    bool   linked_;
    GLuint shaders_ [nShaderTypes];
    std::map <std::string, int> uniformLocations;

    void  throwErrorLogException  (GLuint handle, GLenum handleType, 
          const char *headMessege = "") const;
    int   getShaderIndex     (GLenum type) const;
    GLint getUniformLocation (const char *name);
    bool  fileExists         (const std::string &fileName);
    std::string getExtension (const char *fileName);
    
    GLSLProgram             (const GLSLProgram &other) = delete;
    GLSLProgram &operator = (const GLSLProgram &other) = delete;
};
                            
bool isShader  (GLenum type);
bool isProgram (GLenum type);

#endif // !GLSL_PROGRAM_INCLUDED