#include <GpuProgram.h>
#include <iostream>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include "GpuProgramConstants.h"

namespace Acidrain {

    void checkCompileError(GLuint shaderId) {
        GLint isCompiled = 0;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

            //The maxLength includes the NULL character
            char* errorLog = new char[maxLength];
            glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

            std::cerr << "Shader error: " << errorLog << std::endl;

            glDeleteShader(shaderId); //Don't leak the shader.
            delete errorLog;
            return;
        }
    }


    GpuProgram::GpuProgram(const std::string& vertexShaderContent, const std::string& pixelShaderContent) :
            GpuProgram(vertexShaderContent.c_str(), pixelShaderContent.c_str()) {
    }

    GpuProgram::GpuProgram(const char* vertexShaderContent, const char* pixelShaderContent) {
        shaderIds[0] = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shaderIds[0], 1, &vertexShaderContent, nullptr);
        glCompileShader(shaderIds[0]);
        checkCompileError(shaderIds[0]);

        shaderIds[1] = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shaderIds[1], 1, &pixelShaderContent, nullptr);
        glCompileShader(shaderIds[1]);
        checkCompileError(shaderIds[1]);

        programId = glCreateProgram();
        glAttachShader(programId, shaderIds[0]);
        glAttachShader(programId, shaderIds[1]);
        glLinkProgram(programId);

//        glUseProgram(programId);
        // Be nice to others and do not let this
        // gpu program be set as default unless we really mean it
        glUseProgram(0);
    }

    GpuProgram::~GpuProgram() {
        glDetachShader(programId, shaderIds[0]);
        glDetachShader(programId, shaderIds[1]);

        glDeleteShader(shaderIds[0]);
        glDeleteShader(shaderIds[1]);

        glDeleteProgram(programId);
    }

    void GpuProgram::addConstants(GpuProgramConstantBundle* bundle) {
        constantBundles.push_back(bundle);
    }

    void GpuProgram::use() {
        glUseProgram(programId);
        for (auto& bundle : constantBundles)
            bundle->uploadTo(this);
    }

    void GpuProgram::unuse() {
        glUseProgram(0);
        glDisable(GL_TEXTURE_2D);
    }

    int GpuProgram::getUniform(const char* uniformName) {
        if (uniformCache.count(uniformName) == 0)
            uniformCache[uniformName] = glGetUniformLocation(programId, uniformName);

        return uniformCache[uniformName];
    }

    void GpuProgram::setMatrix3Uniform(float* matrix, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniformMatrix3fv(location, 1, false, matrix);
    }

    void GpuProgram::setMatrix4Uniform(float* matrix, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniformMatrix4fv(location, 1, false, matrix);
    }

    void GpuProgram::setVec3Uniform(float* value, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniform3fv(location, 1, value);
    }

    void GpuProgram::setVec4Uniform(float* value, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniform4fv(location, 1, value);
    }

    void GpuProgram::setIntUniform(int value, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniform1i(location, value);
    }

    void GpuProgram::setFloatUniform(float value, const char* uniformName) {
        GLint location = getUniform(uniformName);
        if (location != -1)
            glUniform1f(location, value);
    }

} // namespace Acidrain

