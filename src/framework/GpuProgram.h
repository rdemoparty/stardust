#pragma once

#include <GLheaders.h>
#include <map>
#include <string>
#include <vector>

namespace Acidrain {
    using namespace std;

    class GpuProgramConstantBundle;

    class GpuProgram {
    public:
        GpuProgram(const char* vertexShaderContent, const char* pixelShaderContent);
        GpuProgram(const string& vertexShaderContent, const string& pixelShaderContent);
        virtual ~GpuProgram();

        void addConstants(GpuProgramConstantBundle* bundle);

        void use();
        void unuse();

        int getUniform(const string& uniformName);

        void setMatrix3Uniform(float* value, const string& uniformName);

        void setMatrix4Uniform(float* value, const string& uniformName);

        void setVec2Uniform(float* value, const string& uniformName);

        void setVec3Uniform(float* value, const string& uniformName);

        void setVec4Uniform(float* value, const string& uniformName);

        void setFloatUniform(float value, const string& uniformName);

        void setIntUniform(int value, const string& uniformName);

    private:
        GLuint programId;
        GLuint shaderIds[2];
        map<string, int> uniformCache;
        vector<GpuProgramConstantBundle*> constantBundles;
    };
} // namespace Acidrain

