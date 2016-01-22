#include "Urho3DAll.h"
#include "SpriteBatch.h"


#define INDICES_PER_SPRITE 6
#define VERTICES_PER_SPRITE 4


namespace Urho3D
{
    SpriteBatch::SpriteBatch(Context *context) : Object(context)
    {
        indexBuffer_ = nullptr;
        vertexBuffer_ = nullptr;
        batches_ = nullptr;
        numSprites_ = 0;
        numBatches_ = 0;
        vertices_ = nullptr;
        camera_ = nullptr;
        maxSprites_ = 0;
    }

    SpriteBatch::~SpriteBatch()
    {
        if (batches_)
            delete[] batches_;
        if (vertexBuffer_)
            delete vertexBuffer_;
        if (indexBuffer_)
            delete indexBuffer_;
    }

    SpriteBatch::BATCH* SpriteBatch::GetBatch(Texture2D *texture)
    {
        BATCH* b;

        if (numBatches_ == 0)
        {
            b = &batches_[0];
            b->texture = texture;
            b->start = 0;
            b->count = 1;
            numBatches_ = 1;
            return b;
        }

        b = &batches_[numBatches_ - 1];
        if (b->texture == texture)
        {
            b->count++;
            return b;
        }

        b = &batches_[numBatches_];
        b->texture = texture;
        b->start = numSprites_ - 1;
        b->count = 1;
        numBatches_++;
        return b;
    }

    bool SpriteBatch::Begin()
    {
        if (vertices_)
            return false;
        vertices_ = (VERTEX*)vertexBuffer_->Lock(0, vertexBuffer_->GetVertexCount());
        return (vertices_ != nullptr);
    }

