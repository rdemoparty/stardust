#pragma once

#include <GLheaders.h>
#include <map>
#include <string>
#include <vector>

namespace Acidrain {

    class GpuProgramConstantBundle;

    class GpuProgram {
    public:

        GpuProgram(const char* vertexShaderContent, const char* pixelShaderContent);

        GpuProgram(const std::string& vertexShaderContent, const std::string& pixelShaderContent);

        virtual ~GpuProgram();

        void addConstants(GpuProgramConstantBundle* bundle);

        void use();

        void unuse();

        int getUniform(const char* uniformName);

        void setMatrix3Uniform(float* value, const char* uniformName);

        void setMatrix4Uniform(float* value, const char* uniformName);

        void setVec3Uniform(float* value, const char* uniformName);

        void setVec4Uniform(float* value, const char* uniformName);

        void setFloatUniform(float value, const char* uniformName);

        void setIntUniform(int value, const char* uniformName);

    private:
        GLuint programId;
        GLuint shaderIds[2];
        std::map<const char*, int> uniformCache;
        std::vector<GpuProgramConstantBundle*> constantBundles;
    };

} // namespace Acidrain

