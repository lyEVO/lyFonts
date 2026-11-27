/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stddef.h>

/* FreeType shim */

static FT_Library _library = NULL;
static FT_Face _faces[8];
static int _faceCount = 0;

int lysis_ft_init(void)
{
    if (_library != NULL)
        return 0;
    return FT_Init_FreeType(&_library);
}

int lysis_ft_load_face(const unsigned char* data, long size)
{
    FT_Face face;

    if (_library == NULL || _faceCount >= 8)
        return -1;
    if (FT_New_Memory_Face(_library, data, size, 0, &face) != 0)
        return -1;

    _faces[_faceCount] = face;
    return _faceCount++;
}

int lysis_ft_set_size(int face, int pixelSize)
{
    if (face < 0 || face >= _faceCount || pixelSize <= 0)
        return -1;
    return FT_Set_Pixel_Sizes(_faces[face], 0, (FT_UInt)pixelSize);
}

int lysis_ft_line_height(int face)
{
    if (face < 0 || face >= _faceCount)
        return -1;
    return (int)(_faces[face]->size->metrics.height >> 6);
}

int lysis_ft_ascent(int face)
{
    if (face < 0 || face >= _faceCount)
        return -1;
    return (int)(_faces[face]->size->metrics.ascender >> 6);
}

int lysis_ft_descent(int face)
{
    if (face < 0 || face >= _faceCount)
        return -1;
    return (int)(_faces[face]->size->metrics.descender >> 6);
}

static int lysis_ft_copy_name(const char* src, char* buf, int capacity)
{
    int i = 0;
    if (src == NULL || buf == NULL || capacity <= 0)
        return -1;
    while (src[i] != '\0' && i < capacity - 1)
    {
        buf[i] = src[i];
        i++;
    }
    buf[i] = '\0';
    return 0;
}

int lysis_ft_family_name(int face, char* buf, int capacity)
{
    if (face < 0 || face >= _faceCount)
        return -1;
    return lysis_ft_copy_name(_faces[face]->family_name, buf, capacity);
}

int lysis_ft_style_name(int face, char* buf, int capacity)
{
    if (face < 0 || face >= _faceCount)
        return -1;
    return lysis_ft_copy_name(_faces[face]->style_name, buf, capacity);
}

int lysis_ft_render(int face, unsigned long charcode,
                    unsigned char* outBitmap, int outCapacity,
                    int* outWidth, int* outRows,
                    int* outLeft, int* outTop, int* outAdvance)
{
    FT_Face f;
    FT_Bitmap* bmp;
    int needed;
    int y;

    if (face < 0 || face >= _faceCount)
        return -1;

    f = _faces[face];

    if (FT_Load_Char(f, charcode, FT_LOAD_DEFAULT) != 0)
        return -1;
    if (FT_Render_Glyph(f->glyph, FT_RENDER_MODE_NORMAL) != 0)
        return -1;

    bmp = &f->glyph->bitmap;
    if (bmp->pixel_mode != FT_PIXEL_MODE_GRAY)
        return -1;

    needed = bmp->width * bmp->rows;

    *outWidth = bmp->width;
    *outRows = bmp->rows;
    *outLeft = f->glyph->bitmap_left;
    *outTop = f->glyph->bitmap_top;
    *outAdvance = (int)((f->glyph->advance.x + 32) >> 6);

    if (outBitmap == NULL || outCapacity < needed)
        return -2;

    if (bmp->pitch >= 0)
    {
        for (y = 0; y < bmp->rows; y++)
        {
            unsigned char* src = bmp->buffer + y * bmp->pitch;
            unsigned char* dst = outBitmap + y * bmp->width;
            int x;
            for (x = 0; x < bmp->width; x++)
                dst[x] = src[x];
        }
    }
    else
    {
        for (y = 0; y < bmp->rows; y++)
        {
            unsigned char* src = bmp->buffer + (bmp->rows - 1 - y) * (-bmp->pitch);
            unsigned char* dst = outBitmap + y * bmp->width;
            int x;
            for (x = 0; x < bmp->width; x++)
                dst[x] = src[x];
        }
    }

    return 0;
}

int lysis_ft_kerning(int face, unsigned long left, unsigned long right)
{
    FT_Face f;
    FT_UInt leftGlyph;
    FT_UInt rightGlyph;
    FT_Vector delta;

    if (face < 0 || face >= _faceCount)
        return 0;

    f = _faces[face];
    leftGlyph = FT_Get_Char_Index(f, left);
    rightGlyph = FT_Get_Char_Index(f, right);
    if (leftGlyph == 0 || rightGlyph == 0)
        return 0;
    if (FT_Get_Kerning(f, leftGlyph, rightGlyph, FT_KERNING_DEFAULT, &delta) != 0)
        return 0;
    return (int)((delta.x + 32) >> 6);
}

int lysis_ft_has_glyph(int face, unsigned long charcode)
{
    if (face < 0 || face >= _faceCount)
        return 0;
    return FT_Get_Char_Index(_faces[face], charcode) != 0 ? 1 : 0;
}

void lysis_ft_done(void)
{
    int i;
    for (i = 0; i < _faceCount; i++)
        FT_Done_Face(_faces[i]);
    _faceCount = 0;
    if (_library != NULL)
    {
        FT_Done_FreeType(_library);
        _library = NULL;
    }
}
