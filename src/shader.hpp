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
    /// <param name="vertexPath">Path to the file that has the vertex shader code</param>
    /// <param name="fragmentPath">Path to the file that has the fragment shader code</param>
    Shader(const char *vertexPath, const char *fragmentPath) {
        // Get source code from files
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            std::stringstream vShaderStream;
            vShaderStream.clear();
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();

            fShaderFile.open(fragmentPath);

            std::stringstream fShaderStream;
            fShaderStream.clear();
            fShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();
            fragmentCode = fShaderStream.str();
        }

        catch (std::ifstream::failure &e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << std::endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();


        // compile shaders
        unsigned int vertexShader, fragmentShader;
        int success;
        char infoLog[512];

        // vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        // error checking
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        // error checking
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertexShader);
        glAttachShader(ProgramID, fragmentShader);
        glLinkProgram(ProgramID);
        // error checking
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
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