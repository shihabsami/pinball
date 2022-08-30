#ifndef DEBUG_H
#define DEBUG_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <sstream>

static void print_float(float f, const char* label = "float") {
    printf("DEBUG: %*s : %.3f\n", -10, label, f);
}

static void print_vec3(const glm::vec3& v, const char* label = "vec3") {
    printf("DEBUG: %*s : %*.3f, %*.3f, %*.3f (x, y, z)\n", -10, label, 7, v.x, 7, v.y, 7, v.z);
}

static void print_quat(const glm::quat& q, const char* label = "quat") {
    printf("DEBUG: %*s: %*.3f, %*.3f, %*.3f, %*.3f (w, x, y, z)\n", -10, label, -7, q.w, -7, q.x, -7, q.y, -7, q.z);
}

static void print_mat3(const glm::mat3& m, const char* label = "mat3") {
    printf("DEBUG: %s:\n", label);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            printf("%.3f ", m[i][j]);

        printf("\n");
    }
}

static void print_mat4(const glm::mat4& m, const char* label = "mat4") {
    printf("DEBUG: %s:\n", label);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            printf("%.3f ", m[i][j]);

        printf("\n");
    }
}

static void GLAPIENTRY gl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* user_param) {
    std::string source_description, type_description, severity_description;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        source_description = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_description = "WINDOW SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_description = "SHADER COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_description = "THIRD PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_description = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
    default:
        source_description = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_description = "DEPRECATED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_description = "UNDEFINED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_description = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        type_description = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        type_description = "OTHER";
        break;
    case GL_DEBUG_TYPE_MARKER:
        type_description = "MARKER";
        break;
    default:
        type_description = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_description = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_description = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_description = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_description = "NOTIFICATION";
        break;
    default:
        severity_description = "UNKNOWN";
        break;
    }

    if (type == GL_DEBUG_TYPE_ERROR) {
        std::ostringstream stream;
        stream << std::endl << "ERROR: source: " << source_description << ", severity: " << severity_description
            << ", message: " << message << std::endl;
        std::string error_description = stream.str();
        throw std::runtime_error(error_description);
    } else {
        printf("\nDEBUG: %s: source: %s, severity: %s, message: %s\n",
            type_description.c_str(), source_description.c_str(), severity_description.c_str(), message);
    }
}

#endif // DEBUG_H
