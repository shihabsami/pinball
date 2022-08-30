#ifndef SHADER_H
#define SHADER_H

#include "Lighting.h"
#include "Material.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

class Shader {
public:
    void init(const char* vert_path, const char* frag_path, const char* geom_path = nullptr);

    void use() const;

    void done() const;

    void set_bool(const char* name, bool value) const { glUniform1i(glGetUniformLocation(m_id, name), value); }
    void set_uint(const char* name, unsigned int value) const { glUniform1ui(glGetUniformLocation(m_id, name), value); }
    void set_int(const char* name, int value) const { glUniform1i(glGetUniformLocation(m_id, name), value); }
    void set_float(const char* name, float value) const { glUniform1f(glGetUniformLocation(m_id, name), value); }
    void set_vec2(const char* name, const glm::vec2& value) const { glUniform2fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value)); }
    void set_vec2(const char* name, float x, float y) const { glUniform2f(glGetUniformLocation(m_id, name), x, y); }
    void set_vec3(const char* name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value)); }
    void set_vec3(const char* name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(m_id, name), x, y, z); }
    void set_vec4(const char* name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(value)); }
    void set_vec4(const char* name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(m_id, name), x, y, z, w); }
    void set_mat2(const char* name, const glm::mat2& value) const { glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value)); }
    void set_mat3(const char* name, const glm::mat3& value) const { glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value)); }
    void set_mat4(const char* name, const glm::mat4& value) const { glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(value)); }

    void set_array_bool(const char* array_name, int array_index, bool value) { set_bool(format_array_name(array_name, array_index), value); }
    void set_array_uint(const char* array_name, int array_index, unsigned int value) { set_uint(format_array_name(array_name, array_index), value); }
    void set_array_int(const char* array_name, int array_index, int value) { set_int(format_array_name(array_name, array_index), value); }
    void set_array_float(const char* array_name, int array_index, float value) { set_float(format_array_name(array_name, array_index), value); }
    void set_array_vec2(const char* array_name, int array_index, const glm::vec2& value) { set_vec2(format_array_name(array_name, array_index), value); }
    void set_array_vec2(const char* array_name, int array_index, float x, float y) { set_vec2(format_array_name(array_name, array_index), x, y); }
    void set_array_vec3(const char* array_name, int array_index, const glm::vec3& value) { set_vec3(format_array_name(array_name, array_index), value); }
    void set_array_vec3(const char* array_name, int array_index, float x, float y, float z) { set_vec3(format_array_name(array_name, array_index), x, y, z); }
    void set_array_vec4(const char* array_name, int array_index, const glm::vec4& value) { set_vec4(format_array_name(array_name, array_index), value); }
    void set_array_vec4(const char* array_name, int array_index, float x, float y, float z, float w) { set_vec4(format_array_name(array_name, array_index), x, y, z, w); }
    void set_array_mat2(const char* array_name, int array_index, const glm::mat2& value) { set_mat2(format_array_name(array_name, array_index), value); }
    void set_array_mat3(const char* array_name, int array_index, const glm::mat3& value) { set_mat3(format_array_name(array_name, array_index), value); }
    void set_array_mat4(const char* array_name, int array_index, const glm::mat4& value) { set_mat4(format_array_name(array_name, array_index), value); }

    void set_struct_bool(const char* struct_name, const char* element_name, bool value) { set_bool(format_struct_name(struct_name, element_name), value); }
    void set_struct_uint(const char* struct_name, const char* element_name, unsigned int value) { set_uint(format_struct_name(struct_name, element_name), value); }
    void set_struct_int(const char* struct_name, const char* element_name, int value) { set_int(format_struct_name(struct_name, element_name), value); }
    void set_struct_float(const char* struct_name, const char* element_name, float value) { set_float(format_struct_name(struct_name, element_name), value); }
    void set_struct_vec2(const char* struct_name, const char* element_name, const glm::vec2& value) { set_vec2(format_struct_name(struct_name, element_name), value); }
    void set_struct_vec2(const char* struct_name, const char* element_name, float x, float y) { set_vec2(format_struct_name(struct_name, element_name), x, y); }
    void set_struct_vec3(const char* struct_name, const char* element_name, const glm::vec3& value) { set_vec3(format_struct_name(struct_name, element_name), value); }
    void set_struct_vec3(const char* struct_name, const char* element_name, float x, float y, float z) { set_vec3(format_struct_name(struct_name, element_name), x, y, z); }
    void set_struct_vec4(const char* struct_name, const char* element_name, const glm::vec4& value) { set_vec4(format_struct_name(struct_name, element_name), value); }
    void set_struct_vec4(const char* struct_name, const char* element_name, float x, float y, float z, float w) { set_vec4(format_struct_name(struct_name, element_name), x, y, z, w); }
    void set_struct_mat2(const char* struct_name, const char* element_name, const glm::mat2& value) { set_mat2(format_struct_name(struct_name, element_name), value); }
    void set_struct_mat3(const char* struct_name, const char* element_name, const glm::mat3& value) { set_mat3(format_struct_name(struct_name, element_name), value); }
    void set_struct_mat4(const char* struct_name, const char* element_name, const glm::mat4& value) { set_mat4(format_struct_name(struct_name, element_name), value); }

    void set_array_struct_bool(const char* array_name, int array_index, const char* element_name, bool value) { set_bool(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_uint(const char* array_name, int array_index, const char* element_name, unsigned int value) { set_uint(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_int(const char* array_name, int array_index, const char* element_name, int value) { set_int(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_float(const char* array_name, int array_index, const char* element_name, float value) { set_float(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_vec2(const char* array_name, int array_index, const char* element_name, const glm::vec2& value) { set_vec2(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_vec2(const char* array_name, int array_index, const char* element_name, float x, float y) { set_vec2(format_array_struct_name(array_name, array_index, element_name), x, y); }
    void set_array_struct_vec3(const char* array_name, int array_index, const char* element_name, const glm::vec3& value) { set_vec3(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_vec3(const char* array_name, int array_index, const char* element_name, float x, float y, float z) { set_vec3(format_array_struct_name(array_name, array_index, element_name), x, y, z); }
    void set_array_struct_vec4(const char* array_name, int array_index, const char* element_name, const glm::vec4& value) { set_vec4(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_vec4(const char* array_name, int array_index, const char* element_name, float x, float y, float z, float w) { set_vec4(format_array_struct_name(array_name, array_index, element_name), x, y, z, w); }
    void set_array_struct_mat2(const char* array_name, int array_index, const char* element_name, const glm::mat2& value) { set_mat2(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_mat3(const char* array_name, int array_index, const char* element_name, const glm::mat3& value) { set_mat3(format_array_struct_name(array_name, array_index, element_name), value); }
    void set_array_struct_mat4(const char* array_name, int array_index, const char* element_name, const glm::mat4& value) { set_mat4(format_array_struct_name(array_name, array_index, element_name), value); }

    void set_material(const char* name, const Material& material) {
        set_struct_vec3(name, "diffuse_color", material.diffuse_color);
        set_struct_vec3(name, "specular_color", material.specular_color);
        set_struct_bool(name, "has_diffuse_map", material.has_diffuse_map);
        set_struct_bool(name, "has_specular_map", material.has_specular_map);
        set_struct_int(name, "diffuse_map", DIFFUSE_MAP);
        set_struct_int(name, "specular_map", SPECULAR_MAP);
        set_struct_float(name, "shininess", material.shininess);
    }

    void set_array_material(const char* name, int array_index, const Material& material) {
        set_array_struct_vec3(name, array_index, "diffuse_color", material.diffuse_color);
        set_array_struct_vec3(name, array_index, "specular_color", material.specular_color);
        set_array_struct_bool(name, array_index, "has_diffuse_map", material.has_diffuse_map);
        set_array_struct_bool(name, array_index, "has_specular_map", material.has_specular_map);
        set_array_struct_int(name, array_index, "diffuse_map", DIFFUSE_MAP);
        set_array_struct_int(name, array_index, "specular_map",SPECULAR_MAP);
        set_array_struct_float(name, array_index, "shininess", material.shininess);
    }

    void set_directional_light(const char* name, const Directional_Light& light) {
        set_struct_int(name, "type", light.type);
        set_struct_vec3(name, "ambient", light.ambient);
        set_struct_vec3(name, "diffuse", light.diffuse);
        set_struct_vec3(name, "specular", light.specular);
        set_struct_vec3(name, "direction", light.direction);
    }

    void set_point_light(const char* name, const Point_Light& light) {
        set_struct_int(name, "type", light.type);
        set_struct_vec3(name, "ambient", light.ambient);
        set_struct_vec3(name, "diffuse", light.diffuse);
        set_struct_vec3(name, "specular", light.specular);
        set_struct_vec3(name, "position", light.position);
        set_struct_float(name, "constant", light.constant);
        set_struct_float(name, "linear", light.linear);
        set_struct_float(name, "quadratic", light.quadratic);
        set_struct_float(name, "shadows", light.shadows);
    }

    void set_array_directional_light(const char* name, int array_index, const Directional_Light& light) {
        set_array_struct_int(name, array_index, "type", light.type);
        set_array_struct_vec3(name, array_index, "ambient", light.ambient);
        set_array_struct_vec3(name, array_index, "diffuse", light.diffuse);
        set_array_struct_vec3(name, array_index, "specular", light.specular);
        set_array_struct_vec3(name, array_index, "direction", light.direction);
    }

    void set_array_point_light(const char* name, int array_index, const Point_Light& light) {
        set_array_struct_int(name, array_index, "type", light.type);
        set_array_struct_vec3(name, array_index, "ambient", light.ambient);
        set_array_struct_vec3(name, array_index, "diffuse", light.diffuse);
        set_array_struct_vec3(name, array_index, "specular", light.specular);
        set_array_struct_vec3(name, array_index, "position", light.position);
        set_array_struct_float(name, array_index, "constant", light.constant);
        set_array_struct_float(name, array_index, "linear", light.linear);
        set_array_struct_float(name, array_index, "quadratic", light.quadratic);
        set_array_struct_float(name, array_index, "shadows", light.shadows);
    }

    void set_lighting_model(const Lighting_Model& lighting_model) {
        set_int("light_count", (int) lighting_model.m_light_count);
        int i = 0;
        if (lighting_model.m_directional_light)
            set_array_directional_light("lights", i++, *lighting_model.m_directional_light);
        for (size_t j = 0; j < lighting_model.m_point_lights.size(); j++)
            set_array_point_light("lights", (int) (i + j), *lighting_model.m_point_lights.at(j));
    }

    unsigned int m_id{};

private:
    std::string m_placeholder;
    std::stringstream m_stream;

    const char* format_struct_name(const char* struct_name, const char* element_name) {
        m_stream << struct_name << "." << element_name;
        m_placeholder = m_stream.str();
        m_stream.str(std::string());
        return m_placeholder.c_str();
    }

    const char* format_array_name(const char* array_name, int array_index) {
        m_stream << array_name << "[" << array_index << "]";
        m_placeholder = m_stream.str();
        m_stream.str(std::string());
        return m_placeholder.c_str();
    }

    const char* format_array_struct_name(const char* array_name, int array_index, const char* element_name) {
        m_stream << array_name << "[" << array_index << "]." << element_name;
        m_placeholder = m_stream.str();
        m_stream.str(std::string());
        return m_placeholder.c_str();
    }

    static std::string read_file(const char* path);

    static bool is_successful(GLuint shader, GLenum type, const char* file = nullptr);
};

#endif // SHADER_H
