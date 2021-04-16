#include "GLSLProgram.h"

static GLint SUCCESS = false;

static std::string matchTypeToError (GLenum type);
static char       *readFile    (FILE *file);
static size_t      getFileSize (FILE *file);


GLSLProgram::GLSLProgram()
    : m_programHandle (glCreateProgram()), m_isLinked (false), m_shaderHandels()
{
    if (m_programHandle == 0)
        throw GLSLProgramException ("Can't create a program");
}

GLSLProgram::~GLSLProgram()
{
    for (const auto &handle : m_shaderHandels)
        if (handle)
            glDeleteShader (handle);
    
    glDeleteProgram (m_programHandle);
}

void GLSLProgram::compileShader (const char *fileName)
{
    GLenum shaderType = ShaderInfo::translateFileExtension (fileName);
    compileShader (fileName, shaderType);
}

void GLSLProgram::compileShader (const char *fileName, GLenum type)
{
    assert (fileName);

    FILE *shaderFile = fopen (fileName, "rb");
    if (!shaderFile)
        throw GLSLProgramException ("Can't open the file " + std::string (fileName));

    char *shaderSourse = readFile (shaderFile);
    assert (shaderSourse);
    
    fclose (shaderFile);

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

    m_shaderHandels[index] = glCreateShader (type);
    if (m_shaderHandels[index] == 0)
        throw GLSLProgramException (std::string (fileName) + "\nCan't create a shader");
    
    glShaderSource (m_shaderHandels[index], 1, &sourse, nullptr);
    glCompileShader (m_shaderHandels[index]);
    
    glGetShaderiv (m_shaderHandels[index], GL_COMPILE_STATUS, &SUCCESS);
    if (SUCCESS == GL_FALSE)
        throw GLSLProgramException (m_shaderHandels[index], type, fileName);
    
    glAttachShader (m_programHandle, m_shaderHandels[index]);
}

void GLSLProgram::link()
{
    glLinkProgram (m_programHandle);

    glGetProgramiv (m_programHandle, GL_LINK_STATUS, &SUCCESS);
    if (SUCCESS == GL_FALSE)
        throw GLSLProgramException (m_programHandle, GL_PROGRAM);

    m_isLinked = GL_TRUE;
}

void GLSLProgram::use() const
{
    if (!m_isLinked || m_programHandle == 0)
        throw GLSLProgramException ("Program was not linked");
    
    glUseProgram (m_programHandle);
}

bool GLSLProgram::isLinked() const
{
    return m_isLinked;
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

GLSLProgramException::GLSLProgramException (GLuint handle, GLenum handleType, 
    const char *headMessege)
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

    m_what = formatedMessage.c_str();
}

std::string GLSLProgramException::what() const
{
    return m_what;
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

GLenum ShaderInfo::translateFileExtension (const char *fileName)
{
    const char *fileExtension = strrchr (fileName, '.');
    if (!fileExtension)
        throw GLSLProgramException ("The file" + std::string (fileName) + "has no extension");


    for (const auto &ext : extensions)
    {
        if (strcmp (fileExtension, ext.ext) == 0)
            return ext.type;
    }

    throw GLSLProgramException ("The file" + std::string (fileName) + "has invalid extension");
    return 0;    
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