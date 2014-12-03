#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include <GpuProgram.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GpuProgram;

    enum class GpuProgramConstantType {
        Matrix4,
        Integer
    };

    class GpuProgramConstant {
    public:
        GpuProgramConstantType type;
        mat4 matrix4Value;
        int intValue;

        GpuProgramConstant() {
        }

        GpuProgramConstant(const GpuProgramConstant& other) {
            type = other.type;
            matrix4Value = other.matrix4Value;
            intValue = other.intValue;
        }

        explicit GpuProgramConstant(const mat4& value) {
            type = GpuProgramConstantType::Matrix4;
            matrix4Value = value;
        }

        explicit GpuProgramConstant(int value) {
            type = GpuProgramConstantType::Integer;
            intValue = value;
        }
    };

    class GpuProgramConstantBundle {
    public:

        void add(string name, const GpuProgramConstant& constant) {
            constants[name] = constant;
        }

        void uploadTo(GpuProgram* program) {
            for (auto& kv : constants) {
                switch (kv.second.type) {
                    case GpuProgramConstantType::Matrix4:
                        program->setMatrix4Uniform(&kv.second.matrix4Value[0][0], kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Integer:
                        program->setIntUniform(kv.second.intValue, kv.first.c_str());
                        break;
                }
            }
        }

        map<string, GpuProgramConstant> constants;
    };
}