#pragma once


namespace Urho3D
{
    class   Texture2D;
    class   Font;
    class   VertexBuffer;
    class   IndexBuffer;
    class   Graphics;
    class   SpriteBatch : public Object
    {
    public:
        URHO3D_OBJECT(SpriteBatch, Object);

    public:
        
        struct VERTEX
        {
            Vector3 pos;
            unsigned color;
            Vector2 uv;
        };
        struct BATCH
        {
            Texture2D* texture;
            unsigned int start, count;
        };
        enum
        {
            SBFX_NONE = 0,
            SBFX_FLIPH = 1,
            SBFX_FLIPV = 2,
            SBFX_FLIPHV = 3,
        };

    public:
        SpriteBatch(Context *context);
        virtual ~SpriteBatch();
        bool Begin();
        bool Draw(Texture2D *texture, const Rect &dest, const Rect &src, const Color &color); // dest - world coordinates
        bool Draw(Texture2D *texture, const Rect &dest, const Rect &src, const Color &color, float rotation, const Vector2 &origin, const Vector2 &scale, int spriteEffect);
        void End();
        void SpriteBatch::Init(unsigned maxSprites, Camera* camera);

    private:
        BATCH* GetBatch(Texture2D *texture);
        IndexBuffer* indexBuffer_;
        VertexBuffer* vertexBuffer_;
        BATCH* batches_;
        unsigned numSprites_;
        unsigned numBatches_;
        VERTEX* vertices_;
        Camera* camera_;
        unsigned maxSprites_;
    };

}
