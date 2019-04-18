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
    float transparency;
	/*
    void SetDiffuseTexture(Texture *tex);
    void SetShader(Shader *shdr);
    void SetTint(const COLOR &color);
    void SetTint(float r, float g, float b, float a);

    void SetDiffuseTextureTiling(const Vector2 &tiling);
    void SetDiffuseTextureTiling(float x, float y);
    void SetDiffuseTextureOffset(const Vector2 &offset);
    void SetDiffuseTextureOffset(float x, float y);

    // update shader uniforms
    void UpdateShader(Matrix4 *cameraMatrix, Matrix4 *modelMatrix);

    Shader *GetShader() const;
    Texture *GetDiffuseTexture() const;
	*/
    Material();
    ~Material();
};

#endif