    void SpriteBatch::End()
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        ShaderVariation* vs = graphics->GetShader(VS, "Basic", "DIFFMAP VERTEXCOLOR");
        ShaderVariation* ps = graphics->GetShader(PS, "Basic", "DIFFMAP VERTEXCOLOR");
        graphics->SetBlendMode(BLEND_ALPHA);
        graphics->SetColorWrite(true);
        graphics->SetCullMode(CULL_NONE);  // вкл вид сзади нужно изменить лицевую сторону полигонов, чтобы их было видно без этой функции (она замедляет)
        graphics->SetDepthWrite(true);
        graphics->SetScissorTest(false);
        graphics->SetStencilTest(false);
        graphics->SetShaders(vs, ps);
        graphics->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
        graphics->SetShaderParameter(VSP_VIEWPROJ, camera_->GetProjection() * camera_->GetView());
        graphics->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));
        graphics->SetVertexBuffer(vertexBuffer_);
        graphics->SetIndexBuffer(indexBuffer_);
        //graphics_->SetDepthTest(CMP_LESSEQUAL);
        //graphics_->SetDepthTest(CMP_ALWAYS);

        vertexBuffer_->Unlock();
        vertices_ = nullptr;

        for (unsigned int i = 0; i < numBatches_; i++)
        {
            BATCH *b = &batches_[i];
            graphics->SetTexture(0, b->texture);
            graphics->Draw(TRIANGLE_LIST, b->start * INDICES_PER_SPRITE, b->count * INDICES_PER_SPRITE, 0, b->count * VERTICES_PER_SPRITE);
        }
        numSprites_ = 0;
        numBatches_ = 0;
    }

    bool SpriteBatch::Draw(Texture2D* texture, const Rect& dest, const Rect& src, const Color& color)
    {
        if (!texture)
            return false;

        if (numSprites_ >= maxSprites_)
            return false;

        numSprites_++; // necessary before GetBatch()

        BATCH *batch = GetBatch(texture);

        unsigned int i = (numSprites_ - 1) * VERTICES_PER_SPRITE;
        
        float z = i * -0.0001f;
        vertices_[i + 0].pos = Vector3(dest.min_.x_, dest.min_.y_, z);
        vertices_[i + 1].pos = Vector3(dest.max_.x_, dest.min_.y_, z);
        vertices_[i + 2].pos = Vector3(dest.min_.x_, dest.max_.y_, z);
        vertices_[i + 3].pos = Vector3(dest.max_.x_, dest.max_.y_, z);

        vertices_[i + 0].color = vertices_[i + 1].color = vertices_[i + 2].color = vertices_[i + 3].color = color.ToUInt();

        float w = 1.0f / texture->GetWidth();
        float h = 1.0f / texture->GetHeight();
        vertices_[i + 0].uv = Vector2(src.min_.x_ * w, src.min_.y_ * h);
        vertices_[i + 1].uv = Vector2(src.max_.x_ * w, src.min_.y_ * h);
        vertices_[i + 2].uv = Vector2(src.min_.x_ * w, src.max_.y_ * h);
        vertices_[i + 3].uv = Vector2(src.max_.x_ * w, src.max_.y_ * h);

        return true;
    }

    bool SpriteBatch::Draw(Texture2D *texture, const Rect &dest, const Rect &src, const Color &color, float rotation, const Vector2 &origin, const Vector2 &scale, int spriteEffect)
    {
        if (!texture)
            return false;

        if (numSprites_ >= maxSprites_)
            return false;

        numSprites_++; // necessary before GetBatch()

        BATCH *batch = GetBatch(texture);

        unsigned int i = (numSprites_ - 1) * VERTICES_PER_SPRITE;

        Vector2 startxy = -origin;
        Vector2 toxy = startxy + dest.Size();

        float z = i * -0.0001f;
        vertices_[i + 0].pos = Vector3(startxy.x_, startxy.y_, z);
        vertices_[i + 1].pos = Vector3(toxy.x_, startxy.y_, z);
        vertices_[i + 2].pos = Vector3(startxy.x_, toxy.y_, z);
        vertices_[i + 3].pos = Vector3(toxy.x_, toxy.y_, z);

        Matrix4 m(Matrix4::IDENTITY);
        float cos = cosf(rotation);
        float sin = sinf(rotation);
        m.m00_ = cos * scale.x_;
        m.m10_ = sin * scale.x_;
        m.m01_ = -sin * scale.y_;
        m.m11_ = cos * scale.y_;
        m.m03_ = dest.min_.x_;
        m.m13_ = dest.min_.y_;
        for (int j = 0; j < 4; j++)
            vertices_[i + j].pos = m * vertices_[i + j].pos;

        vertices_[i + 0].color = vertices_[i + 1].color = vertices_[i + 2].color = vertices_[i + 3].color = color.ToUInt();

        float w = 1.0f / texture->GetWidth();
        float h = 1.0f / texture->GetHeight();

        Vector2 uv1 = src.min_;
        Vector2 uv2 = src.max_;
        
        if (spriteEffect & SBFX_FLIPH)
        {
            uv1.x_ = src.max_.x_;
            uv2.x_ = src.min_.x_;
        }

        if (spriteEffect & SBFX_FLIPV)
        {
            uv1.y_ = src.max_.y_;
            uv2.y_ = src.min_.y_;
        }

        vertices_[i + 0].uv = Vector2(uv1.x_ * w, uv1.y_ * h);
        vertices_[i + 1].uv = Vector2(uv2.x_ * w, uv1.y_ * h);
        vertices_[i + 2].uv = Vector2(uv1.x_ * w, uv2.y_ * h);
        vertices_[i + 3].uv = Vector2(uv2.x_ * w, uv2.y_ * h);

        return true;
    }

    void SpriteBatch::Init(unsigned maxSprites, Camera* camera)
    {
        camera_ = camera;
        maxSprites_ = maxSprites;
        indexBuffer_ = new IndexBuffer(context_);
        indexBuffer_->SetSize(maxSprites_ * INDICES_PER_SPRITE, true);

        unsigned* index = (unsigned*)indexBuffer_->Lock(0, indexBuffer_->GetIndexCount());
        for (unsigned i = 0; i < maxSprites_; i++)
        {
            index[i * INDICES_PER_SPRITE + 0] = i * VERTICES_PER_SPRITE + 0;
            index[i * INDICES_PER_SPRITE + 1] = i * VERTICES_PER_SPRITE + 1;
            index[i * INDICES_PER_SPRITE + 2] = i * VERTICES_PER_SPRITE + 2;
            index[i * INDICES_PER_SPRITE + 3] = i * VERTICES_PER_SPRITE + 2;
            index[i * INDICES_PER_SPRITE + 4] = i * VERTICES_PER_SPRITE + 1;
            index[i * INDICES_PER_SPRITE + 5] = i * VERTICES_PER_SPRITE + 3;
        }
        indexBuffer_->Unlock();

        vertexBuffer_ = new VertexBuffer(context_);
        vertexBuffer_->SetSize(maxSprites_ * VERTICES_PER_SPRITE, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1);

        batches_ = new BATCH[maxSprites_];
    }
}


