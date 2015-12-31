#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <GpuProgram.h>
#include <glm/gtc/type_ptr.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GpuProgram;

    enum class GpuProgramConstantType {
        Matrix4,
        Integer,
        Vec2,
        Vec3,
        Vec4,
        Float
    };

    class GpuProgramConstant {
    public:
        GpuProgramConstantType type;
        mat4 matrix4Value;
        int intValue;
        vec2 vec2Value;
        vec3 vec3Value;
        vec4 vec4Value;
        float floatValue;

        GpuProgramConstant() {
        }

        GpuProgramConstant(const GpuProgramConstant& other) {
            type = other.type;
            matrix4Value = other.matrix4Value;
            intValue = other.intValue;
        }

        GpuProgramConstant(const mat4& value) {
            type = GpuProgramConstantType::Matrix4;
            matrix4Value = value;
        }

        GpuProgramConstant(int value) {
            type = GpuProgramConstantType::Integer;
            intValue = value;
        }

        GpuProgramConstant(float value) {
            type = GpuProgramConstantType::Float;
            floatValue = value;
        }

        GpuProgramConstant(vec2 value) {
            type = GpuProgramConstantType::Vec2;
            vec2Value = value;
        }

        GpuProgramConstant(vec3 value) {
            type = GpuProgramConstantType::Vec3;
            vec3Value = value;
        }

        GpuProgramConstant(vec4 value) {
            type = GpuProgramConstantType::Vec4;
            vec4Value = value;
        }
    };

    class GpuProgramConstantBundle {
    public:

        void set(string name, const GpuProgramConstant& constant) {
            constants[name] = constant;
        }

        void uploadTo(GpuProgram* program) {
            for (auto& kv : constants) {
                switch (kv.second.type) {
                    case GpuProgramConstantType::Matrix4:
                        program->setMatrix4Uniform(value_ptr(kv.second.matrix4Value), kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Integer:
                        program->setIntUniform(kv.second.intValue, kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Float:
                        program->setFloatUniform(kv.second.floatValue, kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Vec2:
                        program->setVec2Uniform(value_ptr(kv.second.vec2Value), kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Vec3:
                        program->setVec3Uniform(value_ptr(kv.second.vec3Value), kv.first.c_str());
                        break;
                    case GpuProgramConstantType::Vec4:
                        program->setVec4Uniform(value_ptr(kv.second.vec4Value), kv.first.c_str());
                        break;
                }
            }
        }

        map<string, GpuProgramConstant> constants;
    };
}