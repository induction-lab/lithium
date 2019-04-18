#ifndef __MATERIAL_H__
#define __MATERIAL_H__

class Material {
private:
    Texture *diffuseTexture; // base color
    Vector2 diffuseTiling;   // to scale texture (using shader)
    Vector2 diffuseOffset;   // to pan texture (using shader)
    Shader *shader;          // shader to render the mesh
    Vector4 tint;			 // tint for base texture
public:
    Material():
        diffuseTexture(NULL),
        shader(NULL),
        transparency(1.0f) {
        // shader = ShaderManager::GetInstance()->GetDefaultShader();
    }
    ~Material() {
        SAFE_DELETE(diffuseTexture);
        SAFE_DELETE(shader);
    }
    void setShader(Shader *materialShader) {
        shader = materialShader;
    }
    Shader *getShader() const {
        return shader;
    };
    void setDiffuseTexture(Texture *texture) {
        diffuseTexture = texture;
        diffuseTiling = Vector2(1.0f, 1.0f);
        diffuseOffset = Vector2(0.0f, 0.0f);
    }
    Texture *getDiffuseTexture() const {
        return diffuseTexture;
    };
    void SetTint(const Vector4 &color) {
        tint = color;
    }
    void setTint(float r, float g, float b, float a) {
        tint = Vector4(r, g, b, a);
    }
    void setDiffuseTextureTiling(const Vector2 &tiling) {
        diffuseTiling = tiling;
    }
    void setDiffuseTextureTiling(float x, float y) {
        diffuseTiling = Vector2(x, y);
    };
    void setDiffuseTextureOffset(const Vector2 &offset) {
        diffuseOffset = offset;
    }
    void setDiffuseTextureOffset(float x, float y) {
        diffuseOffset = Vector2(x, y);
    };
    // update shader uniforms
    void updateShader(Matrix *cameraMatrix, Matrix *modelMatrix) {
        if (shader == NULL) return;
        shader->apply();
        shader->SetUniformMatrix(shader->cameraProjViewMatrixLocation, *cameraMatrix);
        shader->SetUniformMatrix(shader->modelMatrixUniformLocation, *modelMatrix);
        shader->SetUniform4f("u_tintColor", tint.x, tint.y, tint.z, tint.w);
        shader->SetUniform4f("u_tiling_offset", diffuseTiling.x, diffuseTiling.y, diffuseOffset.x, diffuseOffset.y);
        shader->SetUniform1f("u_transparency", transparency);
        if (diffuseTexture != NULL) {
            diffuseTexture->apply();
            shader->SetUniform1i("u_diffuseTexture", 0);
        }
    };
    float transparency;
};

#endif

