#include "GLSLProgram.h"

static GLint SUCCESS = false;

static std::string matchTypeToError (GLenum type);
static char       *readFile    (FILE *file);
static size_t      getFileSize (FILE *file);


GLSLProgram::GLSLProgram()
    : handle_ (glCreateProgram()), linked_ (false), shaders_()
{
    if (handle_ == 0)
        throw GLSLProgramException ("Can't create a program");
}

GLSLProgram::~GLSLProgram()
{
    for (const auto &handle : shaders_)
        if (handle)
            glDeleteShader (handle);
    
    glDeleteProgram (handle_);
}

void GLSLProgram::compileShader (const char *fileName, GLenum type)
{
    assert (fileName);

    FILE *shaderFile = fopen (fileName, "rb");
    if (!shaderFile)
        throw GLSLProgramException ("Can't open the file " + std::string (fileName));

    char *shaderSourse = readFile (shaderFile);
    assert (shaderSourse);

    compileShader (shaderSourse, type, fileName);

    delete [] shaderSourse;
}

void GLSLProgram::compileShader (const char *sourse, GLenum type, const char *fileName) 
{
    assert (sourse);
    assert (fileName);

    int index = getShaderIndex (type);
    if (index < 0)
        throw GLSLProgramException ("Invalid shader type value");

    shaders_[index] = glCreateShader (type);
    if (shaders_[index] == 0)
        throw GLSLProgramException (std::string (fileName) + "\nCan't create a shader");
    
    glShaderSource (shaders_[index], 1, &sourse, nullptr);
    glCompileShader (shaders_[index]);
    
    glGetShaderiv (shaders_[index], GL_COMPILE_STATUS, &SUCCESS);
    if (SUCCESS == GL_FALSE)
        throwErrorLogException (shaders_[index], type, fileName);
    
    glAttachShader (handle_, shaders_[index]);
}

void GLSLProgram::throwErrorLogException (GLuint handle, GLenum handleType, 
    const char *headMessege) const
{           
    assert (headMessege);

    int length = 0;
    std::string formatedMessage = std::string (headMessege) + "\n" + matchTypeToError (handleType) + '\n';
    
    if      (isProgram (handleType))
        glGetProgramiv (handle, GL_INFO_LOG_LENGTH, &length);
    else if (isShader (handleType))
        glGetShaderiv (handle, GL_INFO_LOG_LENGTH, &length);

    if (length > 0)
    {
        char *logMessage = new char [length] {};
        assert (logMessage);

        if      (isProgram (handleType))
            glGetProgramInfoLog (handle, length, nullptr, logMessage);
        else if (isShader (handleType))
            glGetShaderInfoLog (handle, length, nullptr, logMessage);
        
        formatedMessage += std::string (logMessage);
        delete [] logMessage;
    }

    throw GLSLProgramException (formatedMessage.c_str());
}

void GLSLProgram::link()
{
    glLinkProgram (handle_);

    glGetProgramiv (handle_, GL_LINK_STATUS, &SUCCESS);
    if (SUCCESS == GL_FALSE)
        throwErrorLogException (handle_, GL_PROGRAM, "");

    linked_ = GL_TRUE;
}

void GLSLProgram::use() const
{
    if (!linked_ || handle_ == 0)
        throw GLSLProgramException ("Program was not linked");
    
    glUseProgram (handle_);
}

bool GLSLProgram::isLinked() const
{
    return linked_;
}

bool isShader (GLenum type)
{
    return type == GL_VERTEX_SHADER          ||
           type == GL_FRAGMENT_SHADER        ||
           type == GL_GEOMETRY_SHADER        ||
           type == GL_TESS_CONTROL_SHADER    ||
           type == GL_TESS_EVALUATION_SHADER ||
           type == GL_COMPUTE_SHADER;
}

bool isProgram (GLenum type)
{
    return type == GL_PROGRAM;
}

int GLSLProgram::getShaderIndex (GLenum type) const
{
    switch (type)
    {
        case GL_VERTEX_SHADER          : return 0;
        case GL_FRAGMENT_SHADER        : return 1;
        case GL_GEOMETRY_SHADER        : return 2;
        case GL_TESS_CONTROL_SHADER    : return 3;
        case GL_TESS_EVALUATION_SHADER : return 4;
        case GL_COMPUTE_SHADER         : return 5;
        default                        : return -1;
    }
    return -1;
}

std::string matchTypeToError (GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER          : return "ERROR::SHADER::VERTEX::COMPILATION_FAILED";
        case GL_FRAGMENT_SHADER        : return "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED";
        case GL_GEOMETRY_SHADER        : return "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED";
        case GL_TESS_CONTROL_SHADER    : return "ERROR::SHADER::TESS_CONTROL::COMPILATION_FAILED";
        case GL_TESS_EVALUATION_SHADER : return "ERROR::SHADER::TESS_EVALUATION::COMPILATION_FAILED";
        case GL_COMPUTE_SHADER         : return "ERROR::SHADER::COMPUTE::COMPILATION_FAILED";
        case GL_PROGRAM                : return "ERROR::SHADER::PROGRAM::LINKING_FAILED";
    }
    return "ERROR";
}

static size_t getFileSize (FILE *file)
{
    assert (file);
    struct stat file_info = {};
    fstat (_fileno (file), &file_info);

    return file_info.st_size;
}

static char *readFile (FILE *file)
{
    assert (file);

    const size_t fileSize = getFileSize (file);
    
    char *sourse = new char [fileSize + 1] {};
    fread (sourse, sizeof (*sourse), fileSize, file);
    
    return sourse;
}