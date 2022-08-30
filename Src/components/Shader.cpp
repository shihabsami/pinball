#include "Shader.h"

#include <fstream>
#include <sstream>

void Shader::init(const char* vert_path, const char* frag_path, const char* geom_path) {
    auto compile = [&](const char* path, GLenum type, GLuint& shader) {
        if (path) {
            try {
                m_placeholder = read_file(path);
                const char* source = m_placeholder.c_str();
                shader = glCreateShader(type);
                glShaderSource(shader, 1, &source, nullptr);
                glCompileShader(shader);
                if (!is_successful(shader, type, path))
                    glDeleteShader(shader);
            } catch (const std::ifstream::failure& failure) {
                fprintf(stderr, "ERROR: Failed to read shader file %s: %s\n", path, failure.what());
            }
        }
    };

    auto attach = [&](GLuint shader) {
        if (shader != 0) {
            glAttachShader(m_id, shader);
        }
    };

    auto free = [](GLuint shader) {
        if (shader != 0) {
            glDeleteShader(shader);
        }
    };

    GLuint vert_shader = 0, frag_shader = 0, geom_shader = 0;

    compile(vert_path, GL_VERTEX_SHADER, vert_shader);
    compile(frag_path, GL_FRAGMENT_SHADER, frag_shader);
    compile(geom_path, GL_GEOMETRY_SHADER, geom_shader);

    m_id = glCreateProgram();

    attach(vert_shader);
    attach(frag_shader);
    attach(geom_shader);

    glLinkProgram(m_id);

    free(vert_shader);
    free(frag_shader);
    free(geom_shader);

    if (!is_successful(m_id, GL_PROGRAM))
        throw std::runtime_error("ERROR: Failed to initialise shader\n");
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::done() const {
    glDeleteProgram(m_id);
}

std::string Shader::read_file(const char* path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path);
    std::stringstream sstream;
    sstream << file.rdbuf();
    file.close();

    return sstream.str();
}

bool Shader::is_successful(GLuint shader, GLenum type, const char* file) {
    GLint success = 0;
    GLsizei log_max_length = 1024;
    GLchar info_log[1024];

    switch (type) {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
    case GL_GEOMETRY_SHADER:
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, log_max_length, nullptr, info_log);
            fprintf(stderr, "ERROR: Failed to compile shader %s: %s\n", file, info_log);
        }
        return success;
    case GL_PROGRAM:
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, log_max_length, nullptr, info_log);
            fprintf(stderr, "ERROR: Failed to link shader program: %s\n", info_log);
        }
        return success;
    default:
        break;
    }

    return success;
}
