#pragma once

#include <ecs/util/concepts.hpp>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include <array>
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace nexus
{

    template <typename GLtype>
    concept UniformValueType = requires {
        requires std::is_fundamental_v<GLtype>;
        requires ecs::util::OneOf<GLtype, gl::GLfloat, gl::GLdouble, gl::GLint, gl::GLuint, bool>;
    };

    template <typename Float>
    concept UniformMatType = ecs::util::OneOf<Float, gl::GLfloat, gl::GLdouble>;

    class Shader
    {
    public:
        Shader(
            std::filesystem::path                vs_path,
            std::filesystem::path                fs_path,
            std::optional<std::filesystem::path> gs_path = {}
        )
            : m_id{ gl::glCreateProgram() }    // create program
        {
            std::string   vs_source;
            std::ifstream vs_file{ vs_path };
            if (!vs_file) {
                std::cerr << "Error reading vertex shader file: " << vs_path << '\n';
            } else {
                std::stringstream buffer;
                buffer << vs_file.rdbuf();
                vs_source = buffer.str();
            }

            std::string   fs_source;
            std::ifstream fsFile{ fs_path };
            if (!fsFile) {
                std::cerr << "Error reading fragment shader file: " << fs_path << '\n';
            } else {
                std::stringstream buffer;
                buffer << fsFile.rdbuf();
                fs_source = buffer.str();
            }

            auto vs_id = prepare_shader(vs_source, ShaderStage::VERTEX);
            auto fs_id = prepare_shader(fs_source, ShaderStage::FRAGMENT);

            auto gs_id = [&]() -> std::optional<gl::GLuint> {
                if (gs_path) {
                    std::string   gs_source;
                    std::ifstream gs_file{ gs_path.value() };
                    if (!gs_file) {
                        std::cerr << "Error reading fragment shader file: " << gs_path.value() << '\n';
                        return {};
                    } else {
                        std::stringstream buffer;
                        buffer << gs_file.rdbuf();
                        gs_source = buffer.str();
                    }
                    return prepare_shader(gs_source, ShaderStage::GEOMETRY);
                } else {
                    return {};
                }
            }();

            // link shaders to shader program
            gl::glAttachShader(m_id, vs_id);
            gl::glAttachShader(m_id, fs_id);
            if (gs_id) {
                gl::glAttachShader(m_id, gs_id.value());
            }
            gl::glLinkProgram(m_id);
            shader_link_info(m_id);

            // delete shader objects
            gl::glDeleteShader(vs_id);
            gl::glDeleteShader(fs_id);
            if (gs_id) {
                gl::glDeleteShader(gs_id.value());
            }
        }

        Shader(Shader&& other)
            : m_id{ std::exchange(other.m_id, 0) }
            , m_uniform_loc_history{ std::exchange(other.m_uniform_loc_history, {}) }
        {
        }

        Shader& operator=(Shader&& other)
        {
            if (this == &other) {
                return *this;
            }

            if (m_id != 0) {
                gl::glDeleteProgram(m_id);
            }

            m_id                  = std::exchange(other.m_id, 0);
            m_uniform_loc_history = std::exchange(other.m_uniform_loc_history, {});

            return *this;
        }

        Shader(const Shader&)            = delete;
        Shader& operator=(const Shader&) = delete;

        ~Shader() { gl::glDeleteProgram(m_id); }

        gl::GLuint                       m_id;
        std::map<std::string, gl::GLint> m_uniform_loc_history;    // last location of uniform

        void use() const { gl::glUseProgram(m_id); }
        void unuse() const { gl::glUseProgram(0); }

        // glm vector
        // clang-format off
        template <UniformValueType Type> void set_uniform(const std::string& name, const glm::vec<2, Type>& vec) { set_uniform_vec_impl<Type, 2>(name, &vec[0]); }
        template <UniformValueType Type> void set_uniform(const std::string& name, const glm::vec<3, Type>& vec) { set_uniform_vec_impl<Type, 3>(name, &vec[0]); }
        template <UniformValueType Type> void set_uniform(const std::string& name, const glm::vec<4, Type>& vec) { set_uniform_vec_impl<Type, 4>(name, &vec[0]); }

        // glm::matrix
        template <UniformMatType Type> void set_uniform(const std::string& name, const glm::mat<2, 2, Type>& mat2) { set_uniform_mat_impl<Type, 2>(name, mat2); }
        template <UniformMatType Type> void set_uniform(const std::string& name, const glm::mat<3, 3, Type>& mat3) { set_uniform_mat_impl<Type, 3>(name, mat3); }
        template <UniformMatType Type> void set_uniform(const std::string& name, const glm::mat<4, 4, Type>& mat4) { set_uniform_mat_impl<Type, 4>(name, mat4); }

        // simple array; 2 to 4 elements
        template <UniformValueType Type, std::size_t N> requires(N >= 2 && N <= 4) void set_uniform(const std::string& name, const std::array<Type, N>& value) { set_uniform_vec_impl<Type, N>(name, &value[0]); }
        // clang-format on

        // one value
        template <UniformValueType Type>
        void set_uniform(const std::string& name, Type value)
        {
            auto loc = get_loc(name);

            // clang-format off
            if      constexpr (std::same_as<Type, gl::GLfloat>)  gl::glUniform1f(loc, value);
            else if constexpr (std::same_as<Type, gl::GLdouble>) gl::glUniform1d(loc, value);
            else if constexpr (std::same_as<Type, gl::GLint>)    gl::glUniform1i(loc, value);
            else if constexpr (std::same_as<Type, bool>)         gl::glUniform1i(loc, value);
            else if constexpr (std::same_as<Type, gl::GLuint>)   gl::glUniform1ui(loc, value);
            // clang-format on
        }

        // two values (use array)
        template <UniformValueType Type>
        void set_uniform(const std::string& name, Type v0, Type v1)
        {
            std::array value{ v0, v1 };
            set_uniform<Type, 2>(name, value);
        }

        // three values (use array)
        template <UniformValueType Type>
        void set_uniform(const std::string& name, Type v0, Type v1, Type v2)
        {
            std::array value{ v0, v1, v2 };
            set_uniform<Type, 3>(name, value);
        }

        // four values (use array)
        template <UniformValueType Type>
        void set_uniform(const std::string& name, Type v0, Type v1, Type v2, Type v3)
        {
            std::array value{ v0, v1, v2, v3 };
            set_uniform<Type, 4>(name, value);
        }

    private:
        enum class ShaderStage
        {
            VERTEX,
            FRAGMENT,
            GEOMETRY,
        };

        gl::GLint get_loc(const std::string& name)
        {
            if (auto found = m_uniform_loc_history.find(name); found != m_uniform_loc_history.end()) {
                return found->second;
            } else {
                gl::GLint loc{ gl::glGetUniformLocation(m_id, name.c_str()) };
                if (loc == -1) {
                    std::cerr << std::format(
                        "WARNING: [Shader] [{}]: Uniform of name '{}' can't be found\n", m_id, name
                    );
                }
                m_uniform_loc_history.emplace(name, loc);
                return loc;
            }
        }

        void shader_compile_info(gl::GLuint shader, ShaderStage stage)
        {
            std::string_view name;
            switch (stage) {
            case ShaderStage::VERTEX: name = "VERTEX"; break;
            case ShaderStage::FRAGMENT: name = "FRAGMENT"; break;
            case ShaderStage::GEOMETRY: name = "GEOMETRY"; break;
            }

            gl::GLint status{};
            gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &status);
            if (status != 1) {
                gl::GLint max_length{};
                gl::GLint log_length{};

                gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &max_length);
                auto log = new gl::GLchar[(std::size_t)max_length];
                gl::glGetShaderInfoLog(shader, max_length, &log_length, log);
                std::cerr << std::format("Shader compilation of type {} failed:\n{}\n", name, log);
                delete[] log;
            }
        }

        void shader_link_info(gl::GLuint program)
        {
            gl::GLint status{};
            gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &status);
            if (status != 1) {
                gl::GLint max_length{};
                gl::GLint log_length{};

                gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &max_length);
                auto log = new gl::GLchar[(std::size_t)max_length];
                gl::glGetProgramInfoLog(program, max_length, &log_length, log);
                std::cerr << "Program linking failed: \n" << log << '\n';
                delete[] log;
            }
        }

        gl::GLuint prepare_shader(const std::string& source, ShaderStage stage)
        {
            gl::GLenum type;
            switch (stage) {
            case ShaderStage::VERTEX: type = gl::GL_VERTEX_SHADER; break;
            case ShaderStage::FRAGMENT: type = gl::GL_FRAGMENT_SHADER; break;
            case ShaderStage::GEOMETRY: type = gl::GL_GEOMETRY_SHADER; break;
            }

            // compile vertex shader
            auto  id         = gl::glCreateShader(type);
            auto* source_ptr = source.c_str();
            gl::glShaderSource(id, 1, &source_ptr, nullptr);
            gl::glCompileShader(id);
            shader_compile_info(id, stage);

            return id;
        }

        // vector
        template <UniformValueType Type, std::size_t N>
            requires (N >= 2 && N <= 4)
        void set_uniform_vec_impl(const std::string& name, const Type* value)
        {
            gl::GLint loc{ get_loc(name) };

            // another C limitation, the const does not matter
            auto val{ const_cast<Type*>(value) };

            // clang-format off
            if      constexpr (std::same_as<Type, gl::GLfloat>  && N == 2) gl::glUniform2fv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 3) gl::glUniform3fv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLfloat>  && N == 4) gl::glUniform4fv(loc, 1, val);

            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 2) gl::glUniform2dv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 3) gl::glUniform3dv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 4) gl::glUniform4dv(loc, 1, val);

            else if constexpr (std::same_as<Type, gl::GLint>    && N == 2) gl::glUniform2iv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLint>    && N == 3) gl::glUniform3iv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLint>    && N == 4) gl::glUniform4iv(loc, 1, val);

            else if constexpr (std::same_as<Type, bool>         && N == 2) gl::glUniform2iv(loc, 1, val);
            else if constexpr (std::same_as<Type, bool>         && N == 3) gl::glUniform3iv(loc, 1, val);
            else if constexpr (std::same_as<Type, bool>         && N == 4) gl::glUniform4iv(loc, 1, val);

            else if constexpr (std::same_as<Type, gl::GLuint>   && N == 2) gl::glUniform2uiv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLuint>   && N == 3) gl::glUniform3uiv(loc, 1, val);
            else if constexpr (std::same_as<Type, gl::GLuint>   && N == 4) gl::glUniform4uiv(loc, 1, val);
            // clang-format on
        }

        // matrix
        template <UniformMatType Type, std::size_t N>
            requires (N >= 2 && N <= 4)
        void set_uniform_mat_impl(const std::string& name, const glm::mat<N, N, Type>& mat)
        {
            gl::GLint loc{ get_loc(name) };
            // clang-format off
            if      constexpr (std::same_as<Type, gl::GLfloat> && N == 2) gl::glUniformMatrix2fv(loc, 1, gl::GL_FALSE, &mat[0][0]);
            else if constexpr (std::same_as<Type, gl::GLfloat> && N == 3) gl::glUniformMatrix3fv(loc, 1, gl::GL_FALSE, &mat[0][0]);
            else if constexpr (std::same_as<Type, gl::GLfloat> && N == 4) gl::glUniformMatrix4fv(loc, 1, gl::GL_FALSE, &mat[0][0]);

            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 2) gl::glUniformMatrix2dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 3) gl::glUniformMatrix3dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
            else if constexpr (std::same_as<Type, gl::GLdouble> && N == 4) gl::glUniformMatrix4dv(loc, 1, gl::GL_FALSE, &mat[0][0]);
            // clang-format on
        }
    };
}
