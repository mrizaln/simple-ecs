#pragma once

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace nexus
{
    class CubePrimitive
    {
    public:
        static constexpr std::size_t num_of_vertices = 36;

        CubePrimitive(float side_length)
            : m_side_length{ side_length }
            , m_vertices{ std::make_unique<VertexData[]>(num_of_vertices) }
        {
            for (std::size_t i = 0; i < num_of_vertices; ++i) {
                m_vertices[i].m_position = cube_vertices[i] * m_side_length / 2.0f;
                m_vertices[i].m_normal   = cube_normals[i];
#ifdef SHADER_ENABLE_TEXTURE
                m_vertices[i].m_tex_coord = cube_tex_coords[i];
#endif
            }

            set_buffers();
        }

        CubePrimitive(CubePrimitive&& other)
            : m_vao{ std::exchange(other.m_vao, 0) }
            , m_vbo{ std::exchange(other.m_vbo, 0) }
        {
        }

        CubePrimitive& operator=(CubePrimitive&& other)
        {
            if (this == &other) {
                return *this;
            }

            delete_buffers();

            m_vao = std::exchange(other.m_vao, 0);
            m_vbo = std::exchange(other.m_vbo, 0);

            return *this;
        }

        CubePrimitive(const CubePrimitive&)            = delete;
        CubePrimitive& operator=(const CubePrimitive&) = delete;

        // cube destructor must be done before glfwTerminate() is called
        ~CubePrimitive() { delete_buffers(); }

        void draw() const
        {
            gl::glBindVertexArray(m_vao);
            gl::glDrawArrays(gl::GL_TRIANGLES, 0, static_cast<gl::GLsizei>(num_of_vertices));
            gl::glBindVertexArray(0);
        }

        void delete_buffers()
        {
            if (m_vao != 0) {
                gl::glDeleteVertexArrays(1, &m_vao);
            }
            if (m_vbo != 0) {
                gl::glDeleteBuffers(1, &m_vbo);
            }
        }

    private:
        struct VertexData
        {
            glm::vec3 m_position;
            glm::vec3 m_normal;
#ifdef SHADER_ENABLE_TEXTURE
            glm::vec2 m_tex_coord;
#endif
        };

        void set_buffers()
        {
            gl::glGenVertexArrays(1, &m_vao);
            gl::glGenBuffers(1, &m_vbo);

            // bind
            //----
            gl::glBindVertexArray(m_vao);
            gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vbo);
            gl::glBufferData(
                gl::GL_ARRAY_BUFFER,
                static_cast<gl::GLsizei>(sizeof(VertexData) * num_of_vertices),
                m_vertices.get(),
                gl::GL_STATIC_DRAW
            );

            // vertex attribute
            //-----------------
            auto stride = static_cast<gl::GLsizei>(sizeof(VertexData));
            gl::glVertexAttribPointer(
                0,
                decltype(VertexData::m_position)::length(),
                gl::GL_FLOAT,
                gl::GL_FALSE,
                stride,
                (void*)(offsetof(VertexData, m_position))
            );
            gl::glEnableVertexAttribArray(0);

            gl::glVertexAttribPointer(
                1,
                decltype(VertexData::m_normal)::length(),
                gl::GL_FLOAT,
                gl::GL_FALSE,
                stride,
                (void*)(offsetof(VertexData, m_normal))
            );
            gl::glEnableVertexAttribArray(1);

#ifdef SHADER_ENABLE_TEXTURE
            gl::glVertexAttribPointer(
                2,
                decltype(VertexData::m_tex_coord)::length(),
                gl::GL_FLOAT,
                gl::GL_FALSE,
                stride,
                (void*)(offsetof(VertexData, m_tex_coord))
            );
            gl::glEnableVertexAttribArray(2);
#endif

            // unbind
            //----
            gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
            gl::glBindVertexArray(0);
        }

        // clang-format off
        inline static constexpr std::array<glm::vec3, num_of_vertices> cube_vertices{ {
            { -1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f },
            { -1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f },
            { -1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f },
            {  1.0f,  1.0f,  1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f },
            { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f, -1.0f },
            { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f },
        } };
        // clang-format on

        // clang-format off
        inline static constexpr std::array<glm::vec3, num_of_vertices> cube_normals{ {
            {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
            {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
            { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },
            {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },
            {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
            {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }
        } };
        // clang-format on

        // clang-format off
        inline static constexpr std::array<glm::vec2, num_of_vertices> cube_tex_coords{ {
            { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f },     // back face
            { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f },     // front face
            { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f },     // left face
            { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },     // right face
            { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f },     // bottom face
            { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }      // top face
        } };
        // clang-format on

    private:
        float                         m_side_length;
        std::unique_ptr<VertexData[]> m_vertices;
        unsigned int                  m_vao;
        unsigned int                  m_vbo;
    };
}
