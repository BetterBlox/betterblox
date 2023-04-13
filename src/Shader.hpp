/// shader.h
// By Ethan Naugle
// Created Nov 15, 2022
// Manages a vertex and fragment shader and bundles it into a single shader program.
//

#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ProgramID;

    /// <summary>
    /// Loading all the shader stuff
    /// </summary>
    /// <param name="vertex_path">Path to the file that has the vertex shader code</param>
    /// <param name="fragment_path">Path to the file that has the fragment shader code</param>
    Shader(const char *vertex_path, const char *fragment_path) {
        // Get source code from files
        std::string vertex_code;
        std::string fragment_code;
        std::ifstream v_shader_file;
        std::ifstream f_shader_file;

        v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            v_shader_file.open(vertex_path);
            std::stringstream v_shader_stream;
            v_shader_stream.clear();
            v_shader_stream << v_shader_file.rdbuf();
            v_shader_file.close();
            vertex_code = v_shader_stream.str();

            f_shader_file.open(fragment_path);

            std::stringstream f_shader_stream;
            f_shader_stream.clear();
            f_shader_stream << f_shader_file.rdbuf();
            f_shader_file.close();
            fragment_code = f_shader_stream.str();
        }

        catch (std::ifstream::failure &e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << std::endl;
        }

        const char *v_shader_code = vertex_code.c_str();
        const char *f_shader_code = fragment_code.c_str();


        // compile shaders
        unsigned int vertex_shader, fragment_shader;
        int success;
        char info_log[512];

        // vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &v_shader_code, NULL);
        glCompileShader(vertex_shader);
        // error checking
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << info_log << std::endl;
        }

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &f_shader_code, NULL);
        glCompileShader(fragment_shader);
        // error checking
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << info_log << std::endl;
        }

        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertex_shader);
        glAttachShader(ProgramID, fragment_shader);
        glLinkProgram(ProgramID);
        // error checking
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ProgramID, 512, NULL, info_log);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << info_log << std::endl;
        }
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void use() {
        glUseProgram(ProgramID);
    }

    // uniform setter functions
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), value);
    }

    int getId() {
        return ProgramID;
    }
};

#endif